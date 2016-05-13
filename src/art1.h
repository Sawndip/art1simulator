#ifndef _ART1_H_
#define _ART1_H_

/*=====| INCLUDES |===========================================================*/
#include "utls.h"

/*=====| DEFINES |============================================================*/
#define NONE -1

/*=====| PROTOTYPES |=========================================================*/
void network_train(Vector **bestClusts, float *bestFluc, InParam par,
                   Vector *pats);
void network_test(Vector **testClasses, Vector *bestClusts, InParam par,
                  Vector *pats, Vector *clustsClasses, Vector *classes);

#endif
