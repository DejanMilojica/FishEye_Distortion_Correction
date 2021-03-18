/**
 ***************************************************************************************************
 *
 * \file  correction_distortion.h
 *
 * \brief This file contains API for helper functions for correction of lens distortion process
 *
 * \arg author:  Dejan Milojica
 * \arg version: 1.2
 * \arg date:    13.01.2021
 *
 ***************************************************************************************************
 */

#ifndef CORRECTION_DISTORTION_H
#define CORRECTION_DISTORTION_H

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

#define min(X,Y) ((X) < (Y) ? (X) : (Y))    /* min/max macros. */
#define max(X,Y) ((X) > (Y) ? (X) : (Y))
#define ABS(x) ((x) < 0 ? -(x) : (x))       /* Double abs function.  */

#define PARAMETERS_FILE_OPENING_ERROR_MESSAGE (\
    "Error opening parameters file.\n")

#define LENS_PARAMETERS_FILE_READING_ERROR_MESSAGE (\
    "Error while reading LENS cameara parameters, from parameters file.\n")

#define COMBINE_YUV_COMPONENTS_ERROR_MESSAGE (\
    "Error, Unsuccesfull YUV combination of components.\n")

#define PIXELS_POSITION_FILE_OPENING_ERROR_MESSAGE (\
    "Error opening file to save pixels position.\n")

#define MAX_NUM_OF_LENS_PARAMETER (180U)       /* FoV with 180 degrees. */

/**
 ***************************************************************************************************
 *
 * \typedef LensSpec
 *
 * \brief   Structure which represents specification of lens.
 *
 ***************************************************************************************************
 */
typedef struct
{
    double angle[MAX_NUM_OF_LENS_PARAMETER];    /* Array of field angle view in degree.  */
    double height[MAX_NUM_OF_LENS_PARAMETER];   /* Array of image height in mm.          */
    uint32_t num_of_useful_elements;            /* Number of elements previous arrays.   */
    double focal_length_in_mm;                  /* Camera focal length in mm.            */
    double sensor_pixel_pitch_in_mm;            /* Sensor pixel pitch in mm.             */
    double scaling_factor;                      /* Image scaling factor.                 */
}LensSpec;

/* ============================================================================================== */
/*                              Internal Helper Functions                                         */
/* ============================================================================================== */

/**
 ***************************************************************************************************
 *
 * \fn     WriteOperation_WritePixelsPosition2File
 *
 * \brief  Generate txt file, and save undistorted pixels positions.
 *         This can help us to get better performance, with same lens, and similar pictures.
 *
 * \param  [In]  filename     Filename of output pixels positions file.
 * \param  [In]  width        Width of frame.
 * \param  [In]  height       Height of frame.
 * \param  [In]  matrix       Pixels positions arranged into matrix.
 *
 * \return LDC_Status   Exit status.
 *
 ***************************************************************************************************
 */
LDC_Status WriteOperation_WritePixelsPositions2File(const char* const filename, uint32_t height,
                                                    uint32_t width, const double** const matrix);

/* ============================================================================================== */
/*                                     Function Declarations                                      */
/* ============================================================================================== */

/**
 ***************************************************************************************************
 *
 * \fn     FrameCorrection_CorrectLensDistortion
 *
 * \brief  Generate YUV image without distortion.
 *
 * \param  [Out] YUV_out      Undistorted YUV frame.
 * \param  [In]  Y            Y component of YUV frame.
 * \param  [In]  U            U component of YUV frame.
 * \param  [In]  V            V component of YUV frame.
 * \param  [In]  img_size     Size of frame.
 * \param  [In]  width        Width of frame.
 * \param  [In]  height       Height of frame.
 * \param  [In]  yuv_type     Type of YUV image.
 * \param  [In]  filename     Lens parameter CSV filename
 *
 * \return LDC_Status   Exit status.
 *
 ***************************************************************************************************
 */
LDC_Status FrameCorrection_CorrectLensDistortion(uint8_t** YUV_out, const uint8_t* const Y,
                                                 const uint8_t* const U, const uint8_t* const V,
                                                 uint32_t img_size, uint32_t width,
                                                 uint32_t height, YUV_Type yuv_type,
                                                 const char* const filename);


/**
 ***************************************************************************************************
 *
 * \fn     ComponentsStructure_SplitYUV2Component
 *
 * \brief  Split input YUV image, to their component.
 *
 * \param  [In]  YUV          Input YUV image.
 * \param  [Out] Y            Y component of YUV input image.
 * \param  [Out] U            U component of YUV input image.
 * \param  [Out] V            V component of YUV input image.
 * \param  [In]  width        Width of image.
 * \param  [In]  height       Height of image.
 * \param  [In]  img_size     Size of frame.
 * \param  [In]  yuv_type     Type of YUV image.
 *
 * \return LDC_Status   Exit status.
 *
 ***************************************************************************************************
 */
LDC_Status ComponentsStructure_SplitYUV2Component(const uint8_t* const YUV, uint8_t** Y,
                                                  uint8_t** U, uint8_t** V, uint32_t width,
                                                  uint32_t height, uint32_t img_size,
                                                  YUV_Type yuv_type);



/**
 ***************************************************************************************************
 *
 * \fn     ComponentsStructure_CombineYUVComponents
 *
 * \brief  Combine Y, U, V components to one output YUV image.
 *
 * \param  [Out] YUV_out      Output YUV image.
 * \param  [In]  Y	          Y component of output YUV image.
 * \param  [In]  U            U component of output YUV image.
 * \param  [In]  V            V component of output YUV image.
 * \param  [In]  width        Width of image.
 * \param  [In]  height       Height of image.
 * \param  [In]  img_size     Size of frame.
 * \param  [In]  yuv_type     Type of YUV image.
 *
 * \return LDC_Status   Exit status.
 *
 ***************************************************************************************************
 */
LDC_Status ComponentsStructure_CombineYUVComponents(uint8_t* const YUV_out, const uint8_t* const Y,
                                                    const uint8_t* const U, const uint8_t* const V,
                                                    uint32_t width, uint32_t height,
                                                    uint32_t img_size, YUV_Type yuv_type);


#ifdef __cplusplus
}
#endif

#endif
