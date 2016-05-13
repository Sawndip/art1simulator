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
 * @file io.c
 * @author Mathieu Fourcroy
 * @date June 2015
 * @version 0.0.1
 *
 * This file contains some function which are used to print informations on the
 * terminal or to write the results files.
 *
 * @todo Implement save and load for results files.
 *
 * PATTERNS TYPE
 * -------------
 * The patterns are effectivly interned in line_to_pat(). As it is for an ART1
 * network they are binary patterns. Thus, they are interned as vectors of 1 and
 * 0. The patterns are read in line_to_pat() and their "bits" are pushed in the
 * vectors in the form of integers codes but the vectors type is char to save
 * some memory. i.e.: the vectors contains char codes x0 and x1 only (not x30
 * and x31), that means that they need to be treated and printed as integers.
 *
 * INPUT FILES
 * -----------
 * The input file must be in csv format (commat separateed values). The name 
 * of the classes (i.e.: the attribute to guess) must be on the first column.
 * The files can contains any number of patterns of any length but only one 
 * pattern per line and every patterns MUST have the same length.
 * Comment are allowed on the files but they must begin by "#" and "#" must be
 * the first character of the line like:
 * @code
 *  # This is a comment and next are the patterns
 *  poisonous,0,1,0,0,0,0,1,0,0,1,1,...
 * @endocde
 * Empty lines are allowed, they will be ignored by the parser.
 */

/*=====| INCLUDES |===========================================================*/
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <math.h>
#include "io.h"
#include "dbg.h"

/*=====| FUNCTIONS |==========================================================*/
/** Fill the parameters structure.
*
* @warning The order of the arguments if fix!
*
* @todo Replace this function with a true cmdl parser (setop) and remove the
*   "trainart" script.
*
* @param[out] par   The structure whoch contains the arguments's values.
* @param[in]  argc  Number of arguments in argv.
* @param[in]  argv  The arguments.
*/
void set_network_values(InParam *par, int argc, const char *argv[]){
    if((argc < 11)){
        fprintf(stderr, "Invalid number of parameters!\nExiting...\n");
        exit(2);
    }
    strcpy(par->trainFile, argv[1]);
    strcpy(par->testFile, argv[2]);
    strcpy(par->prefix, argv[3]);
    par->skip = atoi(argv[4]);
    par->trainNoise = atol(argv[5]);
    par->testNoise = atol(argv[6]);
    par->beta = atoi(argv[7]);
    par->vigilance = atof(argv[8]);
    par->minFluc = atof(argv[9]);
    par->maxPasses = atol(argv[10]);
}

/** Print the network parameters.
 *
 * @param[in] par   The structure holding the network parameters
 * @param[in] trainFile Path of the input file.
 * @param[in] oFile Path of the output file.
 */
void print_network_values(InParam par){
    puts("   _   ___ _____ _   ___ ___ __  __ _   _ _      _ _____ ___  ___ "\
         "\n  /_\\ | _ \\_   _/ | / __|_ _|  \\/  | | | | |    /_\\_   _/ _ "\
         "\\| _ \\\n / _ \\|   / | | | | \\__ \\| || |\\/| | |_| | |__ / _ "\
         "\\| || (_) |   /\n/_/ \\_\\_|_\\ |_| |_| |___/___|_|  "\
         "|_|\\___/|____/_/ \\_\\_| \\___/|_|_\\\n");
    printf("-------------------------------------------------------\n\n");
    puts("Runing ART1 algorithm using following parameters:");
    printf("\tTraining file: %s\n", par.trainFile);
    printf("\tTesting file: %s\n", par.trainFile);
    if(par.skip){
        printf("\tSkipping first attribute of every patterns\n");
    }
    printf("\tOutput file prefix: %s\n", par.prefix);
    printf("\tFluctuation percentage: %g%%\n", par.minFluc);
    printf("\tMax number of iterations: %lu\n", par.maxPasses);
    printf("\tBeta parameter: %.0f\n", par.beta);
    printf("\tVigilance parmeter: %g\n", par.vigilance);
}

/** Open a file.
 *
 * @param[out] file The opened file.
 * @param[in]  name Path of the file to open.
 * @param[in]  mode The opening mode ("r", "w", ...).
 *
 * @return A boolean which indicate waither the file has successfully been
 *  opened.
 */
int openFile(FILE **file, const char *name, char const *mode){
    //~ printf("Opening file \"%s\"... ", name);
	*file = fopen(name, mode);
	if(*file == 0){
	   fprintf(stderr, "\nFAIL\nERROR: Can't open file \"%s\"\nExiting...\n", 
               name);
	   exit(10);
	}
    //~ printf("OK\n");
    return *file == NULL;
}

/** Count the number of columns of a csv string.
 *
 * This function don't care about commas and effectively count the number of
 * columns (i.e.: attributes) in a csv string.
 *
 * @param[in] line A csv string.
 *
 * @return The number of columns in the csv string.
 */
static ulong nb_cols(char *line){
    ulong c = 0;
    ulong count = 0;

    while(line[c] != '\0'){
        if(line[c] == ','){
            count++;
        }
        c++;
    }
    return count;
}

/** Helper function printing a vector of characters as integers.
 *
 * @param[in] vec The vector to print.
 */
void vec_print_char_as_int(Vector *vec){
    ulong i;

    size_t size = vec_size(vec);
    if(size < 1){
        printf("empty\n");
        return;
    }
    for(i = 0; i < size - 1; i++){
        printf("%i,", vec_get_as_char(vec, i));
    }
    printf("%i\n", vec_get_as_char(vec, i));
}

/** Helper function printing a vector of characters.
 *
 * @param[in] vec The vector to print.
 */
void vec_print_as_char(Vector *vec){
    ulong i;

    size_t size = vec_size(vec);
    if(size < 1){
        printf("empty\n");
        return;
    }
    for(i = 0; i < size - 1; i++){
        printf("%c,", vec_get_as_char(vec, i));
    }
    printf("%c\n", vec_get_as_char(vec, i));
}

/** Helper function printing a vector of long unsigned numbers.
 *
 * @param[in] vec The vector to print.
 */
void vector_print_ulong(Vector *vec){
    ulong i;

    size_t size = vec_size(vec);
    if(size < 1){
        printf("empty\n");
        return;
    }
    for(i = 0; i < size - 1; i++){
        printf("%lu,", vec_get_as_ulong(vec, i));
    }
    printf("%lu\n", vec_get_as_ulong(vec, i));
}

/** Transform a csv string into a pattern (CSVLine strcture).
 *
 * @note If the first attribute is skipped (see parameter skip) then it is added
 *  to the "class" member of the returned structure instead of being added to
 * its "val" member.
 *
 * @param[in] line The csv string to transform.
 * @param[in] skip Indicate weither the first attribute of the csv string must
 *  be skipped or not.
 *
 * @return The pattern corresponding to the input csv string.
 */
static CSVLine line_to_pat(char *line, const bool skip){
    CSVLine res;
    ulong i = 0;
    char snumber;
    char *buffer;

    res.val = iVector.Create(sizeof(char), 1);
    buffer = strtok(line, ",");
    while(buffer && *buffer != '\0' && *buffer != '\n'){
        snumber = *buffer - '0';    // '0' == 0x30 == 48 == 060
        if(i < skip){
            res.class = malloc(sizeof(char) * strlen(buffer) + 1);
            strcpy(res.class, buffer);
        }
        else{
            vec_pushback(res.val, &snumber);
        }
        buffer = strtok(NULL, ",");
        while(buffer && *buffer == ' '){
            buffer++;
        }
        i++;
    }
    return res;
}

/** Read a csv file into a list.
 *
 * Transform each line of the file into a pattern and insert it in the patterns
 * list.
 *
 * @note Every lines of the csv file must contains the same number of columns.
 *  The file can contains basic comments: each comment lines MUST start with a
 *  "#". Moreover a line can be empty, it will be ignored.
 *
 * @param[out] plen  The length of the patterns (their number of attributes), 
 *  computed bi this function
 * @param[out] lines The list which will be filled with the patterns.
 * @param[in]  file  The csv data file containing the string representing the 
 *  future patterns.
 * @param[in]  skip  Indicate weither the first attribute of the csv string must
 *  be skipped or not.
 */
void readCsv(ulong *plen, List *lines, FILE *file, bool skip){
    char *readline = NULL;
    CSVLine pat;
    ulong patLen;
    ulong lineLen = 0;
    ulong nlines = 1;
    ulong columns = 0;
    bool firstLine = false;
    bool firstPat = true;
    int read;
    size_t len;

    while((read = getline(&readline, &len, file)) != -1){
        if((strlen(readline) < 1) || strcmp(readline, "\n") == 0 ||
           (readline[0] == '#')){
            nlines++;
            continue;
        }
        if(firstLine == false){
            columns = nb_cols(readline);
            firstLine = true;
        }
        pat = line_to_pat(readline, skip);
        patLen = skip + vec_size(pat.val);

        if(!firstPat){
            if(lineLen == patLen){
                iList.Add(lines, &pat);
            }
            else if(patLen > 0){
                fprintf(stderr, "\nERROR: line %lu has %lu columns. Previous "\
                        "lines had %lu columns ...\n\nExiting...\n\n",
                        nlines, patLen, lineLen);
                exit(11);
            }
            else{
                printf("\nline %lu is empty - skipping\n", nlines);
            }
        }
        else{
            firstPat = false;
            lineLen = patLen;
            iList.Add(lines, &pat);
        }
        nlines++;
        if(nlines == ULONG_MAX){
            fprintf(stderr, "ERROR: The input file contains too much lines");
            exit(5);
        }
    }
    *plen = columns - skip;
    free(readline);
}

/** Write a pattern (a vector of 0 and 1) on the given file.
 *
 * @param[in] out The output file where the pattern will be written.
 * @param[in] pat The pattern to write.
 */
static void write_vector(FILE *out, Vector *pat){
    ulong i;

    if(vec_size(pat) < 1){
        printf("WARNING: pattern is empty: nothing to print\n");
        return;
    }
    for(i = 0; i < vec_size(pat) - 1; i++){
        fprintf(out, "%i,", vec_get_as_char(pat, i));
    }
    fprintf(out, "%i", vec_get_as_char(pat, i));
    fprintf(out, "\n");
}

/** Write the ID of every patterns, in each cluster.
 *
 * It should look like:
 * @code
 *  Cluster    0: 27, 89, 118, 349, 356, 688, ...
 *  [...]
 *  Cluster  104: 8, 11, 28, 29, 44, 47, 48, ...
 * @endcode
 * @param[in] out    The output file where the clusters's patterns sets will be
 *  wwritten.
 * @param[in] clusts The network clusters.
 */
static void write_clusts_pat_sets(FILE *out, Vector *clusts){
    Vector *patSet;
    ulong iClust, iPatSet;
    
    fprintf(out, "---------------------------------------\n");
    fprintf(out, "------- CLUSTERS'S PATTERNS SETS ------\n");
    fprintf(out, "---------------------------------------\n");
    for(iClust = 0; iClust < vec_size(clusts); iClust++){
        fprintf(out, "Cluster %4lu: ", iClust);
        patSet = get_pat_set(vec_get_as_clust(clusts, iClust));
        for(iPatSet = 0; iPatSet < vec_size(patSet) - 1; iPatSet++){
            fprintf(out, "%lu, ", vec_get_as_ulong(patSet, iPatSet)); 
        }
        fprintf(out, "%lu\n\n", vec_get_as_ulong(patSet, iPatSet));  
    }
}

/** Write the number of patterns in each class, in each cluster.
 *
 * For instance, in the mushrooms database there's two classes:
 * p (poisonous) and e (edible). Then the repartition looks like:
 * @code
 *  No. cluster |    p    |    e    | Class
 *  ------------+---------+---------+------
 *            0 |     274 |     320 | e
 *            1 |       0 |     768 | e
 *            2 |      32 |     108 | e
 *            3 |      72 |     443 | e
 *                 [...]
 *           15 |       8 |       0 | p
 *           16 |       0 |      16 | e
 * @endcode
 * The class column indicate the clusters's classes: the prominent class of the
 * cluster.
 *
 * This function would deserved a little clean up. It is a long function for a
 * small job but it actually generate ascii tables for any number of classes of
 * any lengths.
 *
 * @param[out] clustsClass The array that will be filled with the clusters's 
 *  classes.
 * @param[in] out          The output file where the classes repartition will 
 *  be written.
 * @param[in] clusts       The network clusters.
 * @param[in] patsClass    The network training patterns classes.
 */
static void write_clusts_classes(Vector **clustsClass, FILE *out,
                                 Vector *clusts, Vector *patsClass){
    Vector *patSet;         // Vector of ulong
    ulong iClust, iPatSet;
    char *c;
    int noUniqueClasses;
    ulong i, j;
    char *class;
    char *majorClass;
    ulong majorClassScore;
    int extraMargin;
    Vector *uniqueClasses;  // Vector of strings
    char *dummy = "";

    uniqueClasses = iVector.Create(sizeof(void *), 1);
	for(i = 0; i < vec_size(patsClass); i++){
        class = vec_get_as_str(patsClass, i);
        for(j = 0; j < vec_size(uniqueClasses); j++){
            if(strcmp(class, vec_get_as_str(uniqueClasses, j)) == 0){
                break;
            }
        }
        if(j == vec_size(uniqueClasses)){
            vec_pushback(uniqueClasses, &class);
        }
	}
    noUniqueClasses = vec_size(uniqueClasses);
    ulong classesRep[noUniqueClasses];

    fprintf(out, "\n---------------------------------------\n");
    fprintf(out, "---- CLUSTERS'S CLASSES REPARTITION ---\n");
    fprintf(out, "---------------------------------------\n\n");
    fprintf(out, "No. cluster | ");

    // print the header
    for(i = 0; i < noUniqueClasses; i++){
        class = vec_get_as_str(uniqueClasses, i);
        if(strlen(class) > 7){
            extraMargin = 0;
        }
        else{
            extraMargin =  (7 - strlen(class)) / 2;
        }
        for(j = 0; j < extraMargin; j++){
            fprintf(out, " ");
        }
        fprintf(out, "%s", class);
        for(j = 0; j < extraMargin; j++){
            fprintf(out, " ");
        }
        fprintf(out, " | ");
    }
    fprintf(out, "Class\n");

    // print a line under the header
    fprintf(out, "------------+");
    for(i = 0; i < noUniqueClasses; i++){
        class = vec_get_as_str(uniqueClasses, i);
        extraMargin = strlen(class) > 7 ? strlen(class) : 7;
        for(j = 0; j < extraMargin; j++){
            fprintf(out, "-");
        }
        fprintf(out, "--+");
    }
    fprintf(out, "------\n");

    // Fill the clustsClass vector with anything so you can replace the elements
    for(iClust = 0; iClust < vec_size(clusts); iClust++){
        vec_pushback(*clustsClass, &dummy);
    }

    // Determine the clusters's classes
    for(iClust = 0; iClust < vec_size(clusts); iClust++){
        patSet = get_pat_set(vec_get_as_clust(clusts, iClust));
        // Set every index of the classesRep (1 index = 1 unique class) to 0
        for(i = 0; i < noUniqueClasses; i++){
            classesRep[i] = 0;
        }
        // Increment each index when a pat of the clust has corresponding class
        for(iPatSet = 0; iPatSet < vec_size(patSet); iPatSet++){
            c = vec_get_as_str(patsClass, vec_get_as_ulong(patSet, iPatSet));
            for(i = 0; i < noUniqueClasses; i++){
                if(strcmp(c, vec_get_as_str(uniqueClasses, i)) == 0){
                    classesRep[i] = classesRep[i] + 1;
                }
            }
        }
        // Determine the class of the biggest part of the patterns of the clust
        majorClass = NULL;
        majorClassScore = 0;
        for(i = 0; i < noUniqueClasses; i++){
            if(classesRep[i] > majorClassScore){
                majorClassScore = classesRep[i];
                majorClass = vec_get_as_str(uniqueClasses, i);
            }
        }
        // clustsClass[current_clust] is set to the major class of its patterns
        vec_replace_at(*clustsClass, iClust, &majorClass);
        // print the classes repartition and the cluster's class
        fprintf(out, "%11lu | ", iClust);
        for(i = 0; i < noUniqueClasses; i++){
            class = vec_get_as_str(uniqueClasses, i);
            if(strlen(class) > 7){
                extraMargin = (strlen(class) - 7);
                for(j = 0; j < extraMargin; j++){
                    fprintf(out, " ");
                }
                fprintf(out, "%7lu", classesRep[i]);
            }
            else{
                fprintf(out, "%7lu", classesRep[i]);
            }
            fprintf(out, " | ");
            
        }
        fprintf(out, "%s\n", vec_get_as_str(*clustsClass, iClust));
    }
    iVector.Finalize(uniqueClasses);
}

/** Write the success / fail ratio of the training stage in the given file.
 *
 * @param[in] out         The output file where the prototypes will be written.
 * @param[in] pats        The network training patterns set.
 * @param[in] clusts      The network clusters.
 * @param[in] patsClass   The network training patterns classes.
 * @param[in] clustsClass The network clusters classes.
 */
static void write_ratio(FILE *out, Vector *pats,  Vector *clusts,
                        Vector *patsClass, Vector *clustsClass){
    ulong success = 0;
    ulong fail = 0;
    ulong iPat;
    char *class;

    fprintf(out, "\n---------------------------------------\n");
    fprintf(out, "--------- SUCCESS / FAIL RATIO --------\n");
    fprintf(out, "---------------------------------------\n");
    for(iPat = 0; iPat < vec_size(pats); iPat++){
        class = vec_get_as_str(patsClass, iPat);
        if(strcmp(vec_get_as_str(clustsClass, pat_in_clust_set(clusts, iPat)), class) == 0){
            success++;
        }
        else{
            fail++;
        }
    }
    fprintf(out, "SUCCESS: %lu (%g%%)\n",
           success, success * 100 / (float)(success + fail));
    fprintf(out, "FAIL: %lu (%g%%)\n",
            fail, fail * 100 / (float)(success + fail));
    fprintf(out, "RATIO: %g\n\n", success / (float)fail);
}

/** Write the prototype of every clusters in the given file.
 *
 * @param[in] out     The output file where the prototypes will be written.
 * @param[in] par     The network parameters.
 * @param[in] clusts  The network clusters.
 * @param[in] nClusts Number of network clusters.
 */
static void write_clusts_prototypes(FILE *out, InParam par, Vector *clusts, 
                                    ulong nClusts){
    ulong i;

    fprintf(out, "\n---------------------------------\n");
	fprintf(out, "----- CLUSTERS'S PROTOTYPES -----\n");
    fprintf(out, "---------------------------------\n");
    if(nClusts > 1){
        fprintf(out, "[Clusters's patterns members have been saved in "\
                "%s%s%s_clust[0-%lu].csv]\n\n", RES_FOLDER, CLUST_FOLDER,
                par.prefix, nClusts - 1);
    }
    else{
        fprintf(out, "[Clusters's patterns members have been saved in "\
                "%s%s%s_clust0.csv]\n\n", RES_FOLDER, CLUST_FOLDER, par.prefix);
    }
	for(i = 0; i < nClusts; i++){
        fprintf(out, "Clust %lu prototype:\n", i);
        write_vector(out, get_prot(vec_get_as_clust(clusts, i)));
        fprintf(out, "\n");
	}
}

/** Create a file where a cluster's prototype and its patterns are written.
 *
 * @param[in] par     The network parameters.
 * @param[in] pats    The network training patterns set.
 * @param[in] clusts  The network clusters.
 * @param[in] nClusts Number of network clusters.
 */
static void write_cluster_files(InParam par, Vector *pats, Vector *clusts,
                                ulong nClusts){
    ulong i, j;

	for(i = 0; i < nClusts; i++){
		FILE *of;
		char name[64];
        sprintf(name, "%s%s%s%s_%lu.csv", RES_FOLDER, CLUST_FOLDER,
                par.prefix, CLUST_SUFFIX, i);
		if(openFile(&of, name, "w") == true){
            break;
        }
        fprintf(of, "# prototype:\n");
        write_vector(of, get_prot(vec_get_as_clust(clusts, i)));
        fprintf(of, "\n# patterns:\n");
		for(j = 0; j < vec_size(get_pat_set(vec_get_as_clust(clusts, i))); j++){
            write_vector(of, vec_get_as_vec(pats, vec_get_as_ulong(get_pat_set(
                vec_get_as_clust(clusts, i)), j)));
		}
		fclose(of);
	}
}

/** Write the result datas on the output files.
 *
 * There's too much datas to write them on the terminal so they are written on
 * output files. The main output file is "art_results" (by default) and it
 * contains every information used to build and train the network. Other output
 * files are named "art_clust_xxx.csv" and they contains the cluster #xxx 
 * prototype followed by every patterns which belongs to this cluster.
 *
 * @todo It would be a good idea to use the output file in order to load a
 *  network.
 *
 * @param[out] clustsClass The array that will be filled with the clusters's 
 *  classes.
 * @param[in] par       The network parameters.
 * @param[in] emptyPats Number of empty patterns removed.
 * @param[in] fluc      The best recorded fluctuation.
 * @param[in] pats      The network training patterns set.
 * @param[in] nPats     Number of training patterns of the network.
 * @param[in] clusts    The network clusters.
 * @param[in] patsClass The network training patterns classes.
 */
void write_train_results(Vector **clustsClasses, InParam par, ulong emptyPats,
                         float fluc, Vector *pats, ulong nPats, Vector *clusts, 
                         Vector *patsClass){
    ulong nClusts = vec_size(clusts);
    char path[PATH_MAX];
    FILE *out;

    printf("Writing results...\n");
    sprintf(path, "%s%s%s%s", RES_FOLDER, TRAIN_FOLDER, par.prefix, RES_SUFFIX);
    openFile(&out, path, "w");
    fprintf(out, "---------------------------------------\n");
    fprintf(out, "----- ART1 NETWORK SPECIFICATIONS -----\n");
    fprintf(out, "---------------------------------------\n");
    fprintf(out, "Generated from:\n");
    fprintf(out, "\tfile: \"%s\"\n", par.trainFile);
    fprintf(out, "\tnumber of trained patterns: %lu\n", nPats);
    fprintf(out, "\tnumber of empty patterns (ignored): %lu\n", emptyPats);
    fprintf(out, "\nWith parameters:\n");
    fprintf(out, "\tvigilance: %g\n", par.vigilance);
    fprintf(out, "\tbeta: %g\n", par.beta);
    fprintf(out, "\tnoise: %i\n", par.trainNoise);
    fprintf(out, "\tmaximum fluctuation: %g%%\n", par.minFluc);
    fprintf(out, "\tmaximum number of passes: %lu\n", par.maxPasses);
    fprintf(out, "\n---------------------------------\n");
    fprintf(out, "----- ART1 NETWORK RESULTS ------\n");
    fprintf(out, "---------------------------------\n");
	fprintf(out, "Number of resulting clusters: %lu\n", nClusts);
	fprintf(out, "Minimum fluctuation: %g%%\n", fluc);
    write_clusts_prototypes(out, par, clusts, nClusts);
    write_clusts_pat_sets(out, clusts);
    write_clusts_classes(clustsClasses, out, clusts, patsClass);
    write_ratio(out, pats, clusts, patsClass, *clustsClasses);
    write_cluster_files(par, pats, clusts, nClusts);
    printf("OK\n");
    fclose(out);
}

void write_test_results(InParam par, ulong emptyPats, Vector *pats,
                        ulong nPats, Vector *clusts, Vector *testClasses,
                        Vector *testResClasses){
    char path[PATH_MAX];
    FILE *out;
    ulong success = 0;
    ulong fail = 0;
    ulong iPat;

    sprintf(path, "%s%s%s%s", RES_FOLDER, TEST_FOLDER, par.prefix, RES_SUFFIX);
    openFile(&out, path, "w");
    printf("Writing results... ");
    fprintf(out, "---------------------------------------\n");
    fprintf(out, "----- ART1 NETWORK SPECIFICATIONS -----\n");
    fprintf(out, "---------------------------------------\n");
    fprintf(out, "Generated from:\n");
    fprintf(out, "\ttesting file: \"%s\"\n", par.testFile);
    fprintf(out, "\tnumber of tested patterns: %lu\n", nPats);
    fprintf(out, "\tnumber of empty patterns (ignored): %lu\n", emptyPats);
    fprintf(out, "\nWith parameters:\n");
    fprintf(out, "\tnoise: %i\n", par.testNoise);
    fprintf(out, "\n---------------------------------\n");
    fprintf(out, "------- ART1 TEST RESULTS -------\n");
    fprintf(out, "---------------------------------\n\n");

    fprintf(out, "Pattern | Target | Result | Status\n");
    fprintf(out, "--------+--------+--------+--------\n");
    for(iPat = 0; iPat < nPats; iPat++){
        if(strcmp(vec_get_as_str(testClasses, iPat), vec_get_as_str(testResClasses, iPat)) == 0){
            success++;
            fprintf(out, "%7lu | %6s | %6s | SUCCESS\n", iPat, vec_get_as_str(testClasses, iPat), vec_get_as_str(testResClasses, iPat));
        }
        else{
            fail++;
            fprintf(out, "%7lu | %6s | %6s | FAIL\n",  iPat, vec_get_as_str(testClasses, iPat), vec_get_as_str(testResClasses, iPat));
        }
    }
    fprintf(out, "---------------------------------------\n");
    fprintf(out, "----------- ART1 TEST RATIO -----------\n");
    fprintf(out, "---------------------------------------\n");
    fprintf(out, "SUCCESS: %lu (%g%%)\n",
           success, success * 100 / (float)(success + fail));
    fprintf(out, "FAIL: %lu (%g%%)\n",
            fail, fail * 100 / (float)(success + fail));
    fprintf(out, "RATIO: %g\n\n", success / (float)fail);

    printf("OK\n\n");
    fclose(out);
}
