/**
 ***************************************************************************************************
 *
 * \file  tool_common.c
 *
 * \brief This file contains definition of API functions for helper functions used on Correction
 *        of YUV image distortion.
 *
 * \arg author:  Dejan Milojica
 * \arg version: 1.2
 * \arg date:    13.01.2021
 *
 ***************************************************************************************************
 */

/* ============================================================================================== */
/*                               Internal Include Files                                           */
/* ============================================================================================== */

#include "tool_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ============================================================================================== */
/*                              Internal Helper Functions                                         */
/* ============================================================================================== */

/* ============================================================================================== */
/*                                     API Functions                                              */
/* ============================================================================================== */

/**
 ***************************************************************************************************
 *
 * \fn     ToolCommon_PrintHelpMenu
 *
 * \brief  Helper function used to print the tool help menu
 *
 * \return void
 *
 ***************************************************************************************************
 */
void ToolCommon_PrintHelpMenu(void)
{
    printf(HELP_MESSAGE);
}

/**
 ***************************************************************************************************
 *
 * \fn     ToolCommon_ValidateFileNames
 *
 * \brief  Helper function used to validate file names of input and output files
 *
 * \param  [In] pInputFileName                  Filename of input that has to be validated
 * \param  [In] pOutputFileName                 Filename of output file that has to be validated
 * \param  [In] pInputLensParametersFileName    Filename of output file that has to be validated
 *
 * \return LDC_Status   Validation code
 *
 ***************************************************************************************************
 */
LDC_Status ToolCommon_ValidateFileNames(const char* const pInputFileName,
                                        const char* const pOutputFileName,
                                        const char* const pInputLensParametersFileName)
{

    const char* YUV_extension_UC           = ".YUV";
    const char* YUV_extension_LC           = ".yuv";
    const char* LensCSVFile_extension      = ".csv";

    LDC_Status status = LDC_STATUS_OK; /* EXIT Status. */

    /* Check if filenames are NULL pointers. */
    if (NULL == pInputFileName)
    {
        status = LDC_STATUS_ERROR;
    }
    else if (NULL == pOutputFileName)
    {
        status = LDC_STATUS_ERROR;
    }
    else if (NULL == pInputLensParametersFileName)
    {
        status = LDC_STATUS_ERROR;
    }
    else
    {

        /* Check if both file names are YUV frames, and Lens Parameters file is CSV file. */
        if(NULL == strstr(pInputFileName, YUV_extension_UC) &&
           NULL == strstr(pInputFileName, YUV_extension_LC))
        {
            status = LDC_STATUS_ERROR;
        }

        if(NULL == strstr(pOutputFileName, YUV_extension_UC) &&
           NULL == strstr(pOutputFileName, YUV_extension_LC))
        {
            status = LDC_STATUS_ERROR;
        }

        if(NULL == strstr(pInputLensParametersFileName, LensCSVFile_extension))
        {
            status = LDC_STATUS_ERROR;
        }
    }

    return status;
}

/**
 ***************************************************************************************************
 *
 * \fn     ToolCommon_PrintInvalidArgsMessage
 *
 * \brief  Helper function used to print the message about the invalid program arguments
 *
 *
 * \return void
 *
 ***************************************************************************************************
 */
void ToolCommon_PrintInvalidArgsMessage(void)
{
    printf(INVALID_ARGUMENTS_MESSAGE);
}

/**
 ***************************************************************************************************
 *
 * \fn     ToolCommon_ValidateFormat
 *
 * \brief  Helper function used to validate format type of the input frame
 *
 * \param  [In] pFrameFormat        Frame format type that has to be validated
 *
 * \return LDC_Status    Validation code
 *
 ***************************************************************************************************
 */
LDC_Status ToolCommon_ValidateFormat(uint32_t pFrameFormat, YUV_Type* yuv_type)
{

    LDC_Status status = LDC_STATUS_ERROR;

    if(1U == pFrameFormat)
    {
        *yuv_type = YUV420_NV12;
        status = LDC_STATUS_OK;
    }
    else if(2U == pFrameFormat)
    {
        *yuv_type = YUV422I_UYVY;
        status = LDC_STATUS_OK;
    }

    return status;
}

/**
 ***************************************************************************************************
 *
 * \fn     ToolCommon_ValidateDimensions
 *
 * \brief  Helper function used to validate frame dimensions of the input file
 *
 * \param  [In] pFrameWidth         Width of the frame that has to be validated
 * \param  [In] pFrameHeight        Height of the frame that has to be validated
 *
 * \return LDC_Status    Validation code
 *
 ***************************************************************************************************
 */
LDC_Status ToolCommon_ValidateDimensions(uint32_t pFrameWidth, uint32_t pFrameHeight)
{

    LDC_Status status = LDC_STATUS_OK;

    if (0U >= pFrameHeight)
    {
        status = LDC_STATUS_ERROR;
    }
    else if (0U >= pFrameWidth)
    {
        status = LDC_STATUS_ERROR;
    }

    return status;
}


/**
 ***************************************************************************************************
 *
 * \fn     ToolCommon_PrintCorrectionInformation
 *
 * \brief  Helper function used to print neccesary information like input parameters and so.
 *
 * \param  [In] inputFilename        Filename of input file
 * \param  [In] outputFilename       Filename of output file
 * \param  [In] lensSPecFilename     Lens Specifiacation file name
 * \param  [In] img_size             Size of image frame
 * \param  [In] width                Width of the frame that has to be validated
 * \param  [In] height               Height of the frame that has to be validated
 * \param  [In] yuv_type             Type of YUV image
 *
 * \return void
 *
 ***************************************************************************************************
 */
void ToolCommon_PrintCorrectionInformation(const char* const inputFilename,
                                           const char* const outputFilename,
                                           const char* const lensSPecFilename, 
                                           uint32_t img_size, uint32_t width,
                                           uint32_t height, YUV_Type yuv_type)
{
    printf("------------------------------------------------------------------------\n");
    printf("\t\t\tDistortion Correction!!\n");
    printf("------------------------------------------------------------------------\n");
    printf("Input Filename: %s\n", inputFilename);
    printf("Output Filename: %s\n", outputFilename);
    printf("Lens Spec. file: %s\n", lensSPecFilename);
    printf("Frame Dimensions: (%d, %d)\n", width, height);
    printf("Frame Mem. storage: %d B\n", img_size);
    printf("Type of YUV frame: '%s'\n", (1U == yuv_type) ? "YUV420_NV12" : "YUV422I_UYVY");
    printf("-------------------------------------------------------------------------\n");
}
