close all;
hand=imread('eugene.jpg');
figure(4);
imshow(hand);
hand=rgb2hsl(hand);
skin=hand(400:600,600:750,:);
skin_r=double(skin(:,:,1));
skin_g=double(skin(:,:,2));
skin_b=double(skin(:,:,3));

background=hand(50:300,50:400,:);
background_r=double(background(:,:,1));
background_g=double(background(:,:,2));
background_b=double(background(:,:,3));

x_r=linspace(min([min(background_r(:)) min(skin_r(:))]),max([max(background_r(:)) max(skin_r(:))]),300);
x_g=linspace(min([min(background_g(:)) min(skin_g(:))]),max([max(background_g(:)) max(skin_g(:))]),300);
x_b=linspace(min([min(background_b(:)) min(skin_r(:))]),max([max(background_b(:)) max(skin_b(:))]),300);


figure(1);
subplot(2,3,1);

hist(background_r(:),x_r);
ylabel('Background');
subplot(2,3,2);
hist(background_g(:),x_g);
subplot(2,3,3);
hist(background_b(:),x_b);


subplot(2,3,4);

hist(skin_r(:),x_r);
ylabel('Skin');
subplot(2,3,5);
hist(skin_g(:),x_g);
subplot(2,3,6);
hist(skin_b(:),x_b);
%figure(3);
h_min=-0.5;
h_max=0.4;
s_min=0.25;
s_max=0.51;
l_min=0.28;
l_max=0.55;
%g_max=120;
%g_min=65;
%b_min=50;
%b_max=80;

hand_binarize=double(hand);
%hand_binarize=hand_binarize(:,:,2)<g_max & hand_binarize(:,:,3)<b_max & hand_binarize(:,:,2)>g_min & hand_binarize(:,:,3)>b_min; 
hand_binarize=hand_binarize(:,:,1)<h_max & hand_binarize(:,:,2)<s_max  & hand_binarize(:,:,1)>h_min & hand_binarize(:,:,2)>s_min ; 
figure;
colormap(gray);
imagesc(hand_binarize);


% h_min=-0.5;
% h_max=0.4;
% s_min=0.25;
% s_max=0.51;
% l_min=0.28;
% l_max=0.55;
% 
% for j=1:40
%     hand=handy4(:,:,:,j);
%     hand=rgb2hsl(hand);
%     hand_binarize=double(hand);
%     hand_binarize=hand_binarize(:,:,1)<h_max & hand_binarize(:,:,2)<s_max  & hand_binarize(:,:,1)>h_min & hand_binarize(:,:,2)>s_min ; 
%     colormap(gray);
%     imagesc(hand_binarize);
%     hand=rgb2hsl(hand);
%     F(j)=getframe(gcf);
%     pause(.0333)
% end
% movie(F,10);