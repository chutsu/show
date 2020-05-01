include config.mk

default: show

clean:
	@rm -f show

show.o: show.cpp show.hpp
	@$(CXX) $(CFLAGS) -c show.cpp

show: show.o
	@$(CXX) $(CFLAGS) show.o -o show $(LIBS)
