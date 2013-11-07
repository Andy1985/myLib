CPP = g++
MAKE = make

FLAGS = -Wall -Werror
#FLAGS += -O4
FLAGS += -g -DDEBUG

XML_LIB_PATH = ./XmlLib
DB_LIB_PATH = ./DBLib
THREAD_LIB_PATH = ./ThreadLib

INCS = -I.
INCS += -I/usr/local/include
INCS += -I$(XML_LIB_PATH)
INCS += -I$(DB_LIB_PATH)
#INCS += -I$(THREAD_LIB_PATH)

ORACLE_H = /home/oracle/app/oracle/product/10g
CDBFLAGS = -I$(ORACLE_H)/plsql/public 
CDBFLAGS += -I$(ORACLE_H)/rdbms/public 
LDDBFLAGS = -L$(ORACLE_H)/lib -lclntsh

LIBS = -L/usr/local/lib #-lmemcached


OBJS = syncemail.o Common.o CXmlConfig.o CDBOption.o CRunLog.o
#THREAD_OBJS = threadpool.o
XML_OBJS = tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o

TARGET = syncemail

all: $(TARGET)

$(TARGET): $(OBJS) $(XML_OBJS)
	$(CPP) -o $@ $(OBJS) $(LDDBFLAGS) $(LIBS) $(XML_OBJS)

%.o: %.cpp
	$(CPP) -o $@ -c $< $(INCS) $(FLAGS) $(CDBFLAGS)

$(XML_OBJS):
	$(MAKE) -C $(XML_LIB_PATH)
	cd $(XML_LIB_PATH) && cp $(XML_OBJS) .. -f

$(THREAD_OBJS):
	$(MAKE) -C $(THREAD_LIB_PATH)
	cd $(THREAD_LIB_PATH) && cp $(THREAD_OBJS) .. -f

.PHONY: clean
clean:
	-rm -f *.o
	#$(MAKE) -C $(THREAD_LIB_PATH) clean
	$(MAKE) -C $(XML_LIB_PATH) clean
	-rm -f $(TARGET)
