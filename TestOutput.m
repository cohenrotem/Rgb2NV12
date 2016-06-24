%Compare Rgb2NV12 Matalb result to C result

close all
clc

I = imread('peppers.bmp');
%I = imread('123.bmp');

[srcM, srcN, ~] = size(I);

f = fopen('NV12.bin', 'r');Jc = fread(f, [srcN, srcM*3/2], 'uint8');fclose(f);Jc = Jc';

Jmat = rgb2nv12(double(I));


Jdiff = Jmat - Jc;
maxJdiff = max(abs(Jdiff(:)));

disp(['maxJdiff = ', num2str(maxJdiff)]);

if (maxJdiff > 1)
    figure;imagesc(Jmat);title('Jmat');impixelinfo;
    figure;imagesc(Jc);title('Jc');impixelinfo;
end

if (maxJdiff > 0)
    figure;imagesc(Jdiff);title('Jdiff');impixelinfo;
end
