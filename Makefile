#Makefile

# Specify compiler
CXX = g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb 
CXXFLAGS = -std=c++11 -Wall -g3 -c -pthread

# object files
OBJS = log.o main.o consumer_order.o consumer_execution.o producer_order.o order_queue.o

#  millisleep.o init.o items.o

# Program name
PROGRAM = tradepipeline

# The program depends upon its object files
$(PROGRAM) : $(OBJS)
	$(CXX) -o $(PROGRAM) $^

main.o : main.cpp
	$(CXX) $(CXXFLAGS) main.cpp

consumer_order.o : consumer_order.cpp consumer_order.h
	$(CXX) $(CXXFLAGS) consumer_order.cpp

consumer_execution.o : consumer_execution.cpp consumer_execution.h
	$(CXX) $(CXXFLAGS) consumer_execution.cpp

producer_order.o: producer_order.cpp producer_order.h
	$(CXX) $(CXXFLAGS) producer_order.cpp

order_queue.o: order_queue.cpp order_queue.h
	$(CXX) $(CXXFLAGS) order_queue.cpp

#items.o: items.cpp items.h
#	$(CXX) $(CXXFLAGS) items.cpp

#init.o: init.cpp init.h
#	$(CXX) $(CXXFLAGS) init.cpp

log.o: log.cpp log.h
	$(CXX) $(CXXFLAGS) log.cpp

#millisleep.o: millisleep.cpp millisleep.h
#	$(CXX) $(CXXFLAGS) millisleep.cpp

# Once things work, people frequently delete their object files.
# If you use "make clean", this will do it for you.
# As we use gnuemacs which leaves auto save files terminating
# with ~, we will delete those as well.
clean :
	rm -f *.o $(PROGRAM)

