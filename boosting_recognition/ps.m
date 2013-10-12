% clc;
% maps=coherence_map(Images,num_gest,num_train);
% clc

%[N,p] = pseudo_loss(maps,num_gest,Ysiz*Xsiz);
% a=find_match(maps,num_gest,N,Images(3,75,:));
% disp('The matching gesuture should be 1');
% disp('The matching gesuture is');
% disp(a); 
% %%
if( 0)
stats=zeros(1,num_gest);
for j=1:num_gest %num_gest
    Images=loadgest('TrainingData',j,num_train);
    for k=1:num_train
      aaa = find_match(maps,num_gest,N,Images(1,k,:));
      if(aaa(1,1)~=j)
          stats(j) = stats(j) +1;
          fprintf(1,'j=%d k=%d j''=%d d=%f\n',j,k,aaa(1,1),aaa(1,2));
      end
    end
end
disp(stats);
end
disp(1-sum(stats)/(num_gest*num_train));

if( 1)
   num_train=19;
stats=zeros(1,num_gest);
for j=1:num_gest %num_gest
    Images=loadgest('TestingData',j,num_train);
    for k=1:num_train
      aaa = find_match(maps,num_gest,N,Images(1,k,:));
      if(aaa(1,1)~=j)
          stats(j) = stats(j) +1;
          fprintf(1,'j=%d k=%d j''=%d d=%f\n',j,k,aaa(1,1),aaa(1,2));
      end
    end
end
disp(stats);
end
disp(1-sum(stats)/(num_gest*num_train));
%%
%  % find the distance
%  for j=1:1      %num_gest
%     for k=1:num_train   %num_train
%     %aff(squeeze(Images(j,k,:))',maps(j,:));
%     d=distance(squeeze(Images(j,k,:))',maps(j,:))
% %     if(d>0.05)
% %         fprintf(1,'j=%d k=%d d=%f\n',j,k,d);
% %     end
%     end
%  end

%% test the loading of the images
% fig_num=0;
% for j=1:num_gest
%     for k=80:80
%     fig_num=fig_num+1;
%     figure(fig_num);
%     imagesc(reshape(Images(k,:),Ysiz,Xsiz)); colormap(gray);    
%     end
% end

% fig_num=0;
% for j=[7 33];
%     fig_num=fig_num+1;
%     figure(fig_num);
%     imagesc(reshape(maps(j,:),Ysiz,Xsiz)); colormap(gray); 
%   
% end

if( 0 )
fig_num=0;
for j=1:num_gest;
    fig_num=fig_num+1;
    figure(fig_num);
    tmp = .5*ones(1,Ysiz*Xsiz);
    tmp(N)=maps(j,N);
% tmp = maps(j,:);
    imshow(reshape(tmp,Ysiz,Xsiz)); colormap(gray); 
  
end
end
