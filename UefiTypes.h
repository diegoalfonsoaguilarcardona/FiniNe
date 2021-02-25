/**
 * @file :  UefiTypes.h
 * @brief:  This file contains the macro definitions according UEFI code style.
 *
 * Author:  Ricardo Esteven Lucano – TaIO Systems SAS
 * Version: 1.0
 * Date: Aug 1, 2018
 */

#ifndef _UEFITYPES_H_
#define _UEFITYPES_H_

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef NULL
#define NULL    0
#endif

#define STRUCT    struct
#define VOLATILE  volatile
#define STATIC    static
#define CONST   const
#define EXTERN    extern

typedef unsigned    int     UINTN;
typedef signed      int     INTN;
typedef unsigned    int     UINT32;
typedef signed      int     INT32;
typedef unsigned    short   UINT16;
typedef signed      short   INT16;
typedef unsigned    char    UINT8;
typedef signed      char    INT8;
typedef char                CHAR;
typedef float               FLOAT;
typedef unsigned    char    BOOL;
typedef void                VOID;

#endif /* _UEFITYPES_H_ */
