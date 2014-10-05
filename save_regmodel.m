for k = 1:100
    fib = fopen(['ferns/ferns_level',num2str(k-1),'.txt'],'w');
    for n = 1:400
     fprintf(fib,'%d %d %1.4f ', regModel.regs(k).ftrPos.xs(n,:));
    end 
    fprintf(fib,'\n');
    for n = 1:50
        fernv = regModel.regs(k).regInfo{n}.ysFern;
        for nn = 1:32
            for nnn = 1:136
                fprintf(fib, '%1.5f ',fernv(nn,nnn));
            end 
            fprintf(fib,'\n');
        end 
        for d = 1:5
        fprintf(fib,'%d %d %1.4f ', [regModel.regs(k).regInfo{n}.fids(1:2,d)',regModel.regs(k).regInfo{n}.thrs(d)]);
%         fprintf(fib,'%1.4f %1.4f %1.4f %1.4f %1.4f\n', regModel.regs(k).regInfo{n}.fids(1,:));
%         fprintf(fib,'%1.4f %1.4f %1.4f %1.4f %1.4f\n', regModel.regs(k).regInfo{n}.fids(2,:));
        end 
    end 
    fclose(fib);
end 
