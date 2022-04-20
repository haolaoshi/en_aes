CC=g++
TARGET=libcipher.so
CFLAGS=-m32 -c -fPIC
OBJ=base64.o crypto.o cipher.o
LIBS=-lcrypto

$(TARGET):$(OBJ)
	$(CC) $^ -shared -m32 -lcrypto -o $@   

%.o:%.cpp 
	$(CC) $(CFLAGS) -c -o $@  $< 

clean:
	@rm -rf *.o *.so 
