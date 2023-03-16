CC = g++
SRC = stream.cpp
EXEC = stream

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

OPENCV_LIB = $(shell pkg-config opencv4 --libs)
OPENCV_INC = $(shell pkg-config opencv4 --cflags)

CFLAGS += -c -O3 -D_UNIX_ -D_LINUX_ -fPIC -std=c++11 $(JAIGO_INC) $(OPENCV_INC)

$(EXEC):$(EXEC).o
	$(CC) $(EXEC).o -o $(EXEC) $(JAIGO_LIB) $(OPENCV_LIB)

$(EXEC).o:$(SRC)
	$(CC) $(SRC) -o $(EXEC).o $(CFLAGS)  

clean:
	rm -f $(EXEC) $(EXEC).o



	