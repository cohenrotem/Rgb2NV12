// Test Rgb2NV12 DLL

#include "stdafx.h"
#include "Rgb2NV12.h"
#include "Rgb2NV12_useSSE.h"


static void Bgr2RgbAndReverseUpDown(const BYTE I[],
                                    const int image_width, 
                                    const int image_height,
                                    BYTE J[])
{
    int y, x;
    const BYTE *I0;
    BYTE *J0;

    for (y = 0; y < image_height; y++)
    {
        I0 = &I[y*image_width*3];
        J0 = &J[(image_height-1-y)*image_width*3];
        //memcpy(J0, I0, image_width*3);

        for (x = 0; x < image_width; x++)
        {
            J0[x*3 + 2] = I0[x*3 + 0];	//B
            J0[x*3 + 1] = I0[x*3 + 1];	//G
            J0[x*3 + 0] = I0[x*3 + 2];	//R
        }
    }
}



int _tmain(int argc, _TCHAR* argv[])
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    int i;
    LARGE_INTEGER frequency;
    LARGE_INTEGER t1, t2;
    double elapsedTime;
    QueryPerformanceFrequency(&frequency);


    IppStatus ipp_status = ippInit();

    if (ipp_status != ippStsNoErr)
    {
        return (int)ipp_status;
    }

    //Load bitmaps.
    ////////////////////////////////////////////////////////////////////////////
    // First look for the bitmap in the current directory
    const TCHAR input_image_file_name[] = TEXT("peppers.bmp");
    //const TCHAR input_image_file_name[] = TEXT("123.bmp");
    const TCHAR output_file_name[] = TEXT("NV12.bin");

    HANDLE hFile = CreateFile(input_image_file_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);	// Handle returned from CreateFile

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MessageBox(NULL, TEXT("Error"), TEXT("CreateFile input file error"), MB_ICONERROR | MB_OK);
        int last_error = GetLastError();fwprintf(stderr, L"last_error = %d\n", last_error);
    }

    DWORD dwFileSize = GetFileSize(hFile, NULL);
    if (dwFileSize == INVALID_FILE_SIZE)
    {
        MessageBox(NULL, TEXT("Error"), TEXT("GetFileSize error"), MB_ICONERROR | MB_OK);
        int last_error = GetLastError();fwprintf(stderr, L"last_error = %d\n", last_error);
    }

    BYTE *pFile = (BYTE*)_aligned_malloc(dwFileSize, 128);

    if (pFile == NULL)
    {
        OutputDebugString(TEXT("Could not allocate pFile\n"));
    }

    DWORD nBytesRead = 0;
    if (!ReadFile(hFile, pFile, dwFileSize, &nBytesRead, NULL))
    {
        OutputDebugString(TEXT("ReadFile failed\n"));
        int last_error = GetLastError();fwprintf(stderr, L"last_error = %d\n", last_error);
    }

    // Close and invalidate the file handle, since we have copied its bitmap data
    CloseHandle(hFile);


    int cbFileHeader = sizeof(BITMAPFILEHEADER);

    //Store the size of the BITMAPINFO 
    BITMAPFILEHEADER *pBm = (BITMAPFILEHEADER*)pFile;
    DWORD cbBitmapInfo = pBm->bfOffBits - cbFileHeader;	//Size of the bitmap header

    //Store a pointer to the BITMAPINFO
    BITMAPINFO *pBmi = (BITMAPINFO*)(pFile + cbFileHeader);	//Pointer to the bitmap header

    //Store a pointer to the starting address of the pixel bits
    BYTE *pImage = pFile + cbFileHeader + cbBitmapInfo;	//Points to pixel bits

    const int image_width = pBmi->bmiHeader.biWidth;
    const int image_height = pBmi->bmiHeader.biHeight;


    BYTE *I = (BYTE*)_aligned_malloc(image_width*image_height*3, 128);
    BYTE *J = (BYTE*)_aligned_malloc(image_width*image_height*3, 128);

    if ((I == NULL) || (J == NULL))
    {
        OutputDebugString(TEXT("Could not allocate I or J\n"));
    }


    Bgr2RgbAndReverseUpDown(pImage,
                            image_width, 
                            image_height,
                            I);

    printf("Rgb2NV12\n");
    for (i = 0; i < 11; i++)
    {
        QueryPerformanceCounter(&t1);

        Rgb2NV12(I,				//const unsigned char I[],
                 image_width,	//const int image_width, 
                 image_height,	//const int image_height,
                 J);			//unsigned char J[])

        QueryPerformanceCounter(&t2);elapsedTime = (double)(t2.QuadPart - t1.QuadPart) / (double)frequency.QuadPart;

        printf("elapsedTime = %.2f msec.\n", elapsedTime*1000.0);
    }


    printf("\nRgb2NV12_useSSE\n");
    for (i = 0; i < 11; i++)
    {
        QueryPerformanceCounter(&t1);

        Rgb2NV12_useSSE(I,				//const unsigned char I[],
                        image_width,	//const int image_width, 
                        image_height,	//const int image_height,
                        J);				//unsigned char J[])

        QueryPerformanceCounter(&t2);elapsedTime = (double)(t2.QuadPart - t1.QuadPart) / (double)frequency.QuadPart;

        printf("elapsedTime = %.2f msec.\n", elapsedTime*1000.0);
    }


    hFile = CreateFile(output_file_name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);	// Handle returned from CreateFile

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MessageBox(NULL, TEXT("Error"), TEXT("CreateFile output file error"), MB_ICONERROR | MB_OK);
        int last_error = GetLastError();fwprintf(stderr, L"last_error = %d\n", last_error);
    }

    DWORD nBytesWritten = 0;
    if (!WriteFile(hFile, J, image_width*image_height*3/2, &nBytesWritten, NULL))
    {
        OutputDebugString(TEXT("WriteFile failed\n"));
        int last_error = GetLastError();fwprintf(stderr, L"last_error = %d\n", last_error);
    }


    CloseHandle(hFile);

    _aligned_free(pFile);
    _aligned_free(I);
    _aligned_free(J);

    return 0;
}
