
CROSS_COMPILE=
CC=$(CROSS_COMPILE)gcc

# 当前路径
PWD := $(shell pwd)

#库
LIB += -lpthread 

#参数
CFLAG= -g

#.cpp源文件
SRCS=$(wildcard ./*.c)

#头文件
INC=-I$(PWD)/include

#中间文件
OBJS=$(SRCS:.c=.o)

#二进制文件
BIN=main

#start，表示开始执行
start:$(OBJS)
	$(CC) -o $(BIN) $(OBJS) $(CFLAG) $(LIB)
	
.cpp.o:
	$(CC) $(INC)  -o $@ -c $< $(CFLAB) $(LIB)

.PHONY : clean
clean:
	rm -f ${BIN} ./*.o


