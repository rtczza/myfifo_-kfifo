#include "myloopbuf.h"


#define min(a, b)  (((a) < (b)) ? (a) : (b))

/**************************************************************************************** 只在本文件中调用  */
static void myfifo_copy_in(struct my_fifo *fifo, const void *src, unsigned int len, unsigned int off)
{
	unsigned int size = fifo->mask + 1;
	unsigned int esize = fifo->esize;
	unsigned int l;

	off &= fifo->mask;
	if (esize != 1)
		{
			off *= esize;
			size *= esize;
			len *= esize;
		}
	l = min(len, size - off);

	memcpy(fifo->data + off, src, l);
	memcpy(fifo->data, src + l, len - l);

	/*
	 * make sure that the data in the fifo is up to date before
	 * incrementing the fifo->in index counter
	 */
	/* smp_wmb(); */
}

static void myfifo_copy_out(struct my_fifo *fifo, void *dst, unsigned int len, unsigned int off)
{
	unsigned int size = fifo->mask + 1;
	unsigned int esize = fifo->esize;
	unsigned int l;

	off &= fifo->mask;
	if (esize != 1)
		{
			off *= esize;
			size *= esize;
			len *= esize;
		}
	l = min(len, size - off);

	memcpy(dst, fifo->data + off, l);
	memcpy(dst + l, fifo->data, len - l);
	/*
	 * make sure that the data is copied before
	 * incrementing the fifo->out index counter
	 */
	/* smp_wmb(); */
}

static unsigned int myfifo_out_peek(struct my_fifo *fifo, void *buf, unsigned int len)
{
	unsigned int l;

	l = fifo->in - fifo->out;
	if (len > l)
		len = l;

	myfifo_copy_out(fifo, buf, len, fifo->out);
	return len;
}

/*
 * internal helper to calculate the unused elements in a fifo
 */
static inline unsigned int myfifo_unused(struct my_fifo *fifo)
{
	return (fifo->mask + 1) - (fifo->in - fifo->out);
}



/********************************************************************************************************************  */

/**
  * @brief  Round the given value up to nearest power of two.
  * @param  [in] x: The number to be converted.
  * @return The power of two.
  */
static unsigned int roundup_pow_of_two(unsigned int x)
{
	unsigned int b = 0;
	
	int i;
	for(i = 0; i < 32; i++)
		{
			b = 1UL << i;

			if(x <= b)
				{
					break;
				}
		}

	return b;
}


/* 申请内存空间 */
int myfifo_alloc(struct my_fifo *fifo, unsigned int size, size_t esize)
{
	/*
	 * round up to the next power of 2, since our 'let the indices
	 * wrap' technique works only in this case.
	 */
	size = roundup_pow_of_two(size);

	fifo->in = 0;
	fifo->out = 0;
	fifo->esize = esize;

	if (size < 2)
		{
			fifo->data = NULL;
			fifo->mask = 0;
			return -EINVAL;
		}

	/* fifo->data = kmalloc_array(esize, size, gfp_mask); */
	fifo->data = malloc(size);

	if (!fifo->data)
		{
			fifo->mask = 0;
			return -ENOMEM;
		}
	fifo->mask = size - 1;

	return 0;
}

int myfifo_init(struct my_fifo *fifo, void *buffer, unsigned int size, size_t esize)
{
	size /= esize;

	size = roundup_pow_of_two(size);

	fifo->in = 0;
	fifo->out = 0;
	fifo->esize = esize;
	fifo->data = buffer;

	if (size < 2)
		{
			fifo->mask = 0;
			return -EINVAL;
		}
	fifo->mask = size - 1;

	return 0;
}

void myfifo_free(struct my_fifo *fifo)
{
	free(fifo->data);
	fifo->in = 0;
	fifo->out = 0;
	fifo->esize = 0;
	fifo->data = NULL;
	fifo->mask = 0;
}

unsigned int myfifo_in(struct my_fifo *fifo, const void *buf, unsigned int len)
{
	unsigned int l;

	l = myfifo_unused(fifo);
	if (len > l)
		len = l;

	myfifo_copy_in(fifo, buf, len, fifo->in);
	fifo->in += len;
	return len;
}

unsigned int myfifo_out(struct my_fifo *fifo, void *buf, unsigned int len)
{
	len = myfifo_out_peek(fifo, buf, len);
	fifo->out += len;
	return len;
}



























