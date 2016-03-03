exe = '..\..\..\Build\Debug\driver.exe';
filePath = '../../../Build/pointsxy2.txt';
%-s 0.1 -i ../../../Build/pointsxy.txt -o test.txt -w 0 -b 2

fid = fopen(filePath);
C = textscan(fid, '%f\t%f'); x = C{1}; y = C{2};
fclose(fid);

%x = x - x(1);

cutoffs = 0; %[0 : 2 : 100];
bcs = [2];

step = 0.01;

path1 = getenv('PATH');

for (c = 1 : length(cutoffs))
    for (b = 1 : length(bcs))
        theTitle = sprintf('Trying wavelength=%d  bc=%d', cutoffs(c), bcs(b));
        fprintf([theTitle '\n']);
        cmd = sprintf('"%s" -s %f -w %d -b %d -i %s -o test.txt', exe, step, cutoffs(c), bcs(b), filePath);
        dos(cmd);
        
        fid = fopen('test.txt');
        Ctext = textscan(fid, '%s', 4, 'Delimiter', ',');
        Cdata = textscan(fid, '%f, %f, %f, %f');
        fclose(fid);
        delete('test.txt');
        figure;
        plot(x, y, 'k');
        hold on;
        plot(Cdata{1}, Cdata{2}, 'r');
        %xlim([0 30]);
        %xlim([370 400]);
        %ylim([370 400]);
        title(theTitle);
    end
end

