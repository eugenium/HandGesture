%obtains the FD using a N point FFT 
function [z]=obtainU(name)
    temp = imread(name);
    temp = temp./255;
    temp = double(temp);
    block=ones(3);
    temp2=temp-imerode(temp,block);
    [x,y]=ind2sub(size(temp2),find(temp2>0));
    z=x+y*j;
end