%obtains the FD using a N point FFT 
function [F]=extractFD(U,N)
    %sort based on angle
    s=length(U);
    z_mean=mean(real(U))+mean(imag(U))*i;
    ang=angle(z_mean-U);
    [b ix]=sort(ang);
    U=U(ix);
    %U=U(round(linspace(1,s,N)));%aliasing issues %possible improvement of this decimation procedure
    F=fft(U);
end