% George Todorov 
%
%%
clear;
clc;
num_gest=46;
num_train=80;
Ysiz = 240;
Xsiz = 320;
%% Loads all the images into the 'Images' vector; each
% image is a row.

% [Images, Ysiz, Xsiz] = loadimages('TrainingData',num_gest,num_train);
% % test the loading of the images
% fig_num=0;
% for j=1:num_gest
%     for k=1:num_train
%     fig_num=fig_num+1;
%     figure(fig_num);
%     imagesc(reshape(Images(j,k,:),Ysiz,Xsiz)); colormap(gray);    
%     end
% endtt
% disp('Training');
% tic
% maps=coherence_map(Images,num_gest,num_train);
%% 

% % testing affinity function
% [n,num_pix]=size(maps(1,:));
% half=ones(1,num_pix).*(.5);
% tmp=squeeze(maps(1,:));
% size(tmp)
% tmp=squeeze(Images(1,1,:))';
% size(tmp)
% size(half)
% a=(4/num_pix)*sum((maps(1,:)-half).*(tmp(1,:)-half));


% for j=1:num_gest
%     for k=1:num_train
%     %aff(squeeze(Images(j,k,:))',maps(j,:));
%     distance(squeeze(Images(j,k,:))',maps(j,:))
%     end
% end
tic;
maps=co_map(num_gest,num_train);
[N,p] = pseudo_loss(maps,num_gest,Ysiz*Xsiz);
b = toc;
disp('Total time to train: '); disp(b);
