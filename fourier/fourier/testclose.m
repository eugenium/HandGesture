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
Itestclose1 = imclose(Itest,myst1);
Itestclose2 = imclose(Itest,myst2);
Itestclose3 = imclose(Itest,myst3);
figure('Name', 'Close using default  boundary handling');
subplot(2, 2, 1), imshow(Itest, 'notruesize'), title('Original');
subplot(2, 2, 2), imshow(Itestclose1, 'notruesize'), title('Element 1');
subplot(2, 2, 3), imshow(Itestclose2, 'notruesize'); title('Element 2');
subplot(2, 2, 4), imshow(Itestclose3, 'notruesize'); title('Element 3');


%Padding to get the traditional open
Itestpad1 = padarray(Itest, [3, 3], 0, 'both');
Itestpad2 = padarray(uint8(255*double(Itest)), [3, 3], 128, 'both');
Itestpad1close = imclose(Itestpad1, myst1);
Itestpad2close = imclose(Itestpad2, myst1);

figure('Name', 'Close with padding');
subplot(2,2,1), imshow(Itestpad1), title('Original binary with 0 pad');
subplot(2,2,2), imshow(Itestpad2), title('Original uint8 with 128 pad');
subplot(2,2,3), imshow(Itestpad1close), title('Closed with 0 pad');
subplot(2,2,4), imshow(Itestpad2close), title('Closed with 128 pad');


Itestpad1cu = Itestpad1close(4:12, 4:12);
Itestpad1dilate = imdilate(Itestpad1,myst1);
Itestpad1du = Itestpad1dilate(4:12, 4:12);


figure('Name', 'Close - comparison of boundary');
subplot(2,2,1), imshow(Itest), title('Original ');
subplot(2,2,2), imshow(Itestpad1cu), title('Closed with 0 pad');
subplot(2,2,3), imshow(Itestpad1du), title('Dilated with 0 pad');
subplot(2,2,4), imshow(Itestclose1), title('Closed using same (default) option');

my4 = ones(3);
myst4 = strel(my4);
Itestpad4close = imclose(Itestpad1, myst4);
Itestpad4cu = Itestpad4close(4:12, 4:12);


my5 = ones(5);
myst5 = strel(my5);
Itestpad5close = imclose(Itestpad1, myst5);
Itestpad5cu = Itestpad5close(4:12, 4:12);

figure('Name', 'Close comparison of different structuring elements (0 pad)');
subplot(2,2,1), imshow(Itest), title('Original ');
subplot(2,2,2), imshow(Itestpad1cu), title('Closed with 0 pad');
subplot(2,2,3), imshow(Itestpad4cu), title('Closed with 3x3');
subplot(2,2,4), imshow(Itestpad5cu), title('Closed using 5x5');




enam = imread('enamel.tif');
enamcomp = imcomplement(enam);
enamcpad = padarray(enamcomp, [5, 5], 0, 'both'); %padded bigger
enamcomp4close = imclose(enamcpad, myst4);
enamcomp5close = imclose(enamcpad, myst5);
enamcomp4cu = enamcomp4close(6:(end-5), 6:(end-5));
enamcomp5cu = enamcomp5close(6:(end-5), 6:(end-5));
my6 = ones(7);
myst6 = strel(my6);
enamcomp6close = imclose(enamcpad, myst6);
enamcomp6cu = enamcomp6close(6:(end-5), 6:(end-5));

figure('Name', 'Enamel');
subplot(2, 1, 1), imshow(enam), title('Original');
subplot(2, 1, 2), imshow(enamcomp), title('Complemented');


figure('Name', 'Enamel - Closed');
subplot(2, 2, 1), imshow(enamcomp), title('Complemented');
subplot(2, 2, 2), imshow(enamcomp4cu), title('Closed with 3x3');
subplot(2, 2, 3), imshow(enamcomp5cu), title('Closed with 5x5');
subplot(2, 2, 4), imshow(enamcomp6cu), title('Closed with 7x7');