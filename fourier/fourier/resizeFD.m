%Eugene Belilovsky
%Removes the low energy components of the fourier descriptors F until the
%new size n is reached
function G=resizeFD(F,n)
    N=length(F);
    remove=(N-n)/2;
    G=fftshift(F);
    G=G(remove+1:end-remove);
    G=ifftshift(G);
end
