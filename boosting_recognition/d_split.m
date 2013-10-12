function a = d_split(maps,num_gest,i,j)

a=0;
for k=1:num_gest
    a = a + abs(maps(k,i)-maps(k,j));
end