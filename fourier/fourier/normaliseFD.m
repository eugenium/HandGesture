%Eugene Belilovsky
function G=normaliseFD(F)
    G=F;
    G(1)=0; % put contour at position 0
    G=abs(G);
    G=G./G(2); % scale
end