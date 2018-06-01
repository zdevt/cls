
SRC = LogServer.cpp
SRC += ../common/Sqlite3Interface.cpp 
SRC += ../common/Sqlite3InterfaceLog.cpp
SRC += ../common/ProtocolInc.cpp
SRC += ../common/EvClient.cpp

all: 
	@g++ $(SRC) -o logserver -std=c++11 -levent -levent_pthreads -lsqlite3

s: 
	@$(CROSS_COMPILE)g++ $(SRC) -o a -std=c++11 -levent -levent_pthreads -lsqlite3 -DTESTS

clean:
	@rm -f logserver



