function NV12 = rgb2nv12(RGB)
%Convert RGB image color format to YUV 420 NV12 matrix format
%RGB dimension must be event in both axes.
%
%NV12 format description:
%1. Convert RGB to YUV.
%2. Sub sample (use binning) U and V color panes by factor of x2 in each
%   axis.
%3. NV12 Pixel order is:
%   YYYYYYYYYY...
%   YYYYYYYYYY...
%   YYYYYYYYYY...
%   YYYYYYYYYY...
%   ...
%   UVUVUVUVUV...
%   UVUVUVUVUV...
%
%Example:
%RGB = imread('peppers.png');
%NV12 = rgb2nv12(RGB);

%Keep input class.
classRGBin = class(RGB);

%Convert to double to keep accuracy.
RGB = double(RGB);

%Convert RGB to YUV without clamping.
%YUV = rgb2yuv(RGB, false);

%YUV = rgb2yuv601(RGB, false);
YUV = rgb2ycbcr(RGB/255)*255;
%YUV = rgb2ycbcr(uint8(RGB));

Y = double(YUV(:, :, 1));
U = double(YUV(:, :, 2));
V = double(YUV(:, :, 3));

%Shrink U and V, by averaging each 2x2 pixels.
%h = ones(3)/sum(ones(3));
% h = [1 2 1]'*[1 2 1];
% h = h / sum(h);
% U = imfilter(U, h, 'symmetric');
% V = imfilter(V, h, 'symmetric');

U = (U(1:2:end, 1:2:end) + U(2:2:end, 1:2:end) + U(1:2:end, 2:2:end) + U(2:2:end, 2:2:end))/4;
V = (V(1:2:end, 1:2:end) + V(2:2:end, 1:2:end) + V(1:2:end, 2:2:end) + V(2:2:end, 2:2:end))/4;

% U = imresize(U, size(U)/2, 'bilinear', 'Antialiasing', false);
% V = imresize(V, size(V)/2, 'bilinear', 'Antialiasing', false);

% U = (U(1:2:end, 1:2:end)*18 + U(2:2:end, 1:2:end)*14 + U(1:2:end, 2:2:end)*18 + U(2:2:end, 2:2:end)*14)/4/16;
% V = (V(1:2:end, 1:2:end)*18 + V(2:2:end, 1:2:end)*14 + V(1:2:end, 2:2:end)*18 + V(2:2:end, 2:2:end)*14)/4/16;

UV = zeros(size(U,1), size(Y,2)); %Half height and same width as RGB.

%Interweave U and V: UVUVUVUVUV...
UV(:, 1:2:end) = U;
UV(:, 2:2:end) = V;

%Limit to valid pixel range:
Y = max(min(Y, 235), 16);
UV = max(min(UV, 240), 16);

NV12 = [Y; UV];

%Keep original 
NV12 = cast(NV12, classRGBin);
