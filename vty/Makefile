
LIBSLANKDEV := /home/slank/git/slankdev/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11
LDFLAGS  = -lpthread

CXXFLAGS += -I.

all:
	$(CXX) $(CXXFLAGS) main.cc ssn_vty.cc $(LDFLAGS)

