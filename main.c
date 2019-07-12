#include "myloopbuf.h"
#include <pthread.h>

#define LEN_LOOP 1024


struct my_fifo *fifo;
char buffer[LEN_LOOP];

pthread_t add_id;

pthread_t sub_id;

pthread_t judge_id;


void *Add()
{
	unsigned char add_buf[128]={0};
	memset(add_buf, 'a', sizeof(add_buf));
	
	while(1)
	{
		myfifo_in(fifo, add_buf, sizeof(add_buf));
		
		sleep(2);
	}

}

void *Sub()
{
	unsigned char sub_buf[256]={0};
	int i;
	while(1)
	{
		myfifo_out(fifo, sub_buf, 64);
		
		sleep(1);
		for(i=0; i<64; i++)
			printf("%c ", sub_buf[i]);
		printf("\n");
	}
}


void *Judge()
{
	int ret;
	while(1)
	{
		ret = myfifo_is_full(fifo);
		if(1 == ret)
			printf("****************** [Full] ********** \n");
		
		ret = myfifo_is_empty(fifo);
		if(1 == ret)
			printf("****************** [Empty] ********** \n");
		
		ret = myfifo_len(fifo);
		printf("---------------- used :%d \n", ret);
		
		sleep(1);
	}
}




int main()
{
	fifo = malloc(sizeof(struct my_fifo));
	
	myfifo_alloc(fifo, (unsigned int)LEN_LOOP, sizeof(char));

	/* myfifo_init(fifo, buffer, sizeof(buffer), sizeof(char)); */
	
	pthread_create(&judge_id, NULL, Judge, NULL);

	sleep(2);
	
	pthread_create(&add_id, NULL, Add, NULL);
	
	pthread_create(&sub_id, NULL, Sub, NULL);
		
	while(1){sleep(1);}

	return 0;
}





















