/*############################################################################*\
#         _   ___ _____ _   ___ ___ __  __ _   _ _      _ _____ ___  ___       #
#        /_\ | _ \_   _/ | / __|_ _|  \/  | | | | |    /_\_   _/ _ \| _ \      #
#       / _ \|   / | | | | \__ \| || |\/| | |_| | |__ / _ \| || (_) |   /      #
#      /_/ \_\_|_\ |_| |_| |___/___|_|  |_|\___/|____/_/ \_\_| \___/|_|_\      #
#                                                                              #
#                                                          by Mathieu FOURCROY #
#                                                                         2015 #
\*############################################################################*/
/**
 * @file main.c
 * @author Mathieu Fourcroy
 * @date June 2015
 * @version 0.0.1
 *
 * @todo Change the data structures to make it more readable.
 */

/*=====| INCLUDES |===========================================================*/
#include <math.h>
#include <time.h>
#include "io.h"
#include "utls.h"
#include "art1.h"
#include "ccl_internal.h"

/*=====| FUNCTIONS |==========================================================*/
/** Remove empty patterns from the patterns list.
 *
 * Scan every patterns and remove the patterns made of 0 only. Such a pattern is
 * useless for the network and can cause an infitie loop.
 *
 * @param[in] lines The patterns list to check.
 *
 * @return The number of empty patterns removed.
 */
static ulong rm_empty_pats(List *lines){
    ulong i, j, aftersize, count;
	ulong beforesize = iList.Size(lines);
    ulong lineSize = vec_size((*(CSVLine *)iList.Front(lines)).val);
    Vector *val;

    printf("Removing patterns containing only 0... ");
    for(i = 0; i < beforesize; i++){
        count = 0;
        val = (*(CSVLine *)iList.GetElement(lines, i)).val;
        for(j = 0; j < lineSize; j++){
            if(vec_get_as_char(val, j) == 0){
                count++;
            }
        }
        if(count == lineSize){
            iList.EraseAt(lines, i);
        }
    }
    aftersize = iList.Size(lines);	
    printf("OK\n");
    return beforesize - aftersize;
}

/** Check the patterns validity.
 *
 * Check:
 *  - if there's enough patterns in the patterns list (at least 2).
 *  - if patterns aren't 0000000... (in that case: remove them).
 *
 * @param[in] lines The patterns list to check.
 *
 * @return The number of empty patterns removed.
 */
static ulong check_pats_validity(List *lines){
    ulong nbEmptyPats;

    if(iList.Size(lines) < 1){
        fprintf(stderr, "ERROR: There is %lu patterns. There must be "\
                "at least 2 patterns\nExiting...\n", iList.Size(lines));
        exit(10);
    }
    nbEmptyPats = rm_empty_pats(lines);
    if(nbEmptyPats > 0){
        printf("\nWARNING: %lu patterns has been removed\n", nbEmptyPats);
    }
    if(iList.Size(lines) < 1){
        fprintf(stderr, "ERROR: There is %lu patterns (%lu patterns has been "\
                "removed). There must be at least 2 patterns\nExiting...\n", 
                iList.Size(lines), nbEmptyPats);
        exit(10);
    }
    return nbEmptyPats;
}


/** Add noise to each patterns of the patterns vector.
 *
 * The noise is added by flipping random bits of the patterns.
 *
 * @param[in,out] pats The patterns which will be "noised".
 * @param[in]     perc The noise percentage to add to the patterns.
 */
static void add_noise(Vector *pats, int perc){
    char *bit;
    Vector *pat;
    ulong n, x, iPat, size, noise;
    ulong c = 0;

    printf("Adding %i%% of noise to each patterns... ", perc);
    if(perc == 0){
        printf("OK\n");
        return;
    }
    if(vec_size(pats) < 1){    // should never happen
        printf("OK\n");
        fprintf(stderr, "WARNING: 'pats' is empty\n");
        return;
    }
    srand((unsigned int)time(NULL));
    size = vec_size(vec_get_as_vec(pats, 0));
    noise = (ulong)roundf(perc * size / 100.);
    for(iPat = 0; iPat < vec_size(pats); iPat++){
        pat = vec_get_as_vec(pats, iPat);
        for(n = 0; n < noise; n++){
            x = rand() % size;
            bit = &vec_get_as_char(pat, x);
            if(*bit == 1){
                *bit = 0;
            }
            else{
                *bit = 1;
            }
            c++;
        }
    }
    printf("OK\n");
    printf("%lu pattern's bits have been switched to 0\n", c);
}

/** Check weither every patterns are maid of binary numbers (0/1).
 *
 * The function dosen't return any boolean because it automaticaly exit the
 * program if the patterns aren't binary: it's impossible to build an ART1
 * network with non-binary patterns.
 *
 * @param[in] lines The list of patterns to check.
 * @param[in] skip  Indicate weither the first attribute of the csv string must
 *  be skipped or not.
 */
static void check_binary(List *lines, bool skip){
    List *itline;
    itline = iList.Copy(lines);
    CSVLine line;
    line = *(CSVLine *)iList.GetElement(itline, 0);
    const ulong size = vec_size(line.val);
    ulong index = 1;
    ulong i, j;

    for(i = 0; i < iList.Size(lines); i++){
        line = *(CSVLine *)iList.GetElement(itline, i);
        for(j = 0; j < size; j++){
            int val = vec_get_as_char(line.val, j);
            if((val != 0) && (val != 1) && (j > skip)){
                fprintf(stderr, "\nERROR: value at the line %lu, column %lu "\
                        "is %i. It can only be 1 or 0 (binary patterns)"\
                        "\nWhole data are in data_tmp.\n Exiting...\n", 
                        index, j + 1, val);
                iList.Finalize(itline);
                exit(22);
            }
        }
        index++;
    }
    iList.Finalize(itline);
}

/** The main function.
 *
 * This is the main function of the program. It fetch the network parameters,
 * create the training patterns for the input file, train the network using the
 * given parameters and then create the test patterns from the input test file
 * and test them on the network.
 */
static void train_network(ulong *patLen, Vector **bestClusts,
                          Vector **clustsClasses, InParam par){
    Vector *pats;       // Vector of vectors
    List *lines;        // CSVLine
    Vector *trainClasses = NULL;    // Vector of strings
    float resFluc;
    ulong emptyPats, iLine;
    FILE *in;

    lines = iList.Create(sizeof(CSVLine));
    printf("\n------------- INTERNING TRAINING PATTERNS ------------\n\n");
    openFile(&in, par.trainFile, "r");
    printf("Reading input file \"%s\"... ", par.trainFile);
    readCsv(patLen, lines, in, par.skip);
    printf("OK\n");
    fclose(in);
    printf("\n--------- CHECKING TRAINING PATTERNS VALIDITY --------\n\n");
    check_binary(lines, par.skip);
    printf("%lu patterns have been scanned\n", iList.Size(lines));
    printf("Patterns length is %lu\n", *patLen);
    emptyPats = check_pats_validity(lines);
    printf("Number of network patterns: %lu\n", iList.Size(lines));
    pats = iVector.Create(sizeof(Vector), iList.Size(lines));
    line_val_to_vec(pats, lines);
    trainClasses = iVector.Create(sizeof(void *), iList.Size(lines));
    line_class_to_vec(trainClasses, lines);
    printf("\n-------------------- ADDING NOISE -------------------\n\n");
    add_noise(pats, par.trainNoise);
    printf("\n------------------- TRAINING STAGE ------------------\n\n");
    network_train(bestClusts, &resFluc, par, pats);
    printf("\n-------------- WRITING TRAINING RESULTS -------------\n\n");
    write_train_results(clustsClasses, par, emptyPats, resFluc, pats,
                        vec_size(pats), *bestClusts, trainClasses);

    // free
    iVector.Finalize(pats);
    for(iLine = 0; iLine < iList.Size(lines); iLine++){
        iVector.Finalize((*(CSVLine *)(iList.GetElement(lines, iLine))).val);
    }
    iList.Finalize(lines);
    iVector.Finalize(trainClasses);
}

static void test_network(ulong trainPatLen, Vector *clusts,
                         Vector *clustsClasses, InParam par){
    Vector *pats;           // Vector of vectors
    Vector *testClasses;    // Vector of strings
    List *lines;            // List of CSVLine
    ulong emptyPats, patLen, iLine, iClass;
    FILE *in;
    Vector *testResClasses; // Vector of strings

    testResClasses = iVector.Create(sizeof(void *), 1);
    lines = iList.Create(sizeof(CSVLine));
    printf("\n------------- INTERNING TESTING PATTERNS ------------\n\n");
    openFile(&in, par.testFile, "r");
    printf("Reading input file \"%s\"... ", par.testFile);
    readCsv(&patLen, lines, in, par.skip);
    printf("OK\n");
    
    if(trainPatLen != patLen){
        fprintf(stderr, "\nERROR: training and testing sets do not contains "\
                        "patterns of same length. (training patterns are %lu "\
                        "bits long / testing patterns are %lu bits long)\n",
                        trainPatLen, patLen);
        exit(30);
    }
    
    fclose(in);
    printf("\n--------- CHECKING TESTING PATTERNS VALIDITY --------\n\n");
    check_binary(lines, par.skip);
    printf("%lu patterns have been scanned\n", iList.Size(lines));
    printf("Patterns length is %lu\n", patLen);
    emptyPats = check_pats_validity(lines);
    printf("Number of network patterns: %lu\n", iList.Size(lines));
    pats = iVector.Create(sizeof(Vector), iList.Size(lines));
    line_val_to_vec(pats, lines);
    testClasses = iVector.Create(sizeof(void *), iList.Size(lines));
    line_class_to_vec(testClasses, lines);
    printf("\n-------------------- ADDING NOISE -------------------\n\n");
    add_noise(pats, par.testNoise);
    printf("\n------------------- TESTING STAGE ------------------\n\n");
    network_test(&testResClasses, clusts, par, pats, clustsClasses, 
                 testClasses);
    printf("\n-------------- WRITING TESTING RESULTS -------------\n\n");
    write_test_results(par, emptyPats, pats, vec_size(pats), clusts,
                       testClasses, testResClasses);

    // free
    iVector.Finalize(testResClasses);
    for(iClass = 0; iClass < vec_size(testClasses); iClass++){
        free(vec_get_as_str(testClasses, iClass));
    }
    iVector.Finalize(testClasses);
    iVector.Finalize(pats);
    for(iLine = 0; iLine < iList.Size(lines); iLine++){
        iVector.Finalize((*(CSVLine *)(iList.GetElement(lines, iLine))).val);
    }
    iList.Finalize(lines);
}

int main(int argc, const char *argv[]){
    ulong iClust;
    InParam par;
    Vector *clusts = NULL;          // Vector of Clusters
    Vector *clustsClasses = NULL;   // Vector of strings
    ulong patLen;

    clustsClasses = iVector.Create(sizeof(void *), 1);
    set_network_values(&par, argc, argv);
    print_network_values(par);
    if(strcmp(par.trainFile, "") != 0){
        train_network(&patLen, &clusts, &clustsClasses, par);
    }
    if(strcmp(par.testFile, "") != 0){
        test_network(patLen, clusts, clustsClasses, par);
    }

    // free
    iVector.Finalize(clustsClasses);
    for(iClust = 0; iClust < vec_size(clusts); iClust++){
        iVector.Finalize((vec_get_as_clust(clusts, iClust)).prot);
        iVector.Finalize((vec_get_as_clust(clusts, iClust)).patSet);
    }
    iVector.Finalize(clusts);
    return EXIT_SUCCESS;
}
