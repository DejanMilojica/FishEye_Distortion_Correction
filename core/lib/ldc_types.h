/**
 ***************************************************************************************************
 *
 * \file  ldc_types.h
 *
 * \brief This file contains the enumerations, that defines type of YUV image, and exit status.
 *
 * \arg author:  Dejan Milojica
 * \arg version: 1.2
 * \arg date:    13.01.2021
 *
 ***************************************************************************************************
 */

#ifndef LDC_TYPES_H
#define LDC_TYPES_H

/* ============================================================================================== */
/*                                     Macros & Typedefs                                          */
/* ============================================================================================== */


/**
 ***************************************************************************************************
 *
 * \typedef YUV_Type
 *
 * \brief   Defines possible types of YUV image.
 *
 ***************************************************************************************************
 */
typedef enum
{
    YUV420_NV12     = 1, /* First image YUV type YUV420 nv12 */
    YUV422I_UYVY    = 2  /* Second image YUV type YUV422 UYVY */
} YUV_Type;

/**
 ***************************************************************************************************
 *
 * \typedef LDC_Status
 *
 * \brief   Defines possible types of function exit status.
 *
 ***************************************************************************************************
 */
typedef enum
{
    LDC_STATUS_OK    = 0,  /* Success exit   */
    LDC_STATUS_ERROR = 1,  /* Unsuccess exit */
} LDC_Status;

#endif
