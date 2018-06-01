
SRC = EvTcpHandler.cpp
SRC += EvCoreServer.cpp
SRC += ../common/ProtocolInc.cpp

export STAGING_DIR=../../R16_Tina/out/astar-parrot/staging_dir

CROSS_COMPILE :=
#CROSS_COMPILE := ../../toolchain/bin/arm-openwrt-linux-

all: 
	@$(CROSS_COMPILE)g++ $(SRC) -o core_ipc -std=c++11 -levent -levent_pthreads -DTEST

clean:
	@rm -f core_ipc



