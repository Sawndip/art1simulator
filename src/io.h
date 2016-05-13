#ifndef _IO_H_
#define _IO_H_

/*=====| INCLUDES |===========================================================*/
#include <stdio.h>
#include "utls.h"

/*=====| DEFINES |============================================================*/
#define RES_SUFFIX "_results"
#define CLUST_SUFFIX "_clust"
#define RES_FOLDER "./results/"
#define CLUST_FOLDER "clusters/"
#define TRAIN_FOLDER "train/"
#define TEST_FOLDER "test/"

/*=====| PROTOTYPES |=========================================================*/
void set_network_values(InParam *par, int argc, const char *argv[]);
void print_network_values(InParam par);
void vec_print_char_as_int(Vector *vec);
void vec_print_as_char(Vector *vec);
void vector_print_ulong(Vector *vec);
int openFile(FILE **file, const char *name, char const *mode);
void readCsv(ulong *plen, List *lines, FILE *file, bool skip);
void write_train_results(Vector **clustsClasses, InParam par, ulong emptyPats,
                         float fluc, Vector *pats, ulong nPats, Vector *clusts, 
                         Vector *patsClass);
void write_test_results(InParam par, ulong emptyPats, Vector *pats,
                        ulong nPats, Vector *clusts, Vector *patsClass,
                        Vector *testResClasses);

#endif
