
SRC = DemoPub.cpp
SRC += ../common/EvClient.cpp
SRC += ../common/ProtocolInc.cpp
SRC += ../timer/EvTimer.cpp

export STAGING_DIR=../../R16_Tina/out/astar-parrot/staging_dir

#CROSS_COMPILE := ../../toolchain/bin/arm-openwrt-linux-
CROSS_COMPILE :=

all: 
	@$(CROSS_COMPILE)g++ $(SRC) -o demopub -std=c++11 -lpthread -levent -levent_pthreads

clean:
	@rm -f demopub



