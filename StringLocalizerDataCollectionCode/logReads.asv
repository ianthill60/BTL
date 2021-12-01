%%%%
%This function listents to a TCP/IP port and logs the reads. 
%Input: IP Address (string), Port (uint), Number of Reads (uint).
%Output: masterTable and masterCell of all the reads.
%Example usage: [T,C] = logReads('10.197.91.245', 9055, 10)
%%%

function [masterTable] = logReads(ipAddress, port, multi)
%get first read, which will be the connection read.
%fprintf('Establishing connection...');
%decoderFileName = '/Users/ianth/Box Sync/Brain Tool Laboratory/Tool Tracking/Programs/Program1/CaseDecoder_WithImages3.csv';
t = tcpclient(ipAddress, port);
bufferRead = ''; %variable for reading in buffer from IP port
%clear the first welcome message from the buffer
while isempty(bufferRead)
    bufferRead = strcat(read(t));
end  
bufferRead = '';
dataLabels = {'EPC', 'Time', 'AMPM','dBi','count', 'antenna', 'frequency', 'phase'};
global masterTable;
masterTable = table([],[],[],[],[],[],[],[],'VariableNames',dataLabels);

%setup time of reading before writing table and passing on. 
t1 = datetime('now');
%Determines the amount of seconds of reading before writing to table and
%carying out data analysis
t2 = t1+seconds(118); %Manually enter time you want to read for without flushing buffer
flush(multi); %clear old antenna numbers from multiplexer
ants = [1, 2, 3]; %change this to what antennas you want to read from
while(datetime('now')<t2)
    %get read
    reads = cell(1,3); %change this to the number of antennas 
    for antNum = 0:2 %change this to the number of antennas 
        disp(ants(antNum+1));
        write(multi,ants(antNum+1)-1,"uint8"); %collect antenna number from serial port 
        flush(t);
        while isempty(bufferRead)
            bufferRead = strcat(read(t));
        end    
        %Got Read, Parsing Data
        reads{antNum+1}=bufferRead; %keep this at antNum+1 - it has to do with matlab starting index at 1 and arduino starting at 0.
        %clear for next cycle
        bufferRead = '';
        flush(multi);
    end
    
    for i = 1:length(reads)       
        %split by lines (may get multiple reads from buffer)
        bufferLines = strsplit(reads{i},'\n');
        %Parse line by line
        for k = 1:length(bufferLines)
            myLine = bufferLines{k};
            if ~isempty(myLine) %& (myLine(1) == 'E')
               %do something with the data
               myData = strsplit(myLine);
               myStruct = cell2struct(myData(1:8),dataLabels,2);
               myStruct.dBi = str2num(myStruct.dBi); %needed for vertcat
               myStruct.count = str2num(myStruct.count); %needed for vertcat
               myStruct.antenna = ants(i); 
               asdf = struct2table(myStruct,'AsArray',true);
               masterTable = cat(1,masterTable, asdf); %save to table
            end 
        end
        bufferLines = '';
    end
    clear antNum;
end
    fprintf('reading done');
end 
