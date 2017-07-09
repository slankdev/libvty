

LIBSLANKDEV := /home/slank/git/slankdev/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11 -I../susanow
LDFLAGS  = -lpthread -lvty -L../susanow

all:
	$(CXX) $(CXXFLAGS) main.cc -o a.out $(LDFLAGS) -lvty

clean:
	rm -rf *.o *.out

re: clean all


