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
 * @file utils.c
 * @author Mathieu Fourcroy
 * @date June 2015
 * @version 0.0.1
 *
 * This file contains the function which are used as general purpose, not for
 * training or testing the network, not for I/O. They are mean to be used in any
 * source file.
 */
/*=====| INCLUDES |===========================================================*/
#include <limits.h>
#include "utls.h"
#include "io.h"
#include "ccl_internal.h"

/*=====| FUNCTIONS |==========================================================*/
int cmpFun(const void *elem1, const void *elem2, CompareInfo *ExtraArgs){
    return !(elem1 == elem2);
}

/** Check if a given pattern is in a given patterns set.
 *
 * @param[in] set The patterns set.
 * @param[in] pat The pattern to check.
 */
ulong pat_in_set(Vector *set, Vector *pat){
    ulong i;
	ulong index = NOT_FOUND;

    vec_set_cmp_fun(pat, cmpFun);

	for(i = 0; i < vec_size(set); i++){
        vec_set_cmp_fun(iVector.GetElement(set, i), cmpFun);
        if(iVector.Equal(iVector.GetElement(set, i), pat)){
            index = i;
            break;
        }
	}
    return index;
}

/** Check if a given pattern is in a cluster patterns set.
 *
 * @param[in] clusts The network clusters.
 * @param[in] pat    The pattern to check.
 *
 * @return The cluster index containing the pattern or NOT_FOUND if the pattern
 *  dosen't belongs to any cluster.
 */
ulong pat_in_clust_set(const Vector *clusts, ulong pat){
    ulong i, j;
	ulong prot_number = NOT_FOUND;
    Vector *elem;

	for(i = 0; i < iVector.Size(clusts); i++){
        elem = get_pat_set(vec_get_as_clust(clusts, i));
		for(j = 0; j < iVector.Size(elem); j++){
			if(*(ulong *)iVector.GetElement(elem, j) == pat){
				prot_number = i;
				break;
            }
		}
		if(prot_number != NOT_FOUND){
            break;
        }
	}
	return prot_number;
}

/** Push each pattern of a list of CSVLine structures into a vector.
 *
 * @param[in] lines The list of CSVLine structures.
 *
 * @return The created vector containing the patterns.
 */
void line_val_to_vec(Vector *res, List *lines){
    ulong i;

    for(i = 0; i < iList.Size(lines); i++){
        vec_pushback(res, (*(CSVLine *)iList.GetElement(lines, i)).val);
    }
}

/** Push each class variable of a list of CSVLine structures into a vector.
 *
 * @note The elements of the vector or of variable size so we must create the 
 *  vector with sizeof(void *) and push a pointer to the elements.
 *
 * @param[in] lines The list of CSVLine structures.
 *
 * @return The created vector containing the classes names.
 */
void line_class_to_vec(Vector *res, List *lines){
    ulong i;

    for(i = 0; i < iList.Size(lines); i++){
        vec_pushback(res, &(*(CSVLine *)iList.GetElement(lines, i)).class);
    }
}
