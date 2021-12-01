
%readme
%change logReads to read for the amount of time the arduino waits at a
%position
%the arduino controls the timing of the matlab script through the serial
%port. If a "0" line is read, matlab waits until a line starting with s1 is
%read - that means the arduino is at a new position and matlab should start
%listening to the IP port. 
clear all;
instrument = 'i4'; %Manually fill in name of instrument
stepper = serialport('COM11', 9600); %Manually put in the COM port for the stepper arduino
multiplexer = serialport('COM12', 115200); %Manually put in the COM port for the multiplexer arduino
bRead = '';
flush(stepper);
flush(multiplexer);
configureTerminator(stepper,"CR")
configureTerminator(multiplexer,"CR")
spDi = 21.5; %spool diameter in mm
z_off = 112; %height difference between tag center and 3-string-junction
z_off_set = 10; %tag started 18 mm off the base + 9mm offset
y_off = 0;
x_off = 0;
d1Initial = 1066; %initial length of string 1 w/ instrument at origin in mm
d2Initial = 1074; %initial length of string 2 w/ instrument at origin in mm
d3Initial = 1072; %initial length of string 3 w/ instrument at origin in mm
stepsPerRevolution = 2048; %Steppers
vecs = [];
%Position of spool 1
x_1 = 564;
y_1 = -7;
z_1 = 1019; 
%Position of spool 2
x_2 = -288;
y_2 = -500;
z_2 = 1017; 
%Position of spool 3
x_3 = -285;
y_3 = 495;
z_3 = 1020; 
while 1
    % collect read data from serial port 
    while isempty(bRead)
        bRead = readline(stepper);
        pause(1.0);
    end  
    steppers = zeros(1,3); %initialize/clear steppers
    read = char(bRead);
    if read(1) == 's'
        hostIP = getip();
        [readTable] = logReads(hostIP(1,:), 9055, multiplexer);    
        inds = strfind(read,'s');
        if read(inds(1)+1) == '1'
            steppers(1) = str2double(read((inds(1)+2):(inds(2)-1)));
        end
        if read(inds(2)+1) == '2'
            steppers(2) = str2double(read((inds(2)+2):(inds(3)-1)));
        end
        if read(inds(3)+1) == '3'
            steppers(3) = str2double(read((inds(3)+2):end));
        end

        d_1 = d1Initial-(steppers(1)*3.1415926*spDi/stepsPerRevolution);
        d_2 = d2Initial-(steppers(2)*3.1415926*spDi/stepsPerRevolution);
        d_3 = d3Initial-(steppers(3)*3.1415926*spDi/stepsPerRevolution);

        syms x y z;
        S = solve(d_1^2 == ((x-x_1)^2)+((y-y_1)^2)+((z-z_1)^2),...
            d_2^2 == ((x-x_2)^2)+((y-y_2)^2)+((z-z_2)^2),...
            d_3^2 == ((x-x_3)^2)+((y-y_3)^2)+((z-z_3)^2));
        x_num = eval(real(S.x))-x_off;
        y_num = eval(real(S.y))-y_off;
        z_num = eval(real(S.z))-z_off+z_off_set;
        fold_name = strcat(instrument,"_",num2str(x_num(1)),"_",num2str(y_num(1)),"_",num2str(z_num(1)));
        mkdir(fold_name);
        %vec = {x_num(1), y_num(1), z_num(1), datetime('now')}; No need
        %because I encoded this info into the folder name
        %vecs = [vecs;vec]; %No need to add to list because we're writing
        %each to csv 
        filename = strcat(fold_name,"/RFID Reads.csv");
        writetable(readTable,filename);
        clear filename readTable fold_name steppers inds d_1 d_2 d_3 x y z x_num y_num z_num;
    else 
        fprintf("running home or incorrect serial output");
    end
    clear read;
    flush(stepper);
    flush(multiplexer);
    bRead = '';
end