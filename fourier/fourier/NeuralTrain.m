%Training
%for each class
    %compute the FD for the training images in each class
    %compute the mahalonbis distance for the class
folder_name='..\..\TrainingData';

file_list=dir(folder_name);
cd(folder_name);
N=64;
num_train=80;
num_gest=46;

Feature_vectors=zeros(num_train*num_gest,N-2);
Labels=zeros(num_train*num_gest,num_gest);
pos=1;
for i=1:num_gest;
   out=zeros(1,num_gest);
   out(i)=1;
   for j=1:num_train
       U = obtainU(file_list((i-1)*num_train+2+j).name);
       F=extractFD(U,N);
       
       F=normaliseFD(F);
       Feature_vectors(pos,:)=F(3:end)';
       Labels(pos,:)=out;
       pos=pos+1;
   end
end

Feature_vectors=Feature_vectors';
Labels=Labels';

net1=newfit(Feature_vectors,Labels,[20]);
net1.trainFcn='trainbfg';
[net1]=train(net1,Feature_vectors,Labels);
save net1;