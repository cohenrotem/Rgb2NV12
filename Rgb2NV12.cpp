// Rgb2NV12.cpp
//
#include "stdafx.h"
#include "Rgb2NV12.h"


#if 0
//Pain C Code implementation:
//----------------------------

//https://software.intel.com/en-us/node/503873
//YCbCr Color Model:
//	The YCbCr color space is used for component digital video and was developed as part of the ITU-R BT.601 Recommendation. YCbCr is a scaled and offset version of the YUV color space.
//	The Intel IPP functions use the following basic equations [Jack01] to convert between R'G'B' in the range 0-255 and Y'Cb'Cr' (this notation means that all components are derived from gamma-corrected R'G'B'):
//	Y' = 0.257*R' + 0.504*G' + 0.098*B' + 16
//	Cb' = -0.148*R' - 0.291*G' + 0.439*B' + 128
//	Cr' = 0.439*R' - 0.368*G' - 0.071*B' + 128


//Y' = 0.257*R' + 0.504*G' + 0.098*B' + 16
static __inline float Rgb2Y(float r0, float g0, float b0)
{
    float y0 = 0.257f*r0 + 0.504f*g0 + 0.098f*b0 + 16.0f;
    return y0;
}

//U equals Cb'
//Cb' = -0.148*R' - 0.291*G' + 0.439*B' + 128
static __inline float Rgb2U(float r0, float g0, float b0)
{
    float u0 = -0.148f*r0 - 0.291f*g0 + 0.439f*b0 + 128.0f;
    return u0;
}

//V equals Cr'
//Cr' = 0.439*R' - 0.368*G' - 0.071*B' + 128
static __inline float Rgb2V(float r0, float g0, float b0)
{
    float v0 = 0.439f*r0 - 0.368f*g0 - 0.071f*b0 + 128.0f;
    return v0;
}

//Convert two rows from RGB to two Y rows, and one row of interleaved U,V.
//I0 and I1 points two sequential source rows.
//I0 -> rgbrgbrgbrgbrgbrgb...
//I1 -> rgbrgbrgbrgbrgbrgb...
//Y0 and Y1 points two sequential destination rows of Y plane.
//Y0 -> yyyyyy
//Y1 -> yyyyyy
//UV0 points destination rows of interleaved UV plane.
//UV0 -> uvuvuv
static void Rgb2NV12TwoRows(const unsigned char I0[],
                            const unsigned char I1[],
                            const int image_width,
                            unsigned char Y0[],
                            unsigned char Y1[],
                            unsigned char UV0[])
{
    int x;  //Column index

    //Process 4 source pixels per iteration (2 pixels of row I0 and 2 pixels of row I1).
    for (x = 0; x < image_width; x += 2)
    {
        //Load R,G,B elements from first row (and convert to float).
        float r00 = (float)I0[x*3 + 0];
        float g00 = (float)I0[x*3 + 1];
        float b00 = (float)I0[x*3 + 2];

        //Load next R,G,B elements from first row (and convert to float).
        float r01 = (float)I0[x*3 + 3];
        float g01 = (float)I0[x*3 + 4];
        float b01 = (float)I0[x*3 + 5];

        //Load R,G,B elements from second row (and convert to float).
        float r10 = (float)I1[x*3 + 0];
        float g10 = (float)I1[x*3 + 1];
        float b10 = (float)I1[x*3 + 2];

        //Load next R,G,B elements from second row (and convert to float).
        float r11 = (float)I1[x*3 + 3];
        float g11 = (float)I1[x*3 + 4];
        float b11 = (float)I1[x*3 + 5];

        //Calculate 4 Y elements.
        float y00 = Rgb2Y(r00, g00, b00);
        float y01 = Rgb2Y(r01, g01, b01);
        float y10 = Rgb2Y(r10, g10, b10);
        float y11 = Rgb2Y(r11, g11, b11);

        //Calculate 4 U elements.
        float u00 = Rgb2U(r00, g00, b00);
        float u01 = Rgb2U(r01, g01, b01);
        float u10 = Rgb2U(r10, g10, b10);
        float u11 = Rgb2U(r11, g11, b11);

        //Calculate 4 V elements.
        float v00 = Rgb2V(r00, g00, b00);
        float v01 = Rgb2V(r01, g01, b01);
        float v10 = Rgb2V(r10, g10, b10);
        float v11 = Rgb2V(r11, g11, b11);

        //Calculate destination U element: average of 2x2 "original" U elements.
        float u0 = (u00 + u01 + u10 + u11)*0.25f;

        //Calculate destination V element: average of 2x2 "original" V elements.
        float v0 = (v00 + v01 + v10 + v11)*0.25f;

        //Store 4 Y elements (two in first row and two in second row).
        Y0[x + 0]	= (unsigned char)(y00 + 0.5f);
        Y0[x + 1]	= (unsigned char)(y01 + 0.5f);
        Y1[x + 0]	= (unsigned char)(y10 + 0.5f);
        Y1[x + 1]	= (unsigned char)(y11 + 0.5f);

        //Store destination U element.
        UV0[x + 0]	= (unsigned char)(u0 + 0.5f);

        //Store destination V element (next to stored U element).
        UV0[x + 1]	= (unsigned char)(v0 + 0.5f);
    }
}


//Convert image I from pixel ordered RGB to NV12 format.
//I - Input image in pixel ordered RGB format
//image_width - Number of columns of I
//image_height - Number of rows of I
//J - Destination "image" in NV12 format.

//I is pixel ordered RGB color format (size in bytes is image_width*image_height*3):
//RGBRGBRGBRGBRGBRGB
//RGBRGBRGBRGBRGBRGB
//RGBRGBRGBRGBRGBRGB
//RGBRGBRGBRGBRGBRGB
//
//J is in NV12 format (size in bytes is image_width*image_height*3/2):
//YYYYYY
//YYYYYY
//UVUVUV
//Each element of destination U is average of 2x2 "original" U elements
//Each element of destination V is average of 2x2 "original" V elements
//
//Limitations:
//1. image_width must be a multiple of 2.
//2. image_height must be a multiple of 2.
//3. I and J must be two separate arrays (in place computation is not supported). 
void Rgb2NV12(const unsigned char I[],
              const int image_width, 
              const int image_height,
              unsigned char J[])
{
    //In NV12 format, UV plane starts below Y plane.
    unsigned char *UV = &J[image_width*image_height];

    //I0 and I1 points two sequential source rows.
    const unsigned char *I0;  //I0 -> rgbrgbrgbrgbrgbrgb...
    const unsigned char *I1;  //I1 -> rgbrgbrgbrgbrgbrgb...

    //Y0 and Y1 points two sequential destination rows of Y plane.
    unsigned char *Y0;	//Y0 -> yyyyyy
    unsigned char *Y1;	//Y1 -> yyyyyy

    //UV0 points destination rows of interleaved UV plane.
    unsigned char *UV0; //UV0 -> uvuvuv

    int y;  //Row index

    //In each iteration: process two rows of Y plane, and one row of interleaved UV plane.
    for (y = 0; y < image_height; y += 2)
    {
        I0 = &I[y*image_width*3];		//Input row width is image_width*3 bytes (each pixel is R,G,B).
        I1 = &I[(y+1)*image_width*3];

        Y0 = &J[y*image_width];			//Output Y row width is image_width bytes (one Y element per pixel).
        Y1 = &J[(y+1)*image_width];

        UV0 = &UV[(y/2)*image_width];	//Output UV row - width is same as Y row width.

        //Process two source rows into: Two Y destination row, and one destination interleaved U,V row.
        Rgb2NV12TwoRows(I0,
                        I1,
                        image_width,
                        Y0,
                        Y1,
                        UV0);
    }
}



#else


void Rgb2NV12(const unsigned char I[],
              const int image_width, 
              const int image_height,
              unsigned char J[])
{
    //memcpy(J, I, image_width*image_height*3);

    IppStatus ipp_status;

    int srcStep = image_width*3;
    int dstYStep = image_width;
    int dstCbCrStep = image_width;
    IppiSize roiSize = {image_width, image_height};

    const Ipp8u* pSrc = (Ipp8u*)I;

    Ipp8u *pDstY	= (Ipp8u*)J;							//Y color plane is the first image_width*image_height pixels of J.
    Ipp8u *pDstCbCr	= (Ipp8u*)&J[image_width*image_height];	//In NV12 format, UV plane starts below Y.

    ipp_status = ippiRGBToYCbCr420_8u_C3P2R(pSrc, srcStep, pDstY, dstYStep, pDstCbCr, dstCbCrStep, roiSize);

    if (ipp_status != ippStsNoErr)
    {
        memset(J, 128, image_width*image_height*3/2);
    }
}
#endif
