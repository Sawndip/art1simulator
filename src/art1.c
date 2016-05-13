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
 * @file art1.c
 * @author Mathieu Fourcroy
 * @date June 2015
 * @version 0.0.1
 *
 * This file contains the functions about training and testing the network.
 *
 * CLUSTERS
 * --------
 * A cluster include two things:
 *  - A prototype: it's the centroid of the cluster, it's basically a pattern
 *    wich contains 1 only where every patterns of the cluster have a 1.
 *  - A set of patterns: it's the cluster members, they shape the cluster
 *    prototype.
 * 
 * In this programm a cluster is represented by the Cluster structure (yes, 
 * really!) 
 *
 * FLUCTUATION
 * -----------
 * The param.minFluc parameter is used to indicate the maximum fluctuation
 * percentage. The fluctuation is a function of the number of patterns which
 * were re-assigned to a different cluster. The higher the number of re-assigned
 * patterns, the higher the fluctuation rate.
 * This parameter is used in order to avoid (nearly) infinite loops when
 * training patterns.
 *
 * @todo The fluctuation rate must be increased after each pass.
 */

/*=====| INCLUDES |===========================================================*/
#include <limits.h>
#include <time.h>
#include "art1.h"
#include "utls.h"
#include "io.h"
#include "ccl_internal.h"

/*=====| FUNCTIONS |==========================================================*/
/** Add a given pattern to a cluster prototype.
 *
 * Add pattern 'pat' to a cluster prototype 'prot'. The prototype 'prot' will be
 * more similar to this pattern because if it has a 1 where pattern have a 0
 * then the 1 become a 0.
 *
 * @note A prototype have a 1 only where every member patterns have a 1.
 *
 * @param[in, out]  prot The prototype that will be modified to ressembles the
 *  pattern.
 * @param[in]       pat  The pattern to add to the prototype.
 */
static void prot_add_pat(Vector *prot, Vector *pat){
    ulong i;
    char c = 0;

	for(i = 0; i < vec_size(pat); i++){
		if(vec_get_as_char(pat, i) == 0 && vec_get_as_char(prot, i) == 1){
            vec_replace_at(prot, i, &c);
        }
	}
}

/** Remove the given pattern from the given cluster.
 *
 * Remove the pattern at index 'iPat' of 'pats' from cluster prototype at 
 * index 'iClust' of 'clusts'. Also remove the pattern ID from the prototype
 * patterns set.
 *
 * @param[in]     pats   The network training patterns set.
 * @param[in]     iPat   The index of the pattern to remove.
 * @param[in,out] clusts The network clusters.
 * @param[in]     iClust The index of the cluster from which the pattern will be
 *  removed.
 */
static void clust_rm_pat(Vector *clusts, ulong iClust, Vector *pats,
                         ulong iPat){
    ulong i;
    Cluster *clust = &vec_get_as_clust(clusts, iClust);
    Vector *patSet = clust->patSet; // Vector of ulongs
    Vector *prot = clust->prot;     // Vector of chars

	for(i = 0; i < vec_size(patSet); i++){
        if(vec_get_as_ulong(patSet, i) == iPat){
            vec_erase(patSet, vec_get(patSet, i));
            break;
        }
    }
	if(vec_size(patSet) == 0){
        iVector.EraseAt(clusts, iClust); 
	}
	else{
        prot = vec_get_as_vec(pats, vec_get_as_size_t(patSet, 0));
		for(i = 1; i < vec_size(patSet); i++){
            prot_add_pat(prot, vec_get(pats, vec_get_as_size_t(patSet, i)));
        }
	}
}

/** Returns the number of 1 that the two given patterns have in common.
 *
 * Count and return the number of 1 which are present in 'pat1' and 'pat2', at
 * same indexes.
 *
 * @note 'pat1' and 'pat2' must have the same length. It is admit and it's
 *  obvious because the program check it at the begining so no test are carried
 *  out here in order to save time.
 *
 * @param[in] pat1 The first pattern, will be compared to 'pat2'.
 * @param[in] pat2 The second pattern, will be compared to 'pat1'.
 *
 * @return The number of 1 that the two given patterns have in common.
 */
static long comOnes(Vector *pat1, Vector *pat2){
    unsigned int i;
    ulong count = 0;

	for(i = 0; i < vec_size(pat1); i++){
        if(vec_get_as_char(pat1, i) == 1 && vec_get_as_char(pat2, i) == 1){
            count++;
        }
    }
	return count;
}

/** Returns number of 1 in a given pattern (or a prototype).
 *
 * @param[in] pat The pattern.
 *
 * @return The number of 1 in the given pattern.
 */
static long ones(Vector *pat){
    unsigned int i;
    ulong count = 0;

    for(i = 0; i < vec_size(pat); i++){
        if(vec_get_as_char(pat, i) == 1){
            count++;
        }
    }
	return count;
}

/** Add a new cluster to the network clusters vector.
 *
 * Ceate a new cluster and initialize it by setting pattern at index 'iPat' in
 * 'pats' as its prototype and adding pattern ID 'iPat' in its patterns set.
 * Finaly, add the new cluster to 'clusts'.
 *
 * @param[in, out] clusts The network clusters vector where the new cluster will
 *  be added.
 * @param[in]      pats   The network training patterns.
 * @param[in]      iPat   The index of the current training pattern.
 */
static void clust_add_new(Vector *clusts, Vector *pats, ulong iPat){
    ulong iClust = pat_in_clust_set(clusts, iPat);
    Cluster newClust;
    Vector *newProtSet;    // Vector of ulongs

	if(iClust != NOT_FOUND){
        clust_rm_pat(clusts, iClust, pats, iPat);
    }
    newProtSet = iVector.Create(sizeof(ulong), 1);
    vec_pushback(newProtSet, &iPat);
    newClust.prot = vec_copy(vec_get_as_vec(pats, iPat));
    newClust.patSet = newProtSet;
    newClust.inhib = false;
    vec_pushback(clusts, &newClust);
}

/** Add a pattern to a cluster.
 *
 * To add a pattern to a candidate cluster we must:
 *  - Add the pattern ID to the cluster patterns set
 *  - Reshape the cluster prototype.
 *
 * If the pattern already belongs to another cluster then it is removed from
 * this cluster and added to the candidate cluster (reassignement) and true is 
 * returned. If the pattern already belongs to the candidate cluster then false
 * is returned and the algorithm should go with the next pattern to train. 
 * Else, if the pattern dosen't already belongs to any cluster then it is simply
 * added to the candidate cluster and true is returned.
 *
 * @param[in] pat        The pattern to add to the cluster.
 * @param[in] pats       The network training patterns set.
 * @param[in,out] clusts The network clusters.
 * @param[in] iPat       The index of 'pat' in 'pats'.
 * @param[in] iCandidate The index of the cluter in which the pattern will be 
 *  added.
 *
 * @return true if the pattern has been added to the cluster ; false if the 
 *  pattern already belongs to the cluster. 
 */
static bool clust_add_pat(Vector *pat, Vector *pats, Vector *clusts, ulong iPat,
                          ulong iCandidat){
    ulong iClust = pat_in_clust_set(clusts, iPat);

    if(iClust != NOT_FOUND){
        if(iClust == iCandidat){
            return false;
        }
        else{
            clust_rm_pat(clusts, iClust, pats, iPat);
        }
    }
    prot_add_pat(get_prot(vec_get_as_clust(clusts, iCandidat)), pat);
    vec_pushback(get_pat_set(vec_get_as_clust(clusts, iCandidat)), &iPat);
    return true;
}

/** Fill the score array with the computed scores of the network prototypes.
 *
 * Fill the 'score' array with a score value for each cluster prototype in
 * 'clusts'. The score value represents the similarity level between the actual
 * pattern and a cluster prototype. If a prototype is inhibited then its score
 * is NONE.
 *
 * @note The calculation is based on the 'beta' parameter, the pattern 'pat'
 *  and the cluster prototypes.
 *
 * @note The 'clusts' vector size, is passed as parameter 'psize' in 
 * order to avoid a second vec_size() call.
 *
 * @param [out] scores The scores array to fill.
 * @param [in]  psize  Number of clusters of the network.
 * @param [in]  pat    The current training pattern.
 * @param [in]  clusts The network cllusters.
 * @param [in]  beta   The network beta parameter.
 */
static void fill_scores(double *scores, const ulong psize, Vector *pat,
                        Vector *clusts, float beta){
    ulong iClust;
    Vector *prot;   // Vector of chars

    for(iClust = 0; iClust < psize; iClust++){
	    if(get_inhib(vec_get_as_clust(clusts, iClust))){
            scores[iClust] = NONE;
        }
	    else{
            prot = get_prot(vec_get_as_clust(clusts, iClust));
            scores[iClust] = (double)(comOnes(prot, pat)) / (beta + ones(prot));
        }
    }
}

/** Compute the highest score in the 'scores' array.
 *
 * The 'eqScores' array is modified to contain the highest score prototype(s)
 * ID(s) (there can be multiple prototypes with the same highest score that's
 * why 'eqScores' is a vector).
 *
 * @param [out] eqScores The scores array to fill with every highest scores.
 * @param [in]  scores   The array containing the prototypes's scores.
 * @param [in]  psize    Number of clusters of the network.
 * @param [in]  clusts   The network cllusters.
 */
static void highest_score(Vector **eqScores, double *scores, ulong psize,
                          Vector *clusts){
    ulong i;
    double highest = NONE;

    for(i = 0; i < psize; i++){
        if(scores[i] == highest){
            vec_pushback(*eqScores, &i);
        }
        else if(scores[i] > highest){
            vec_clear(*eqScores);
            vec_pushback(*eqScores, &i);
            highest = scores[i];
        }
    }
}

/** Returns the number of inhibited clusters.
 *
 * @param[in] clusts The network clusters.
 *
 * @return The number of inhibited clusters.
 */
static ulong count_inhib_clusts(Vector *clusts){
    ulong i;
    ulong count = 0;

    for(i = 0; i < vec_size(clusts); i++){
        if(get_inhib(vec_get_as_clust(clusts, i))){
            count++;
        }
    }
    return count;
}

/**Compute and returns the uninhibited prototype closest to a given pattern.
 *
 * To achieve this, the function first compute the "similarity score" of every
 * uninhibited prototypes (inhibited prototypes have a score of -1) then it
 * computes the prototype with the highest score (if more than one prototype
 * with equal highest score then the returned one is randomly chosen).
 * If all clusters are inhibited then the function return false.
 *
 * @note It would be more C-style to return a bool indicating if a candidate
 *  has been found or not and set it via a passed argument. But this way it is
 *  easier for freeing.
 *
 * @param[out] candidat  The uninhibited prototype closest to a given pattern 
 *  (the candidate).
 * @param[out] iCandidat The index of the computed candidate.
 * @param[in]  pat       The reference pattern.
 * @param[in]  clusts    The network clusters.
 * @param[in]  beta      The network beta parameter.
 *
 * @return true or false weither a candidate has been found.
 */
static Cluster *nearest_prot(ulong *iCandidat, Vector *pat, Vector *clusts,
                             float beta){
	Vector *eqScores;  // Vector of ulongs
    Cluster *empty = malloc(sizeof(*empty));
	const ulong psize = vec_size(clusts);
    double scores[psize];

    eqScores = iVector.Create(sizeof(ulong), 1);
    empty->prot = iVector.Create(sizeof(Vector), 0);
	if(count_inhib_clusts(clusts) == vec_size(clusts)){
        iVector.Finalize(eqScores);
        return empty;
    }
    fill_scores((double *)scores, psize, pat, clusts, beta);
    highest_score(&eqScores, (double *)scores, psize, clusts);
    if(vec_size(eqScores) == 0){    // Should never happen
        iVector.Finalize(eqScores);
        return empty;
    }
    iVector.Finalize(empty->prot);
    free(empty);
    if(vec_size(eqScores) == 1){    // Don't have to chose
        *iCandidat = vec_get_as_ulong(eqScores, 0);
    }
    else{                           // Randomly chose.
        *iCandidat = vec_get_as_ulong(eqScores, (rand() % vec_size(eqScores)));
    }
    iVector.Finalize(eqScores);
    return &vec_get_as_clust(clusts, *iCandidat);
}

/** Compute the statistics of the last training pass.
 *
 * Sets the 'noReassigned' parameter (number of patterns reassigned to another
 * cluster during the pass) and the 'fluc' parameter (fluctuation percentage for
 * the pass).
 *
 * @param[out] noReassigned Number of patterns which have been assigned to 
 *  another cluster during the pass.
 * @param[out] fluc         Fluctuation percentage of the pass.
 * @param[in] reassigned    Array of values indicating the ID of the patterns
 *  which have been reassigned to another cluster (ID is the index in the 
 *  array).
 * @param[in] pats          The network training patterns set.
 */
static void compute_pass_stats(ulong *noReassigned, float *fluc,
                               Vector *reassigned, Vector *pats){
    ulong c;

    *noReassigned = 0;
    for(c = 0; c < vec_size(reassigned); c++){
        if(vec_get_as_char(reassigned, c) != false){
            *noReassigned = *noReassigned + 1;
        }
    }
    *fluc = ((float)*noReassigned / vec_size(pats)) * 100;
}

/** Select a cluster candidate and try to add the pattern to this candidate.
 *
 * Select the nearest prototype of the pattern at index 'iPat' in 'pats' (the 
 * candidate) and check if the pattern can fits in the candidate's cluster 
 * depending on beta and vigilance parameters. If the pattern fits in the 
 * cluster, then it is added to it, else, a new cluster is  create and the 
 * pattern became its prototype.
 *
 * @param[in]     param      The network parameters. 
 * @param[in]     pats       The training patterns set.
 * @param[in,out] clusts     The network clusters.
 * @param[in,out] reassigned The reassigned pattern flags.
 * @param[in]     iPat       The index of the current training pattern to 
 *  assigned to a cluster.
 *
 * @return true or false weither The pattern has been added to a cluster or not.
 */
static bool try_next_candidate(InParam param, Vector *pats, Vector *clusts,
                               Vector *reassigned, ulong iPat){
    bool trueValue = true;
    Vector *candProt;                   // Vector of chars
    // Current pattern in 'pats'
    Vector *pat = vec_get(pats, iPat);  // Vector of chars
    // Index of the cluster with highest prototype score in 'clusts'
    ulong iCandidat = NOT_FOUND;
    // Cluster prototype with highest score
    Cluster *candidat = nearest_prot(&iCandidat, pat, clusts, param.beta);
    // Used to compute pattern / prototype similarity level (using param.beta)
    double ppSimA, ppSimB, similarity;

    // No cluster or they're all inhibited: create a new cluster
    if(vec_size(candidat->prot) == 0){
        clust_add_new(clusts, pats, iPat);
        vec_replace_at(reassigned, iPat, &trueValue);
        iVector.Finalize(candidat->prot);
        free(candidat);
        return true;
    }
    // Inhibit the current candidate
    candidat->inhib = true;
    // Use beta to determine if candidate prototype and pat are similar enough
    candProt = vec_copy(candidat->prot);
    ppSimA = (double)comOnes(candProt, pat) / (param.beta + ones(candProt));
    ppSimB = (double)ones(pat) / (param.beta + vec_size(pat));
    // If they are: determine if pat fits in the candidate cluster
    if(ppSimA >= ppSimB){
        similarity = (double)comOnes(candProt, pat) / ones(pat);
        iVector.Finalize(candProt);
        // If vigilance is reached: add pattern to candidate cluster
        if(similarity >= param.vigilance){
            if(clust_add_pat(pat, pats, clusts, iPat, iCandidat)){
                vec_replace_at(reassigned, iPat, &trueValue);
            }
            return true;
        }
        // Else: choose a new candidate prototype
        else{
            return false;
        }
    }
    // Else: create a new cluster
    else{
        clust_add_new(clusts, pats, iPat);
        vec_replace_at(reassigned, iPat, &trueValue);
        iVector.Finalize(candProt);
        return true;
    }
}

/** Set every elements of the vector to false.
 *
 * The reassigned vector contains one flag for each pattern of the training
 * patterns set which indicate if the pattern has been reassigned or not.
 * At the begining of every passes these flags must be set back to false.
 *
 * @param[in,out] reassigned The vector containing the flags to reset.
 */
static void reset_reassigned(Vector *reassigned){
    ulong i;
    bool falseValue = false;

    for(i = 0; i < vec_size(reassigned); i++){
        vec_replace_at(reassigned, i, &falseValue);
    }
}

/** Set the inhib flag of every elements of the vector to false.
 *
 * The clusts vector containg the clusters of the network. Each cluster has an
 * "inhib" flag which indicate if it has been inhibited or not.
 * Before trying to add a pattern in a cluster, every clusters inhib flag must
 * be set back to false.
 *
 * @param[in,out] clusts The vector containing the clusters.
 */
static void reset_clusts_inhib_flags(Vector *clusts){
    ulong i;

    for(i = 0; i < vec_size(clusts); i++){
        get_inhib(vec_get_as_clust(clusts, i)) = false;
    }
}

/** Train the network using the training patterns set and the parameters.
 *
 * It loops until at least one stop condition is met. There's two stop 
 * conditions:
 *  - Stop when the number of performed passes reaches the maximum number of 
 *    passes (par->maxPasses).
 *  - Stop when the fluctuation rate is lower than or equal to the maximum 
 *    fluctuation percentage (par->minFluc).
 *
 * Inside this while loop there's a for loop which loops for each training 
 * patterns.
 * And inside this for loop there's a second while loop which loops until the 
 * pattern is successfuly added to an existing or a new cluster.
 *
 * The function looks like::
 *
 * LOOP WHILE ( pass < maxPasses ) AND ( fluc > minFluc ):
 *     LOOP FOR pat IN pats:
 *         DO:
 *             candidat = pick_a_candidat(clusters, pat)
 *         WHILE NOT fits_in_candidat(candidat, pat)
 *
 * @param[out] bestClusts 
 * @param[out] bestFluc
 * @param[in]  pats 
 * @param[in]  par 
 */
void network_train(Vector **bestClusts, float *bestFluc, InParam par,
                   Vector *pats){
    ulong i;
    // Index of current pattern 'pat' in 'pats'
    ulong iPat;
    // Number of reassigned patterns
    ulong noReassigned;
    // Number of passes (can't exceed par.maxPasses)
    ulong pass = 0;
    // Have to pass reference to vec_add and vec_pushback
    bool trueValue = true;
    /* Every clusters (prototypes, patterns sets and inhib flag)
     * modified in: - try_next_candidat->clust_rm_pat
     *              - try_next_candidat->clust_rm_pat->prot_add_pat
     *              - try_next_candidat->clust_add_new
     *              - try_next_candidat->clust_add_new->clust_rm_pat
     *              - try_next_candidat->clust_add_pat
     *              - try_next_candidat->clust_add_pat->clust_rm_pat
     *              - try_next_candidat->clust_add_pat->prot_add_pat
     */
    Vector *clusts;         // Vector of Cluster
    /* Tell if a pattern were reassigned to another cluster
     * modified in: - here
     *              - try_next_candidat
     */
    Vector *reassigned;     // Vector of bool
    // percentage of reassigned patterns (starting at 100%)
    float fluc = 100;

    printf("Pass n° | No. reassigned | Fluctuation | No. clusters\n");
    printf("--------+----------------+-------------+-------------\n");
    *bestFluc = fluc + 1;    // starting at an impossible value
    clusts = iVector.Create(sizeof(Cluster), 1);
    reassigned = iVector.Create(sizeof(bool), vec_size(pats));
    for(i = 0; i < vec_size(pats); i++){
        iVector.Add(reassigned, &trueValue);
    }
    srand((unsigned int)time(0)); 

    // loop while pass < maxPasses and fluc > minFluc
    while((pass < par.maxPasses) && (fluc > par.minFluc)){
        // start of pass: no pattern have been reassigned yet
        reset_reassigned(reassigned);
        // for each training pattern
        for(iPat = 0; iPat < vec_size(pats); iPat++){
            // next iteration pattern: no prototype have been inhibited yet
            reset_clusts_inhib_flags(clusts);
            do{
                if(try_next_candidate(par, pats, clusts, reassigned, iPat)){
                    break;
                }
            }
            while(vec_size(clusts) != vec_size(pats));
        }
        // compute pass statistics
        compute_pass_stats(&noReassigned, &fluc, reassigned, pats);
        pass++;
        printf("%7lu | %14lu | %10g%% | %12lu\n", pass, noReassigned, fluc, 
               vec_size(clusts));
        // if new best pass: set the best statistics with its statistics
        if(fluc < *bestFluc){
            *bestClusts = clusts;
            *bestFluc = fluc;
        }
    }
    // free
    iVector.Finalize(reassigned);
}

static ulong test_next_candidate(InParam par, Vector *clusts,
                                 Vector *pats, ulong iPat){
    ulong iCandidat = NOT_FOUND;

    nearest_prot(&iCandidat, vec_get(pats, iPat), clusts, par.beta);
    return iCandidat;
}

void network_test(Vector **testResClasses, Vector *clusts, InParam par,
                  Vector *pats, Vector *clustsClasses, Vector *classes){
    ulong iPat;
    ulong clustID;
    char *clustClass;
    char *patClass;
    ulong success = 0;
    ulong fail = 0;

    // For each test pattern
    for(iPat = 0; iPat < vec_size(pats); iPat++){
        // Index of cluster prototype with highest score in clusts
        clustID = test_next_candidate(par, clusts, pats, iPat);
        // Class of the best matching cluster
        clustClass = vec_get_as_str(clustsClasses, clustID);
        // Class of the testing pattern
        patClass = vec_get_as_str(classes, iPat);
        // Save the cluster class in the vector
        vec_pushback(*testResClasses, &clustClass);
        // Increment success or fail wether the cluster and the testing pattern 
        // classes match
        if(strcmp(clustClass, patClass) == 0){
            success++;
        }
        else{
            fail++;
        }
    }
    printf("SUCCESS: %lu (%g%%)\n",
           success, success * 100 / (float)(success + fail));
    printf("FAIL: %lu (%g%%)\n",
            fail, fail * 100 / (float)(success + fail));
    printf("RATIO: %g\n", success / (float)fail);
}
