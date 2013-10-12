function [N,p] = pseudo_loss(maps, num_gest, img_size)

p=zeros(img_size,2);
tmp=0;
% for each pixel 
for i=1:img_size
    p(i,1)=i;
    tmp=0;
    for j=1:num_gest
        t=maps(j,i)-.5;
        tmp = tmp+t;
        p(i,2) = p(i,2) + abs(t);
    end
    p(i,2) = abs(tmp)-p(i,2);
end

[p(:,2),I] = sort(p(:,2));
%p(:,2) = aa;
p(:,1) = I;

% N=p(1:1200,1)';





processed = zeros(img_size,1);
num_pix=0;
num_rem=img_size;
i=1;
%pick the best images
while ((i<=img_size)&&(num_rem > 0))
    if(processed(i)==0)
        num_pix=num_pix+1;
        processed(i)=1;
        num_rem=num_rem-1;
        N(num_pix)=p(i,1);
        for j=(i+1):img_size
            if((processed(j)==0)&&(d_split(maps,num_gest,p(i,1),p(j,1)) < 2)) % was < .000001
                processed(j)=1;
                num_rem = num_rem-1;
            end
        end
       % i=i+1;
    end
    i=i+1;
%     else
%         i=i+1;
%         while( (i<=img_size)&&(processed(i)==1))
%             i=i+1;
%         end
%     end
   % i=i+1;   
end


%% old version which was not working properly
% processed = zeros(img_size,1);
% num_pix=0;
% 
% % pick the best images
% for i=1:img_size
%     if(processed(i)==0)
%         num_pix=num_pix+1;
%         processed(i)=1;
%         N(num_pix)=p(i,1);
%         for j=(i+1):img_size
%             if(d_split(maps,num_gest,i,j) < .001 ) % was < 2
%                 processed(j)=1;
%             end
%         end
%     else
%         i=i+1;
%         while( (i<=img_size)&&(processed(i)==1))
%             i=i+1;
%         end
%     end
%         
% end