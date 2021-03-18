/**
 ***************************************************************************************************
 *
 * \file  read_save_YUV.h
 *
 * \brief This file contains API for helper functions for YUV image operations(Read/Write)
 *
 * \arg author:  Dejan Milojica
 * \arg version: 1.2
 * \arg date:    13.01.2021
 *
 ***************************************************************************************************
 */

#ifndef READ_SAVE_YUV_FILE_H
#define READ_SAVE_YUV_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================================== */
/*                                       Include Files                                            */
/* ============================================================================================== */

#include <stdint.h>
#include <stdio.h>

#include "../lib/ldc_types.h"


/* ============================================================================================== */
/*                                     Macros & Typedefs                                          */
/* ============================================================================================== */

#define INPUT_FILE_HANDLING_ERROR_MESSAGE (\
    "The input file doesn't exist. Please provide a valid input file path.")

#define OUTPUT_FILE_HANDLING_ERROR_MESSAGE (\
    "There was a problem opening the output file.")

#define READING_INPUT_FILE_ERROR_MESSAGE (\
    "There was a problem reading the input file.")

/* ============================================================================================== */
/*                              Internal Helper Functions                                         */
/* ============================================================================================== */

/* ============================================================================================== */
/*                                     Function Declarations                                      */
/* ============================================================================================== */

/**
 ***************************************************************************************************
 *
 * \fn     FileOperation_ReadRawYUV
 *
 * \brief  Read a raw yuv image file.
 *
 * \param  [In]  filename     filename of image to read.
 * \param  [Out] YUV          YUV image that we need to read.
 * \param  [In]  width        width of image!
 * \param  [In]  height       height of image!
 * \param  [In]  img_size     Size of image!
 * \param  [In]  yuv_type     Type of YUV image!
 *
 * \return LDC_Status   Exit status!
 *
 ***************************************************************************************************
 */
LDC_Status FileOperation_ReadRawYUV(const char* const filename, uint8_t** YUV, uint32_t width,
                                    uint32_t height, uint32_t* img_size, YUV_Type yuv_type);

/**
 ***************************************************************************************************
 *
 * \fn     FileOperation_SaveRawYUV
 *
 * \brief  Write a raw yuv image file.
 *
 * \param  [In]  filename     filename of image to save.
 * \param  [In]  yuv_image    YUV image that we need to save.
 * \param  [In]  img_size     Size of image!
 * \param  [In]  width        width of image!
 * \param  [In]  height       height of image!
 *
 * \return LDC_Status   Exit status!
 *
 ***************************************************************************************************
 */
LDC_Status FileOperation_SaveRawYUV(const char* const filename, const uint8_t* const yuv_image,
                                    uint32_t img_size, uint32_t width, uint32_t height);

#ifdef __cplusplus
}
#endif

#endif
