#ifndef _MYLOOPBUF_H
#define _MYLOOPBUF_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

struct my_fifo
{
	unsigned int	in;
	unsigned int	out;
	unsigned int	mask;
	unsigned int	esize;
	void			*data;
};

//返回已经使用的缓存区大小
#define myfifo_len(fifo)\
({\
	fifo->in - fifo->out;\
})

//判断fifo是否为空,为空则返回true
#define myfifo_is_empty(fifo)\
({\
	fifo->in == fifo->out;\
})

//判断fifo是否满,满则返回true
#define myfifo_is_full(fifo)\
({\
	myfifo_len(fifo) - fifo->mask;\
})

int myfifo_alloc(struct my_fifo *fifo, unsigned int size, size_t esize);

int myfifo_init(struct my_fifo *fifo, void *buffer, unsigned int size, size_t esize);

void myfifo_free(struct my_fifo *fifo);

unsigned int myfifo_in(struct my_fifo *fifo, const void *buf, unsigned int len);

unsigned int myfifo_out(struct my_fifo *fifo, void *buf, unsigned int len);

#endif



