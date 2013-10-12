Itest = zeros(9);
Itest(1:3, 6:9) = 1;
Itest(4:6, 4:7) = 1;
Itest(7:9, 1:4) = 1;


my1 = [ 0 1 0;1 1 0; 0 0 0];
my2 = [ 0 0 0; 0 1 1; 0 1 0];
my3 = [ 1 1 0; 1 1 0; 0 0 0];
figure('Name', 'Three asymmetric structuring elements');
subplot(1, 3, 1), imshow(my1, 'notruesize'), title('Element 1');
subplot(1, 3, 2), imshow(my2, 'notruesize'), title('Element 2');
subplot(1, 3, 3), imshow(my3, 'notruesize'), title('Element 3');

myst1 = strel(my1);
myst2 = strel(my2);
myst3 = strel(my3);
Itesterode1 = imerode(Itest,myst1);
Itesterode2 = imerode(Itest,myst2);
Itesterode3 = imerode(Itest,myst3);
figure('Name', 'Erosion using default (same) boundary handling');
subplot(2, 2, 1), imshow(Itest, 'notruesize'), title('Original');
subplot(2, 2, 2), imshow(Itesterode1, 'notruesize'), title('Element 1');
subplot(2, 2, 3), imshow(Itesterode2, 'notruesize'); title('Element 2');
subplot(2, 2, 4), imshow(Itesterode3, 'notruesize'); title('Element 3');

Itesterode1f = imerode(Itest,myst1, 'full');
Itesterode2f = imerode(Itest,myst2, 'full');
Itesterode3f = imerode(Itest,myst3, 'full');
figure('Name', 'Erosion using full boundary handling');
subplot(2, 2, 1), imshow(Itest, 'notruesize'), title('Original');
subplot(2, 2, 2), imshow(Itesterode1f, 'notruesize'), title('Element 1');
subplot(2, 2, 3), imshow(Itesterode2f, 'notruesize'); title('Element 2');
subplot(2, 2, 4), imshow(Itesterode3f, 'notruesize'); title('Element 3');

%Padding to get the traditional erosion
Itestpad1 = padarray(Itest, [3, 3], 0, 'both');
Itestpad2 = padarray(uint8(255*double(Itest)), [3, 3], 128, 'both');
Itestpad1erode = imerode(Itestpad1, myst1);
Itestpad2erode = imerode(Itestpad2, myst1);

figure('Name', 'Erosion with padding');
subplot(2,2,1), imshow(Itestpad1), title('Original binary with 0 pad');
subplot(2,2,2), imshow(Itestpad2), title('Original uint8 with 128 pad');
subplot(2,2,3), imshow(Itestpad1erode), title('Eroded with 0 pad');
subplot(2,2,4), imshow(Itestpad2erode), title('Eroded with 128 pad');


Itestpad1eu = Itestpad1erode(4:12, 4:12);

figure('Name', 'Erosion - comparison of boundary');
subplot(2,2,1), imshow(Itest), title('Original ');
subplot(2,2,2), imshow(Itestpad1eu), title('Eroded with 0 pad');
subplot(2,2,3), imshow(Itesterode1f), title('Eroded using full option');
subplot(2,2,4), imshow(Itesterode1), title('Eroded using same (default) option');