
%readme
%this collects data without linking it to positions of the string localizer
%for use in mock clinical evaluation 

%change logReads to read for the amount of time the arduino waits at a
%position
%the arduino controls the timing of the matlab script through the serial
%port. If a "0" line is read, matlab waits until a line starting with s1 is
%read - that means the arduino is at a new position and matlab should start
%listening to the IP port. 
clear all;
multiplexer = serialport('COM12', 115200); %Manually put in the COM port for the multiplexer arduino
flush(multiplexer);
configureTerminator(multiplexer,"CR")

%while 1
    hostIP = getip();
    [readTable] = logReads(hostIP(1,:), 9055, multiplexer);    
    filename = '-x.csv';
    writetable(readTable,filename);
    clear filename readTable fold_name;
    flush(multiplexer);
%end