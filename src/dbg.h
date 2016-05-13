#ifndef _DBG_H_
#define _DBG_H_

/*=====| INCLUDES |===========================================================*/
#include <stdio.h>
#include <string.h>

/*=====| DEFINES |============================================================*/
#define DEBUG 1

#if defined(DEBUG) && DEBUG > 0
#define FILE_NAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define DEBUG_PRINT(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, CNRM, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_BLK(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, CBLK, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_RED(fmt, args...)\
        fprintf(stderr, "%sDEBUG: %s:%d:%s(): %s" fmt,\
                CRED, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_GRN(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, CGRN, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_YEL(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, CYEL, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_BLU(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, CBLU, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_MAG(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, CMAG, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_CYN(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, CCYN, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_WHT(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, CWHT, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_BNRM(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, BNRM, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_BBLK(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, BBLK, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_BRED(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, BRED, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_BGRN(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, BGRN, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_BYEL(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, BYEL, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_BBLU(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, BBLU, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_BMAG(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, BMAG, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_BCYN(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, BCYN, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_BWHT(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, BWHT, FILE_NAME, __LINE__, __func__, CNRM, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#define DEBUG_PRINT_GRN_L(fmt, args...)\
        fprintf(stderr, "%sDEBUG:%s %s:%d:%s(): %s" fmt,\
                CRED, CGRN, FILE_NAME, __LINE__, __func__, CNRM, ##args);
#define DEBUG_PRINT_L(fmt, args...)\
        fprintf(stderr, "" fmt, ##args);
#define DEBUG_PRINT_NL(fmt, args...)\
        fprintf(stderr, "" fmt, ##args);\
        fprintf(stderr, "%s\n", CNRM);
#else
#define DEBUG_PRINT(fmt, args...)
#define DEBUG_PRINT_BLK(fmt, args...)
#define DEBUG_PRINT_RED(fmt, args...)
#define DEBUG_PRINT_GRN(fmt, args...)
#define DEBUG_PRINT_YEL(fmt, args...)
#define DEBUG_PRINT_BLU(fmt, args...)
#define DEBUG_PRINT_MAG(fmt, args...)
#define DEBUG_PRINT_CYN(fmt, args...)
#define DEBUG_PRINT_WHT(fmt, args...)
#define DEBUG_PRINT_BNRM(fmt, args...)
#define DEBUG_PRINT_BBLK(fmt, args...)
#define DEBUG_PRINT_BRED(fmt, args...)
#define DEBUG_PRINT_BGRN(fmt, args...)
#define DEBUG_PRINT_BYEL(fmt, args...)
#define DEBUG_PRINT_BBLU(fmt, args...)
#define DEBUG_PRINT_BMAG(fmt, args...)
#define DEBUG_PRINT_BCYN(fmt, args...)
#define DEBUG_PRINT_BWHT(fmt, args...)
#define DEBUG_PRINT_GRN_L(fmt, args...)
#define DEBUG_PRINT_L(fmt, args...)
#define DEBUG_PRINT_NL(fmt, args...)
#endif

#define CNRM  "\x1B[0m"
#define CBLK  "\x1B[30m"
#define CRED  "\x1B[31m"
#define CGRN  "\x1B[32m"
#define CYEL  "\x1B[33m"
#define CBLU  "\x1B[34m"
#define CMAG  "\x1B[35m"
#define CCYN  "\x1B[36m"
#define CWHT  "\x1B[37m"

#define BNRM  "\x1B[1m\x1b[0m"
#define BBLK  "\x1B[1m\x1b[30m"
#define BRED  "\x1B[1m\x1b[31m"
#define BGRN  "\x1B[1m\x1b[32m"
#define BYEL  "\x1B[1m\x1b[33m"
#define BBLU  "\x1B[1m\x1b[34m"
#define BMAG  "\x1B[1m\x1b[35m"
#define BCYN  "\x1B[1m\x1b[36m"
#define BWHT  "\x1B[1m\x1b[37m"

#define BCRED  "\x1B[41m"
#define BCGRN  "\x1B[42m"
#define BCWHT  "\x1B[47m"


#endif
