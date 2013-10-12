function choice=testImage(filename,Variances,Means)
    num_gest=size(Means,1);
    choice=1;
    U=obtainU(filename);
    F=extractFD(U,64);
    %plotFD(F);
    F=normaliseFD(F);
    F=resizeFD(F,10);
    F=F(3:end);
    current_dist=9999999999999999;
    
    x=[6,11,12,15,22,27,28,33,36,42];
    for i=x %x(x~=14 & x~=31)
        F=F(:);
        mah_dist=(((F-Means(i,:)')')/Variances{i})*(F-Means(i,:)');
        if(mah_dist<current_dist)
            current_dist=mah_dist;
            choice=i;
        end
    end
end