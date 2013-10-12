function a=aff(x,y)

% [n,m,num_pix]=size(x);
% half=ones(num_pix,1).*(.5);
% a=(4/num_pix)*sum((squeeze(x(1,1,:))-half).*(squeeze(y(1,1,:))-half));

[n,num_pix]=size(x);
half=ones(1,num_pix).*(.5);
a=(4/num_pix)*sum((x(1,:)-half).*(y(1,:)-half));