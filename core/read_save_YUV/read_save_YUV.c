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

/* ============================================================================================== */
/*                                       Include Files                                            */
/* ============================================================================================== */

#include "read_save_YUV.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

/* ============================================================================================== */
/*                                       Global variables                                         */
/* ============================================================================================== */

/* ============================================================================================== */
/*                              Internal Helper Functions                                         */
/* ============================================================================================== */

/* ============================================================================================== */

/*                                     API Functions                                              */

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
 * \return LDC_Status   Exit status.
 *
 ***************************************************************************************************
 */
LDC_Status FileOperation_ReadRawYUV(const char* const filename, uint8_t** YUV, uint32_t width,
                                    uint32_t height, uint32_t* img_size, YUV_Type yuv_type)
{
    uint32_t expected_size = width * height + 2 * (width / 2) * ((int32_t)yuv_type * height / 2);
    LDC_Status status = LDC_STATUS_OK; /* EXIT Status. */

    FILE* fp = fopen(filename, "rb");

    if (NULL == fp)
    {
        printf(INPUT_FILE_HANDLING_ERROR_MESSAGE);
        status = LDC_STATUS_ERROR;
    }
    else
    {
        /* Check file size. */
        fseek(fp, 0, SEEK_END);

        /* Set file size. */
        *img_size = ftell(fp);

        if(*img_size != expected_size)
        {
            fprintf(stderr, "Wrong size of yuv image : %d bytes, expected %d bytes\n",
            *img_size, expected_size);

            status = LDC_STATUS_ERROR;
        }
        else
        {
            size_t result;

            /* Back to file begining. */
            fseek(fp, 0, SEEK_SET);

            *YUV = malloc(*img_size * sizeof(uint8_t));

            result = fread(*YUV, 1, *img_size, fp);

            if(result != *img_size)
            {
                printf(READING_INPUT_FILE_ERROR_MESSAGE);
                status = LDC_STATUS_ERROR;
            }

        }

        fclose(fp);
    }

    return status;
}

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
 * \return LDC_Status   Exit status.
 *
 ***************************************************************************************************
 */
LDC_Status FileOperation_SaveRawYUV(const char* const filename, const uint8_t* const yuv_image,
                                    uint32_t img_size, uint32_t width, uint32_t height)
{
    LDC_Status status = LDC_STATUS_OK; /* EXIT Status. */
    FILE* fp = fopen(filename, "wb");

    if (NULL == fp)
    {
        printf(OUTPUT_FILE_HANDLING_ERROR_MESSAGE);
        status = LDC_STATUS_ERROR;
    }
    else
    {
        fwrite(yuv_image, 1, img_size, fp);
        fclose(fp);
    }

    return status;
}

