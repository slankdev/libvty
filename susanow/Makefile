
LIBSLANKDEV := /home/slank/git/slankdev/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11
LDFLAGS  = -lpthread

CXXFLAGS += -I.
SRC = \
		main.cc            \
		ssn_vty.cc         \
		vty_server.cc      \
		vty_cmd_history.cc \
		vty_cmd.cc         \
		vty_keyfunction.cc \
		vty_client.cc

all:
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS)

