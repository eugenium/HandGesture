%Training
%for each class
    %compute the FD for the training images in each class
    %compute the mahalonbis distance for the class
folder_name='../../TrainingData';

file_list=dir(folder_name);
cd(folder_name);
% addpath('/home/eugene/hand_gest/fourier/fourier/');

% cd(folder_name);
% file_list=dir(folder_name);
N=64;
N2=10;
num_train=80;
num_gest=10;

Feature_vectors=zeros(num_train,N2-2);
Means=zeros(num_gest,N2-2);
Variances=cell(1,num_gest);
for i=[6,11,12,15,22,27,28,33,36,42]
   for j=1:num_train
       U = obtainU(file_list((i-1)*num_train+2+j).name);
       F=extractFD(U,N);
      % plotFD(F);
       F=normaliseFD(F);
       F=resizeFD(F,N2);
       Feature_vectors(j,:)=F(3:end)';
   end
   Variances{i}=cov(Feature_vectors)
   Means(i,:)=mean(Feature_vectors,1);
   %plotFD([0 1 Means(i,:)]');   
   dlmwrite(strcat('/home/eugene/hand_gest/fourier/fourier/Variances',num2str(i),'.txt'),inv(Variances{i}),'delimiter',' ','precision',6);
end
dlmwrite('/home/eugene/hand_gest/fourier/fourier/Means.txt',Means,'delimiter',' ','precision',6);