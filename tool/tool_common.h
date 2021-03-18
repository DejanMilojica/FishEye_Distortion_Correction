/**
 ***************************************************************************************************
 *
 * \file  tool_common.h
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

#ifndef TOOL_COMMON_H
#define TOOL_COMMON_H

/* ============================================================================================== */
/*                                       Include Files                                            */
/* ============================================================================================== */

#include <stdint.h>
#include <stdio.h>
#include "../core/lib/ldc_types.h"
#include "../core/correction_distortion/correction_distortion.h"
#include "../core/read_save_YUV/read_save_YUV.h"

/* ============================================================================================== */
/*                              Global Variables                                                  */
/* ============================================================================================== */

#define HELP_MESSAGE (\
    "Tool supports one of the following arguments:\n"\
    "--help                   Help\n"\
    "-i [YUV FILE]            Input file\n"\
    "-o [YUV FILE]            Output file\n"\
    "-p [CSV FILE]            Lens camera specification file\n"\
    "-w [WIDTH]               Frame width\n"\
    "-h [HEIGHT]              Frame height\n"\
    "-f [FORMAT]              Format of YUV frame\n"\
    "\n"\
    "Supported frame formats:\n"\
    "1:       YUV_420_NV12      12 bpp\n"\
    "2:       YUV422I_UYVY      16 bpp \n")

#define MISSING_ARGUMENTS_MESSAGE (\
    "Missing arguments. Run the program with --help for help on how to use the tool.\n")

#define INVALID_ARGUMENTS_MESSAGE (\
    "Invalid arguments. Run the program with --help for help on how to use the tool.\n")

#define INVALID_WIDTH_PARAM_MESSGE (\
    "Invalid width parameter. Width has to be a integer greater that zero.\n")

#define INVALID_HEIGHT_PARAM_MESSGE (\
    "Invalid height parameter. Width has to be a integer greater that zero.\n")

#define INVALID_FILE_NAME_MESSAGE (\
    "Invalid file names. File names can't be left empty, and must have appropriate extensions.\n")

#define INVALID_DIMENSION_MESSAGE (\
    "Invalid frame dimensions. Frame dimensions have to be positive integers.\n")

#define INVALID_FORMAT_MESSAGE (\
    "Invalid file format. Run the tool with --help to see the supported format list.\n")

#define INVALID_MODE_MESSAGE (\
    "You must provide a flag for the tool run yuv_type and only one yuv_type can be provided."\
    "Run the tool with --help to see available run yuv_types.\n")

#define SPLIT_YUV_COMPONENTS_ERROR_MESSAGE (\
    "Error in split YUV components. Check type of YUV format, or main YUV file.\n")

#define CORRECTION_DISTORTION_ERROR_MESSAGE (\
    "Error in correction of frame distortion. Check YUV components.\n")

/* ============================================================================================== */
/*                                     Macros & Typedefs                                          */
/* ============================================================================================== */

/* ============================================================================================== */
/*                                     Function Declarations                                      */
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
void ToolCommon_PrintHelpMenu(void);

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
void ToolCommon_PrintInvalidArgsMessage(void);

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
                                        const char* const pInputLensParametersFileName);

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
LDC_Status ToolCommon_ValidateFormat(uint32_t pFrameFormat, YUV_Type* yuv_type);

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
LDC_Status ToolCommon_ValidateDimensions(uint32_t pFrameWidth, uint32_t pFrameHeight);


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
                                           uint32_t height, YUV_Type yuv_type);
#endif
