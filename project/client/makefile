CC=arm-linux-gcc

TARGET=bin/client
LIB_NAME=./lib/libmy.so
LIB_NAME2=./lib/libmy.a
MAIN_SOURCE=$(wildcard ./main/*.c)
INCLUDE_PATH=-I ./include/
LIB_PATH=-L ./lib -lmy

C_SOURCE=$(wildcard ./src/*.c) 
OBJ=$(patsubst %.c,%.o,$(C_SOURCE))
%.o:%.c
	$(CC) $^ -o $@ -c -fPIC $(INCLUDE_PATH)
	
lib/libmy.so:$(OBJ)
	$(CC) -fPIC -shared -o $@ $^
	
lib/libmy.a:$(OBJ)
	ar rcs $@ $^

$(TARGET):$(MAIN_SOURCE)            
	$(CC) $^ -o $@ $(INCLUDE_PATH) $(LIB_PATH)
	
.PHONY:clean
	
clean:
	$(RM) $(TARGET)
	$(RM) $(LIB_NAME) 
	$(RM) $(LIB_NAME2) 
	$(RM) ./src/*.o
