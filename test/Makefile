

LIBVTY := ../
LIBSLANKDEV := $(HOME)/git/slankdev/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -I$(LIBVTY) -I. -std=c++11
LDFLAGS  = -L$(LIBVTY) -lpthread -lvty

all:
	$(CXX) $(CXXFLAGS) main.cc ssn_vty.cc $(LDFLAGS)

clean:
	rm -rf *.o *.out

re: clean all


