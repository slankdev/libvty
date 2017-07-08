
LIBSLANKDEV := $(HOME)/git/slankdev/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11
CXXFLAGS += -Wno-format-security
CXXFLAGS += -fsanitize=address
CXXFLAGS += -g -O0 -std=c++11
LDFLAGS  += -lpthread

all:
	$(CXX) $(CXXFLAGS) main.cc VTY.cc $(LDFLAGS)

clean:
	rm -f a.out

re: clean all


