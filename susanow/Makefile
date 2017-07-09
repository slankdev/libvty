
LIBSLANKDEV := /home/slank/git/slankdev/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11
LDFLAGS  = -lpthread

CXXFLAGS += -I.
SRC = main.cc vty.cc vty_cmd_history.cc vty_cmd.cc vty_keyfunction.cc

all:
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS)

