%obtains the FD using a N point FFT 
function [z]=obtainU(name)
    temp = imread(name);
    temp(1,1)=0;
    temp = temp./255;
    temp = double(temp);
    block=ones(3);
    temp=imdilate(temp,ones(12)); % the higher the dilation the more the
    %contour becomes messed up but a certain dilation is needed to remove
    %the inside
    temp2=temp-imerode(temp,block);
    %clean up the inside of the contour so its just one contour
    [x,y]=ind2sub(size(temp2),find(temp2>0));
    z=x+y*1i;
end