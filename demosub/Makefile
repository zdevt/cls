
SRC = DemoSub.cpp
SRC += ../common/EvClient.cpp
SRC += ../common/ProtocolInc.cpp

export STAGING_DIR=../../R16_Tina/out/astar-parrot/staging_dir

#CROSS_COMPILE := ../../toolchain/bin/arm-openwrt-linux-
CROSS_COMPILE :=

all: 
	@g++ $(SRC) -o demosub -std=c++11 -levent -levent_pthreads -DTEST

clean:
	@rm -f demosub



