

LIBSLANKDEV := /home/slank/git/slankdev/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11 -I.
LDFLAGS  = -lpthread

.cc.o:
	@echo "CXX $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

SRC = \
		main.cc            \
		ssn_vty.cc         \
		vty_server.cc      \
		vty_cmd_history.cc \
		vty_cmd.cc         \
		vty_keyfunction.cc \
		vty_client.cc
OBJ = $(SRC:.cc=.o)

all: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o a.out $(LDFLAGS)

clean:
	rm -rf *.o *.out


