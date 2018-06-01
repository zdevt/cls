
SRC = EvClient.cpp
SRC += ProtocolInc.cpp

export STAGING_DIR=../../R16_Tina/out/astar-parrot/staging_dir

CROSS_COMPILE :=
#CROSS_COMPILE := ../../toolchain/bin/arm-openwrt-linux-

all: 
	@$(CROSS_COMPILE)g++ $(SRC) -o a -std=c++11 -levent -levent_pthreads -lsqlite3 -DTEST_EVCLIENT
clean:
	@rm -f a



