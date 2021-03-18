/**
 ***************************************************************************************************
 *
 * \file  tool_main.c
 *
 * \brief This file contains main function, and demonstrate basic usage of distortion correction.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "tool_common.h"

/* ============================================================================================== */
/*                              Internal Helper Functions                                         */
/* ============================================================================================== */

void ToolMain_MemoryFree(uint8_t* YUV_in, uint8_t* YUV_out, uint8_t* Y_in,
                         uint8_t* U_in, uint8_t* V_in)
{
    free(YUV_out);
    free(Y_in);
    free(U_in);
    free(V_in);
    free(YUV_in);

}

/* ============================================================================================== */
/*                                        Main Function                                           */
/* ============================================================================================== */

int32_t main(int32_t argc, char** argv)
{

    /* Input parametars declaration, and preset init. */
    char* inputFileName              = NULL;
    char* outputFileName             = NULL;
    char* inputLensFileParameters    = NULL;

    uint32_t argIteratorCounter         = 1;
    uint32_t frameFormat                = 0;
    uint32_t frameWidth                 = 0;
    uint32_t frameHeight                = 0;
    uint32_t img_size;
    YUV_Type yuv_type;

    /* YUV format declaration! */
    uint8_t* YUV_in     = NULL;
    uint8_t* YUV_out    = NULL;
    uint8_t* Y_in       = NULL;
    uint8_t* U_in       = NULL;
    uint8_t* V_in       = NULL;


    /* Minimum must be exe file and --help option. */
    if (argc < 2)
    {
        printf(MISSING_ARGUMENTS_MESSAGE);
        return EXIT_FAILURE;
    }

    /* Parsing command line arguments. */
    while (argIteratorCounter < argc)
    {
        if (0 == strcmp(argv[argIteratorCounter], "--help"))
        {
            ToolCommon_PrintHelpMenu();
            return EXIT_SUCCESS;
        }
        else if (0 == strcmp(argv[argIteratorCounter], "-i"))
        {
            if (argIteratorCounter + 1 < argc)
            {
                inputFileName = argv[argIteratorCounter + 1];
                argIteratorCounter++;
            }
            else
            {
                printf(INVALID_FILE_NAME_MESSAGE);
                return EXIT_FAILURE;
            }
        }
        else if (0 == strcmp(argv[argIteratorCounter], "-o"))
        {
            if (argIteratorCounter + 1 < argc)
            {
                outputFileName = argv[argIteratorCounter + 1];
                argIteratorCounter++;
            }
            else
            {
                printf(INVALID_FILE_NAME_MESSAGE);
                return EXIT_FAILURE;
            }
        }
        else if (0 == strcmp(argv[argIteratorCounter], "-p"))
        {
            if (argIteratorCounter + 1 < argc)
            {
                inputLensFileParameters = argv[argIteratorCounter + 1];
                argIteratorCounter++;
            }
            else
            {
                printf(INVALID_FILE_NAME_MESSAGE);
                return EXIT_FAILURE;
            }
        }
        else if (0 == strcmp(argv[argIteratorCounter], "-w"))
        {
            if (argIteratorCounter + 1 < argc)
            {
                frameWidth = atoi(argv[argIteratorCounter + 1]);
                argIteratorCounter++;
            }
            else
            {
                printf(INVALID_WIDTH_PARAM_MESSGE);
                return EXIT_FAILURE;
            }
        }
        else if (0 == strcmp(argv[argIteratorCounter], "-h"))
        {
            if (argIteratorCounter + 1 < argc)
            {
                frameHeight = atoi(argv[argIteratorCounter + 1]);
                argIteratorCounter++;
            }
            else
            {
                printf(INVALID_HEIGHT_PARAM_MESSGE);
                return EXIT_FAILURE;
            }
        }
        else if (0 == strcmp(argv[argIteratorCounter], "-f"))
        {
            if (argIteratorCounter + 1 < argc)
            {
                frameFormat = atoi(argv[argIteratorCounter + 1]);
                argIteratorCounter++;
            }
            else
            {
                printf(INVALID_FORMAT_MESSAGE);
                return EXIT_FAILURE;
            }
        }
        else
        {
            printf(INVALID_ARGUMENTS_MESSAGE);
            return EXIT_FAILURE;
        }

        argIteratorCounter++;
    }


    /* Validating image dimensions. */
    if (ToolCommon_ValidateDimensions(frameWidth, frameHeight))
    {
        printf(INVALID_DIMENSION_MESSAGE);
        return EXIT_FAILURE;
    }

    /* Validate input/output image filename, and Lens parameters filename. */
    if (ToolCommon_ValidateFileNames(inputFileName, outputFileName, inputLensFileParameters))
    {
        printf(INVALID_FILE_NAME_MESSAGE);
        return EXIT_FAILURE;
    }

    /* Validate YUV format. */
    if (ToolCommon_ValidateFormat(frameFormat, &yuv_type))
    {
        printf(INVALID_MODE_MESSAGE);
        return EXIT_FAILURE;
    }

    /* Read input YUV image data. */
    if(FileOperation_ReadRawYUV(inputFileName, &YUV_in, frameWidth, frameHeight,
                                &img_size, yuv_type))
    {
        return EXIT_FAILURE;
    }


    /* Split YUV to their Y,U,V components. */
    if(ComponentsStructure_SplitYUV2Component(YUV_in, &Y_in, &U_in, &V_in, frameWidth,
                                              frameHeight, img_size, yuv_type))
    {
        ToolMain_MemoryFree(YUV_in, YUV_out, Y_in, U_in, V_in);
        printf(SPLIT_YUV_COMPONENTS_ERROR_MESSAGE);
        return EXIT_FAILURE;
    }

    /* Correction of image distortion. */
    if(FrameCorrection_CorrectLensDistortion(&YUV_out, Y_in, U_in, V_in, img_size,
                                             frameWidth, frameHeight, yuv_type,
                                             inputLensFileParameters))
    {
        ToolMain_MemoryFree(YUV_in, YUV_out, Y_in, U_in, V_in);
        printf(CORRECTION_DISTORTION_ERROR_MESSAGE);
        return EXIT_FAILURE;
    }

    /* Save YUV output data. */
    if(FileOperation_SaveRawYUV(outputFileName, YUV_out, img_size, frameWidth, frameHeight))
    {
        ToolMain_MemoryFree(YUV_in, YUV_out, Y_in, U_in, V_in);
        return EXIT_FAILURE;
    }

    /* Print necessary information. */
    ToolCommon_PrintCorrectionInformation(inputFileName, outputFileName, inputLensFileParameters,
                                          img_size, frameWidth, frameHeight, yuv_type);

    /* Free allocated memory. */
    ToolMain_MemoryFree(YUV_in, YUV_out, Y_in, U_in, V_in);

    return EXIT_SUCCESS;
}


