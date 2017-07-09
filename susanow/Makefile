
LIBSLANKDEV := /home/slank/git/slankdev/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11
LDFLAGS  = -lpthread

CXXFLAGS += -I.
SRC = main.cc ssn_vty.cc vty_cmd_history.cc

all:
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS)

