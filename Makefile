

LIBSLANKDEV := /home/slank/git/slankdev/libslankdev
CXXFLAGS = -I$(LIBSLANKDEV) -std=c++11 -I.

.cc.o:
	@echo "CXX $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

SRC = \
		vty_server.cc      \
		vty_keyfunction.cc \
		vty_client.cc
OBJ = $(SRC:.cc=.o)

libvty.a: $(OBJ)
	@ar rcs libvty.a $(OBJ)
	@echo "AR $@"

clean:
	rm -rf *.o *.a

re: clean all
