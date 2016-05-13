#ifndef _UTS_H_
#define _UTS_H_

/*=====| INCLUDES |===========================================================*/
#include <stdbool.h>
#include <limits.h>
#include "containers.h"

/*=====| DEFINES |============================================================*/
#define NOT_FOUND ULONG_MAX
#define TRAIN 200
#define TEST 300
#define vec_get_as_str(pat, idx) *(char **)iVector.GetElement(pat, idx)
#define vec_get_as_char(pat, idx) *(char *)iVector.GetElement(pat, idx)
#define vec_get_as_int(pat, idx) *(int *)iVector.GetElement(pat, idx)
#define vec_get_as_ulong(pat, idx) *(ulong *)iVector.GetElement(pat, idx)
#define vec_get_as_size_t(pat, idx) *(size_t *)iVector.GetElement(pat, idx)
#define vec_get_as_vec(pat, idx) (Vector *)iVector.GetElement(pat, idx)
#define vec_get_as_clust(pat, idx) *(Cluster *)iVector.GetElement(pat, idx)
#define vec_get_as_CSVLine(pat, idx) *(CSVLine *)iVector.GetElement(pat, idx)
#define vec_get(pat, idx) iVector.GetElement(pat, idx)
#define vec_size(vec) iVector.Size(vec)
#define vec_replace_at(vec, idx, data) iVector.ReplaceAt(vec, idx, data)
#define vec_erase(vec, elem) iVector.Erase(vec, elem)
#define vec_clear(vec) if(vec_size(vec) > 0) { iVector.Clear(vec); }
#define vec_add(vec, data) iVector.Add(vec, data)
#define vec_pushback(vec, data) iVector.PushBack(vec, data)
#define vec_copy(vec) iVector.Copy(vec)
#define vec_equal(vec1, vec2) iVector.Copy(vec)
#define vec_set_cmp_fun(vec, fun) iVector.SetCompareFunction(vec, fun)
#define get_pat_set(clust) (&clust)->patSet
#define get_prot(clust) (&clust)->prot
#define get_inhib(clust) (&clust)->inhib

/*=====| TYPEDEFS & STRUCTURES |==============================================*/
typedef unsigned long ulong;    // For the sake of clarity

/** The structure holds an input file line.
 */
typedef struct {
   /** @var CSVLine::class
    * The class is the first column of every valid line of the input dataset 
    * file (for now, it can only be the first column). In the mushrooms dataset
    * it can be 'p' for poisonous or 'e' for edible
    */
	char *class;

   /** @var CSVLine::val
    * The val is the actual pattern of the input file line.
    */
    Vector *val;    // Vector of char
} CSVLine;

/** The structure for a cluster, holding its prototype and assigned patterns.
 */
typedef struct {
    /** @var Cluster::prot
     * The cluster prototype. it's the centroid of the cluster, it's basically 
     * a pattern wich contains 1 only where every patterns of the cluster have 
     * a 1.
     */
    Vector *prot;       // Vector of chars

    /** @var Cluster::patSet
     * patSet is a set of patterns ID which belong to the cluster. It means
     * that the previous prot vector has been shaped by these patterns when
     * they where added to the cluster.
     */
    Vector *patSet;    // Vector of ulongs

    /** @var Cluster::inhib
     * The inhib boolean is a flag indicated whether the cluster is inhibited.
     */
    bool inhib;
} Cluster;

/** This structure holds every parameters of the network.
 *
 * The parameters can be set by the user via the "trainart" shell script and are
 * initialized in the io.set_network_values function.
 */
typedef struct {
    /** @var InParam::beta
     * The beta value is used when testing patterns against clusters it
     * influences the amount of created clusters.
     */
    float beta;

    /** @var InParam::skip
     * The skip flag indicates weither the first input file column should be
     * concidered as a class name.
     */
    bool skip;

    /** @var InParam::vigilance
     * The vigilance is the more important parameter of the art network. It act
     * as a pattern acceptance threshold  for the clusters. The higher the
     * vigilance the pickier the threshold, the higher the amount of clusters.
     */
    float vigilance;

    /** @var InParam::maxPasses
     * The maxPasses indicates the maximum number of pass that the training will
     * carry out.
     */
    unsigned long maxPasses;

    /** @var InParam::minFluc
     * The minFluc indicates the minimum percentage of fluctuation. If pass
     * ever present a lower fluctuation percentage then the training stage
     * stops.
     */
    float minFluc;

    /** @var InParam::trainNoise
     * The trainNoise variable indicates the noise percentage to add to the
     * training patterns.
     */
    int trainNoise;

    /** @var InParam::testNoise
     * The testNoise variable indicates the noise percentage to add to the
     * testing patterns.
     */
    int testNoise;

    /** @var InParam::prefix
     * The prefix is the string that will be added to output files containing
     * the results.
     */
    char prefix[PATH_MAX];

    /** @var InParam::trainFile
     * trainFile is the input file: the csv file which is the data set.
     */
    char trainFile[PATH_MAX];

    /** @var InParam::testFile
     * testFile contains the patterns to test.
     */
    char testFile[PATH_MAX];
} InParam;   

/*=====| PROTOTYPES |=========================================================*/
int cmpFun(const void *elem1, const void *elem2, CompareInfo *ExtraArgs);
unsigned long pat_in_clust_set(const Vector *clust, unsigned long set);
unsigned long pat_in_set(Vector *set, Vector *pat);
void line_val_to_vec(Vector *res, List *lines);
void line_class_to_vec(Vector *res, List *lines);

#endif
