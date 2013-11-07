CPP = g++
MAKE = make

FLAGS = -Wall -Werror
#FLAGS += -O4
FLAGS += -g -DDEBUG

DB_LIB_PATH = ./DBLib

INCS = -I.
INCS += -I/usr/local/include
INCS += -I$(DB_LIB_PATH)

TIMESTEN_PATH = /home/timesten/TimesTen/tt1122
CDBFLAGS = -I$(TIMESTEN_PATH)/include
LDDBFLAGS = -L$(TIMESTEN_PATH)/lib -lttclient 


OBJS = test.o CDBOption.o

TARGET = test

all: $(TARGET)

$(TARGET): $(OBJS) 
	$(CPP) -o $@ $(OBJS) $(LDDBFLAGS) 

%.o: %.cpp
	$(CPP) -o $@ -c $< $(INCS) $(FLAGS) $(CDBFLAGS)

.PHONY: clean
clean:
	-rm -f *.o
	-rm -f $(TARGET)
