
SRC = Ls01aDriver.cpp
SRC += SerialPort.cpp
SRC += ScanStartAck.cpp
SRC += ScanEndAck.cpp
SRC += LidarData.cpp
SRC += ScanPoint.cpp 

SRC += ../common/ProtocolInc.cpp
SRC += ../common/EvClient.cpp

DST := lidarpub

all: 
	@g++ $(SRC) -o $(DST) -std=c++11 -levent -levent_pthreads -DTEST
clean:
	@rm -f $(DST)



