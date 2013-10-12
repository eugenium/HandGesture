function maps=co_map(num_gest,num_train)

% how close the images should be
min_aff=.95;
%max number of centers
centers=num_train;
Images=loadgest('TrainingData',1,num_train);
[m,n,num_pix]=size(Images(1,1,:));

% the map which will be returned
maps=double(zeros(num_gest,num_pix));
% set of cluster centers
center=zeros(centers,num_pix);
%keep track which images we have added to the centers


for k= 1:num_gest
    Images=loadgest('TrainingData',k,num_train);
    num_rem=num_train;
    num_maps=0;
    processed=zeros(num_train);
    while (num_rem > 0)
        for j=1:num_train
            if(processed(j)==0)
                processed(j)=1;
                num_rem=num_rem-1;
                num_maps=num_maps+1;
                center(num_maps,:)=Images(1,j,:);
                for l=(j+1):num_train
                    af=aff(squeeze(Images(1,j,:))',squeeze(Images(1,l,:))');
                    if ((processed(l)==0)&&(af>min_aff))
                        %fprintf('k=%d j=%d l=%d af=%f\n',k,j,l,af);
                        processed(l)=1;
                        num_rem=num_rem-1;
                    end
                end
            end
        end
    end
    maps(k,:)=mean(center(1:num_maps,:),1);
end
