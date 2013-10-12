function Images = loadgest(folder_name,gest_num,num_training)


%%
 % This functions assumes there exist a folder folder_name in which
 % all the images that are going to describe the space of work
 % are stored; (Ignores the 'Thumbs.db' file that exists on windows
 % machines' )

%%

% Images(num_gestures, num_training,image data)

file_list = ls(folder_name);
[m,n] = size(file_list);
cd(folder_name);
temp = imread(file_list(5,:));
[Ysiz, Xsiz ] = size(temp);

Images=zeros(1, num_training,Ysiz*Xsiz);
if( m > 2)
%     tic 
    if(m > 3) 
            for j= 1:num_training
                temp = imread(file_list((gest_num-1)*num_training+2+j,:));
                temp = temp./255;
                temp = double(temp);
                %temp = one - temp; % added to invert pixel values
                Images(1,j,:) = reshape(temp,Ysiz*Xsiz,1);
                %Images(k,j,1) = 0;
            end

    else disp('You have no images in the image folder');     
    end
%     b = toc;
%     disp('Total time to load images: '); disp(b);
else
    disp('You have no files in the folder!')
end
cd('..');