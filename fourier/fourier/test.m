%train
folder_name='../../TestinSData';
file_list=dir(folder_name);
cd(folder_name);
num_test=19;
num_gest=10;
acc=zeros(1,num_gest);
chosen=zeros(1,num_gest);

correct=0;
p=[6,11,12,15,22,27,28,33,36,42]
for   i=1:10
   for j=1:num_test
        filename=file_list((i-1)*num_test+2+j).name;
        ges=testImage(filename,Variances,Means);
        if(ges~=p(i))
            acc(i)=acc(i)+1;
        else
            correct=correct+1;
        end
        
     %   chosen(ges)=chosen(ges)+1;
   end
end
acc
correct/(num_test*num_gest)
