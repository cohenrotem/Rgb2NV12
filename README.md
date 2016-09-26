# Rgb2NV12
Convert RGB to NV12 color space, in Matlab, IPP, Plain C, and SSE optimized code.

NV12 format definition: http://www.fourcc.org/yuv.php#NV12

NV12 format is YUV 4:2:0 format ordered in memory with a Y plane first, followed by packed chroma samples in interleaved UV plane.
Example:  

YYYYYY  
YYYYYY  
UVUVUV  
http://i.stack.imgur.com/XboNY.png

RGB color format, refers to Pixel-Order RGB (byte per pixel, lower byte is Red):
RGBRGBRGBRGBRGB
RGBRGBRGBRGBRGB
http://i.stack.imgur.com/y0ggj.png

Better description can be found here: http://stackoverflow.com/questions/37768751/how-to-convert-rgb-to-nv12-color-space-using-ipp

I did some Web research, and found out that regarding NV12, YUV is defined as YCbCr color space. There are currently at least 2   possible YCbCr formats apply NV12:  

- BT.601 - Applies SDTV: https://en.wikipedia.org/wiki/Rec._601  
- BT.709 - Applies HDTV: https://en.wikipedia.org/wiki/Rec._709  

I found out that IPP function for converting RGB to NV12 exists (applies BT.601):  
ippiRGBToYCbCr420_8u_C3P2R  
https://software.intel.com/en-us/node/503959  

Although IPP can be used, I have decided to implement optimized code using SSE intrinsics.  
The IPP function is limited for converting RGB and to BT.601 standard.  

The advantege of my SSE optimized code, is that it can easily be modified for supporting:  
BGR to NV12 conversion.  
Supporting BT.709 standard.  
Can be incorporated into existing image filter, and do the processing in a single pass to improve performance.  

For testing the implementation, I compared the result of with Matalb reference (I used my own Matalb implementation as reference).  
Testing istructions:  
1. Execute BuildInput.m in Matlab to create input image.  
2. Execute Rgb2NV12 in Visual Studio 2010 (select x64 configuration).
3. Execute TestOutput.m in Matlab.  

Performance:  
When compiling the code using Intel Compiler 15.0, the performance of my optimized code was about the same as IPP performance.  
