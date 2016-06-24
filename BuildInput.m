%Build input image for testing Rgb2NV12

%Load peppers.png, and save it as peppers.bmp
RGB = imread('peppers.png');
imwrite(RGB, 'peppers.bmp');
figure;imshow(RGB);title('RGB')

% % Build all combinations of R, G, B triples:
% [R, G] = meshgrid(0:255);
% R = uint8(R);
% G = uint8(G);
% 
% R = repmat(R, 16, 16);
% G = repmat(G, 16, 16);
% RGB = cat(3, R, G, zeros(size(R), 'uint8'));
% 
% z = 0;
% for y = 0:15
%     for x = 0:15
%         B = uint8(zeros(256, 256) + z);
%         RGB(1+y*256:(y+1)*256, 1+x*256:(x+1)*256, 3) = B;
%         z = z + 1;
%     end
% end
% 
% imwrite(RGB, '123.bmp');
% 
% figure;imshow(RGB);title('RGB');impixelinfo

