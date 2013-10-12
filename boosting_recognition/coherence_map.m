function maps=coherence_map(Images,num_gest,num_train)

% how close the images should be
min_aff=.95;
%max number of centers
centers=num_train;
[m,n,num_pix]=size(Images(1,1,:));
% the map which will be returned
maps=double(zeros(num_gest,num_pix));
% set of cluster centers
center=zeros(num_gest,centers,num_pix);
%keep track which images we have added to the centers
processed=zeros(num_gest,num_train);

for k= 1:num_gest
    num_rem=num_train;
    num_maps=0;
    while (num_rem > 0)
        for j=1:num_train
            if(processed(k,j)==0)
                processed(k,j)=1;
                num_rem=num_rem-1;
                num_maps=num_maps+1;
                center(k,num_maps,:)=Images(k,j,:);
                for l=(j+1):num_train
                    af=aff(squeeze(Images(k,j,:))',squeeze(Images(k,l,:))');
                    if ((processed(k,l)==0)&&(af>min_aff))
                        %fprintf('k=%d j=%d l=%d af=%f\n',k,j,l,af);
                        processed(k,l)=1;
                        num_rem=num_rem-1;
                    end
                end
            end
        end
    end
    maps(k,:)=mean(center(k,1:num_maps,:),2);
end
