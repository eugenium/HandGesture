function a = find_match(maps,num_gest,N,img)

num_pix=size(N,2);
img=squeeze(img(1,1,:));
img=img';
img_size=size(img,2);
%distances=zeros(num_gest);
a=zeros(1,2);
a=double(a);
half=ones(1,img_size).*(.5);
for i=1:num_gest
    
    tmp=(4/num_pix)*sum((maps(i,N)-half(1,N)).*(img(1,N)-half(1,N)));
    %fprintf(1,'i=%d tmp=%f\n',i,tmp);
    if(tmp>a(1,2))
        %fprintf(1,'a better match is %d with val %f\n',i,tmp);
        a(1,2)=tmp;
        a(1,1)=i;
    end
end

a(1,2)=1-a(1,2);