.PHONY : all debug release backup
.SUFFIXES : .cpp .o

SOURCES  := $(wildcard *.cpp)
INCLUDES := -I../threadpool/
OBJECTS  := $(SOURCES:.cpp=.o)
LIBRARY := -lpthread
CPP := g++
#CPP := arm-linux-gnueabihf-g++
TARGET = run

all : debug

$(TARGET) : $(OBJECTS)
	$(CPP) -o $@  $^ -pg $(LIBRARY)

.cpp.o : $(SOURCES)
	$(CPP) $(SOURCES) $(CPPFLAGS) $(INCLUDES) $(LIBRARY)

clean :
	rm -rf $(OBJECTS) $(TARGET) *~ gmon.out *.bak

debug : CPPFLAGS := -g -c -Wall -std=c++11
debug : $(TARGET)

release : CPPFLAGS := -O3 -c -Wall -std=c++11
release : $(TARGET)
#	./$(TARGET)
#	gprof ./$(TARGET) gmon.out
