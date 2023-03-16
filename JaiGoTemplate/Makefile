CC = g++
SRC = PvPipelineSample.cpp
EXEC = PvPipelineSample
JAIGO_LIB = -L/opt/jai/ebus_sdk/Ubuntu-20.04-x86_64/lib \
	-lPvAppUtils                 \
	-lPtConvertersLib            \
	-lPvBase                     \
	-lPvBuffer                   \
	-lPvGenICam                  \
	-lPvSystem                   \
	-lPvStream                   \
	-lPvDevice                   \
	-lPvTransmitter              \
	-lPvVirtualDevice            \
	-lPvPersistence              \
	-lPvSerial                   \
	-lPvCameraBridge
JAIGO_INC = -I/opt/jai/ebus_sdk/Ubuntu-20.04-x86_64/include 

CFLAGS += -c $(JAIGO_INC) -O3 -D_UNIX_ -D_LINUX_ -fPIC -std=c++11

#all: 
#$(CC) $(CFLAGS) -o $(EXEC) $(SRC) $(JAIGO_LIB)

$(EXEC):$(EXEC).o
	$(CC) $(EXEC).o -o $(EXEC) $(JAIGO_LIB)

$(EXEC).o:
	$(CC) $(SRC) -o $(EXEC).o $(CFLAGS)  

clean:
	rm -f $(EXEC) $(EXEC).o