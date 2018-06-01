LOCALCONFIG_FILE := ../mkconfig
include $(LOCALCONFIG_FILE)

SRCS = $(wildcard *.cpp) 
OBJS = $(SRCS:.cpp=.o) 

all:$(OBJS)

%.o: %.cpp
	$(CPP) $(CPPFLAGS) $(INC_FLAGS) $(LIB_FLAGS) $(DEF_FLAGS) -c $< -o $@
	$(CP) $@ $(OBJ_DIR)

clean:
	$(RM) $(OBJS)

