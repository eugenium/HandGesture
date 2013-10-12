function plotFD(F)
    figure(1);
    scatter(real(ifft(F)),imag(ifft(F)));
end