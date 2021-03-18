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

/* ============================================================================================== */
/*                                       Include Files                                            */
/* ============================================================================================== */

#include "correction_distortion.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>

/* ============================================================================================== */
/*                                       Global variables                                         */
/* ============================================================================================== */


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
                                                    uint32_t width, const double** const matrix)
{
    LDC_Status status = LDC_STATUS_OK; /* EXIT Status. */

    FILE* fp = fopen(filename, "w");
    if (NULL == fp)
    {
        printf(PIXELS_POSITION_FILE_OPENING_ERROR_MESSAGE);
        status = LDC_STATUS_ERROR;
    }
    else
    {
        uint32_t i;
        uint32_t j;

        for(i = 0;i < height;i++)
        {
            for(j = 0;j < width;j++)
            {
                fprintf(fp, "%lf ", matrix[i][j]);
            }
            fprintf(fp,"\n");
        }

        fclose(fp);
    }

    return status;
}


static void cart2pol(uint32_t width, uint32_t height, double** xt, double** yt,
                     double** phi, double **r)
{
    uint32_t i;
    uint32_t j;

    for(i = 0;i < height;i++)
    {
        for(j = 0;j < width;j++)
        {
            /* Pythagorean theorem. */
            r[i][j] = sqrt(pow(xt[i][j], 2) + pow(yt[i][j], 2));

            /*Find Four-Quadrant Inverse Tangent of a Point. */
            phi[i][j] = atan2(yt[i][j], xt[i][j]);
        }

    }

}


static void pol2cart(uint32_t width, uint32_t height, double** h_d, double** v_d,
                     double** phi, double** r_d)
{
    uint32_t i;
    uint32_t j;

    for(i = 0;i < height;i++)
    {
        for(j = 0;j < width;j++)
        {
           /* If we have one side length, and one angle, we can get other two triangle len sides.*/
            h_d[i][j] = r_d[i][j] * cos(phi[i][j]);
            v_d[i][j] = r_d[i][j] * sin(phi[i][j]);
        }
    }

}

static LDC_Status ParamOperation_ReadParametersOfCorrection(LensSpec* lens_spec,
                                                            const char* const filename)
{
    LDC_Status status = LDC_STATUS_OK; /* EXIT Status. */

    FILE* fp = fopen(filename, "r");
    if (NULL == fp)
    {
        printf(PARAMETERS_FILE_OPENING_ERROR_MESSAGE);
        status = LDC_STATUS_ERROR;
    }
    else
    {

        double focal_length;
        double pixel_pitch;
        double scaling_factor;
        double angle;
        double imageHeightInmm;

        /* Read Focal length/ Pixel pitch/ Scaling factor. */

        if((fscanf(fp, "%lf,\n,", &focal_length)) != EOF)
        {
            if((fscanf(fp, "%lf,\n,", &pixel_pitch)) != EOF)
            {
                if((fscanf(fp, "%lf,\n,", &scaling_factor))== EOF)
                {
                    printf(LENS_PARAMETERS_FILE_READING_ERROR_MESSAGE);
                    status = LDC_STATUS_ERROR;
                }
                else
                {
                    lens_spec->focal_length_in_mm = focal_length;
                    lens_spec->sensor_pixel_pitch_in_mm = pixel_pitch;
                    lens_spec->scaling_factor = scaling_factor;

                    uint32_t i = 0;

                    /* Read Angle / Height of Lens Camera. */
                    while (fscanf(fp, "%lf, %lf\n", &angle, &imageHeightInmm) != EOF)
                    {
                        lens_spec->angle[i] = angle / 180 * M_PI; /* Degree to Radian. */
                        lens_spec->height[i++]
                                    = imageHeightInmm / lens_spec->sensor_pixel_pitch_in_mm;
                    }

                    /* Set number of array elements. */
                    lens_spec -> num_of_useful_elements = i - 1;

                    /* If there is no lens parameters, we need to protect our program from crash.*/
                    if(lens_spec -> num_of_useful_elements <= 0)
                    {
                        printf(LENS_PARAMETERS_FILE_READING_ERROR_MESSAGE);
                        status = LDC_STATUS_ERROR;
                    }

                }
            }
            else
            {
                printf(LENS_PARAMETERS_FILE_READING_ERROR_MESSAGE);
                status = LDC_STATUS_ERROR;
            }
        }
        else
        {
            printf(LENS_PARAMETERS_FILE_READING_ERROR_MESSAGE);
            status = LDC_STATUS_ERROR;
        }

        fclose(fp);
    }

    return status;
}

static int32_t findNearestNeighbourIndex(double value, const double* const x, int32_t len)
{
    double dist;
    int32_t idx;
    int32_t i;

    idx = -1;
    dist = DBL_MAX;

    for(i = 0;i < len;i++)
    {
        double new_dist = ABS(value - x[i]); /* New distance. */
        if(new_dist <= dist)
        {
            dist = new_dist;
            idx = i;
        }
    }

    return idx;
}

static double* FrameCorrection_LinearInterpolation(double* x, double* y, int32_t sample_points_size,
                                                   double* xq, int32_t queri_points_size)
{
    double* slope;
    double* intercept;
    double* yq = (double *) malloc(queri_points_size*sizeof(double)); /* Interpolated array. */
    int32_t i;

    slope       = (double *)malloc(sample_points_size*sizeof(double));
    intercept   = (double *)malloc(sample_points_size*sizeof(double));

    for(i = 0;i < sample_points_size;i++)
    {
        if(i < sample_points_size-1)
        {
            double dx;
            double dy;

            dx = x[i + 1] - x[i];
            dy = y[i + 1] - y[i];
            slope[i] = dy / dx;                     /* Define slope of function. */
            intercept[i] = y[i] - x[i] * slope[i];  /* Define intercept of function.*/
        }
        else
        {
            slope[i] = slope[i-1];
            intercept[i] = intercept[i-1];
        }
    }

    for(i = 0;i < queri_points_size;i++)
    {
        int32_t index_of_nearest_number;

        index_of_nearest_number = findNearestNeighbourIndex(xq[i], x, sample_points_size);
        if(index_of_nearest_number != -1)
        {
            yq[i]=slope[index_of_nearest_number] * xq[i] + intercept[index_of_nearest_number];
        }
        else
        {
            yq[i]=DBL_MAX;
        }
    }

    free(slope);
    free(intercept);

    return yq;
}

static LDC_Status FrameCorrection_XYZ2Distorted(const char* const filename, uint32_t width,
                                                uint32_t height, double** x, double** y,
                                                double hc, double vc,
                                                double** h_d, double** v_d)
{
    LensSpec lens_spec;
    LDC_Status status = LDC_STATUS_OK; /* EXIT Status. */

    memset(lens_spec.angle, 0, sizeof(lens_spec.angle));
    memset(lens_spec.height, 0, sizeof(lens_spec.height));

    /* Read necessary lens specification parameters. */
    if(ParamOperation_ReadParametersOfCorrection(&lens_spec, filename))
    {
        status = LDC_STATUS_ERROR;
    }
    else
    {
        uint32_t i;
        uint32_t j;
        double f;
        double z;
        double** xt;
        double** yt;
        double** zt;
        double** phi;
        double** r;
        double** theta;
        double** r_d;

        f = lens_spec.focal_length_in_mm / lens_spec.sensor_pixel_pitch_in_mm;
        z = f / lens_spec.scaling_factor;

        xt    = (double**)malloc(height * sizeof(double*));
        yt    = (double**)malloc(height * sizeof(double*));
        zt    = (double**)malloc(height * sizeof(double*));
        phi   = (double**)malloc(height * sizeof(double*));
        r     = (double**)malloc(height * sizeof(double*));
        theta = (double**)malloc(height * sizeof(double*));
        r_d   = (double**)malloc(height * sizeof(double*));

        for(i = 0;i < height;i++)
        {
            xt[i]    = (double*)malloc(width * sizeof(double));
            yt[i]    = (double*)malloc(width * sizeof(double));
            zt[i]    = (double*)malloc(width * sizeof(double));
            phi[i]   = (double*)malloc(width * sizeof(double));
            r[i]     = (double*)malloc(width * sizeof(double));
            theta[i] = (double*)malloc(width * sizeof(double));
        }

        /* Substract position of centar. */
        for(i = 0;i < height;i++)
        {
            for(j = 0;j < width;j++)
            {
                xt[i][j] = x[i][j] - hc;
                yt[i][j] = y[i][j] - vc;
                zt[i][j] = z;
            }
        }

        /* Cartesian to Polar. */
        cart2pol(width, height, xt, yt, phi, r);

        for(i = 0;i < height;i++)
        {
            for(j = 0;j < width;j++)
            {
                /* Four-Quadrant Inverse Tangent of a Point. */
                theta[i][j] = atan2(r[i][j], zt[i][j]);
            }
        }

        /* Linear Interpolation of matrix. */
        for(i = 0;i < height;i++)
        {
            r_d[i] = FrameCorrection_LinearInterpolation(lens_spec.angle, lens_spec.height,
                                                         lens_spec.num_of_useful_elements,
                                                         theta[i], width);
        }

        /* Polar to Cartesian. */
        pol2cart(width, height, h_d, v_d, phi, r_d);

        /* On every pixel position, add vertical and horizontal position of centar. */
        for(i = 0;i < height;i++)
        {
            for(j = 0;j < width;j++)
            {
                h_d[i][j] += hc;
                v_d[i][j] += vc;
            }
        }

        /*
        WriteOperation_WritePixelsPosition2File("h_d_C.txt",height,width,h_d);
        WriteOperation_WritePixelsPosition2File("v_d_C.txt",height,width,v_d);
        */

        for(i = 0;i < height;i++)
        {
            free(xt[i]);
            free(yt[i]);
            free(zt[i]);
            free(phi[i]);
            free(r[i]);
            free(theta[i]);
            free(r_d[i]);
        }

        free(xt);
        free(yt);
        free(zt);
        free(r);
        free(phi);
        free(theta);
        free(r_d);
    }

    return status;
}


/* ============================================================================================== */

/*                                     API Functions                                              */

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
 * \param  [In]  U	          U component of YUV frame.
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
                                                 const char* const filename)
{

    LDC_Status status = LDC_STATUS_OK; /* EXIT Status. */

    if(NULL == Y || NULL == U || NULL == V)
    {
        status = LDC_STATUS_ERROR;
    }
    else
    {
        uint32_t i;
        uint32_t j;
        uint32_t x;
        uint32_t y;
        uint8_t* Y_out;
        uint8_t* U_out;
        uint8_t* V_out;
        double** h_p;
        double** v_p;
        double** h_d;
        double** v_d;

        Y_out = (uint8_t*)malloc(width * height * sizeof ( uint8_t ));
        U_out = (uint8_t*)malloc((width / 2) * ((int32_t)yuv_type * height / 2)*sizeof(uint8_t));
        V_out = (uint8_t*)malloc((width / 2) * ((int32_t)yuv_type * height / 2)*sizeof(uint8_t));

        *YUV_out = malloc (img_size * sizeof ( uint8_t ));

        h_p = (double**)malloc(height * sizeof(double*));
        v_p = (double**)malloc(height * sizeof(double*));
        h_d = (double**)malloc(height * sizeof(double*));
        v_d = (double**)malloc(height * sizeof(double*));


        for(i = 0;i < height;i++)
        {
            h_p[i] = (double*)malloc(width * sizeof(double));
            v_p[i] = (double*)malloc(width * sizeof(double));
            h_d[i] = (double*)malloc(width * sizeof(double));
            v_d[i] = (double*)malloc(width * sizeof(double));
        }


        /* Create Meshgrid. with*/
        for(i = 0;i < width;i++)
        {
            for(j = 0;j < height;j++)
            {
                h_p[j][i] = i;
            }
        }

        for(i = 0;i < height;i++)
        {
            for(j = 0;j < width;j++)
            {
                v_p[i][j] = i;
            }
        }


        /* Get position of original pixels! Back Mapping. */
        if(FrameCorrection_XYZ2Distorted(filename, width, height, h_p, v_p,
                                         (width - 1)/2, (height - 1)/2, h_d, v_d))
        {
            status = LDC_STATUS_ERROR;
        }
        else
        {
            for(x = 0;x < width;x++)
            {
                for(y = 0;y < height;y++)
                {

                    /* Casting the double coordinates into int. Add +0.5 to get higher position. */
                    int32_t srcX = (int32_t)(h_d[y][x] + 0.5);
                    int32_t srcY = (int32_t)(v_d[y][x] + 0.5);

                    /* Chech if source coordinates are into the image range. */
                    if(srcX >= 0 && srcY >= 0 && srcX < width && srcY < height)
                    {
                        Y_out[y * width + x] = Y[srcY * width + srcX];

                        U_out[(int32_t)((int32_t)yuv_type*y/2) * (int32_t)(width/2) + (int32_t)(x/2)]
                        = U[(int32_t)((int32_t)yuv_type*srcY/2)*(int32_t)(width/2)+(int32_t)(srcX/2)];

                        V_out[(int32_t)((int32_t)yuv_type*y/2) * (int32_t)(width/2) + (int32_t)(x/2)]
                        = V[(int32_t)((int32_t)yuv_type*srcY/2) * (int32_t)(width/2) + (int)(srcX/2)];
                    }
                }
            }


            /* Create one YUV file, that represent an image! */
            if(ComponentsStructure_CombineYUVComponents(*YUV_out, Y_out, U_out, V_out,
                                                        width, height, img_size, yuv_type))
            {
                printf(COMBINE_YUV_COMPONENTS_ERROR_MESSAGE);
                status = LDC_STATUS_ERROR;
            }
        }

        /* Free allocated memory! */
        free(Y_out);
        free(U_out);
        free(V_out);

        for(i = 0;i < height;i++)
        {
            free(h_p[i]);
            free(v_p[i]);
            free(h_d[i]);
            free(v_d[i]);
        }

        free(h_p);
        free(v_p);
        free(h_d);
        free(v_d);
    }

    return status;
 }


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
                                                  YUV_Type yuv_type)
{

    *Y = malloc(width*height                                   * sizeof ( uint8_t ));
    *U = malloc((width / 2) * ((int32_t)yuv_type*height / 2) * sizeof ( uint8_t ));
    *V = malloc((width / 2) * ((int32_t)yuv_type*height / 2) * sizeof ( uint8_t ));
    LDC_Status status = LDC_STATUS_OK; /* EXIT Status. */

    if(NULL == YUV || NULL == *U || NULL == *V || NULL == *Y)
    {
        status = LDC_STATUS_ERROR;
    }
    else
    {
        /*   YUV420 nv12   , has 12bpp, and U V components are INTERLEAVED!
             YUV422i UYVY , has 16bpp, and Y U V components are INTERLEAVED!
        */
        uint32_t i;
        uint32_t j;
        uint32_t k;

        if(YUV420_NV12 == yuv_type)
        {
            /* Create Y component! */
            memcpy(*Y, YUV, width*height);

            for(i = 0, j = 0;i < width * height / 2;i+=2)
            {
                (*U)[j]   = YUV[width * height + i];
                (*V)[j++] = YUV[width * height + i + 1];
            }
        }
        else if(YUV422I_UYVY == yuv_type)
        {
            for(i = 0, j = 0,k = 0;i < img_size;i+=2)
            {
                (*Y)[j++]   = YUV[i + 1];

                if(!(i % 4))
                {
                    (*U)[k]     = YUV[i];
                    (*V)[k++]   = YUV[i + 2];
                }
            }

        }
        else
        {
            status =  LDC_STATUS_ERROR;
        }

    }

    return status;
}


/**
 ***************************************************************************************************
 *
 * \fn     ComponentsStructure_CombineYUVComponents
 *
 * \brief  Combine Y, U, V components to one output YUV image.
 *
 * \param  [Out] YUV_out      Output YUV image.
 * \param  [In]  Y            Y component of output YUV image.
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
                                                    uint32_t img_size, YUV_Type yuv_type)
{
    LDC_Status status = LDC_STATUS_OK; /* EXIT Status. */

    if(NULL == YUV_out || NULL == U || NULL == V|| NULL == Y)
    {
        status = LDC_STATUS_ERROR;
    }
    else
    {

        /* Create one YUV frame, with Y, U and V components. */
        uint32_t i;
        uint32_t j;
        uint32_t k;

        if(YUV420_NV12 == yuv_type)
        {
            memcpy(YUV_out, Y, width*height);

            for(i = 0, j = 0;i < width * height / 2;i+=2)
            {
                YUV_out[width * height + i]      = U[j];
                YUV_out[width * height + i + 1]  = V[j++];
            }
        }
        else if(YUV422I_UYVY == yuv_type)
        {
            for(i = 0, j = 0, k = 0;i < img_size;i+=2)
            {
                YUV_out[i+1] = Y[j++];

                if(!(i % 4))
                {
                    YUV_out[i] = U[k];
                    YUV_out[i + 2] = V[k++];
                }
            }

        }
        else
        {
            status = LDC_STATUS_ERROR;
        }

    }

    return status;
}

