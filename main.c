#include "kfifo.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

static pthread_t ReadId; //读线程ID

static pthread_t WriteId; //写线程ID

struct kfifo event_fifo; //实例化

bool readDone_flag; //读文件完成标准

//读线程:打开“27M.mp4”文件,
void *P_read()
{
	FILE *fp;
	char *pname = "./16M.mp4";
	fp = fopen(pname, "r");
	if(NULL == fp)
		perror("fopen error: ");
	else
		printf ("fopen %s success ! \n", pname);

	char buf[1024] = {0};
	int n;
	int total_len=0;
	readDone_flag = 0;

	while(1)
		{
			if(feof(fp))
				break ;
			n = fread(buf, 1, sizeof(buf), fp);

			n = kfifo_in(&event_fifo, buf, n);
			if(n>0)
				printf("P_read : len :%d_[%d]--- %dM \n",n,  total_len, total_len/(1024*1024));

			total_len = total_len + n;

			sleep(0);
		}

	fclose(fp);
	sleep(1);
	readDone_flag = 1;
}


void *P_write()
{
	FILE *fp;
	char *pname = "test.mp4";
	fp = fopen(pname, "a+");
	if(NULL == fp)
		perror("fopen error: ");
	else
		printf ("fopen %s success ! \n", pname);

	char buf[2048] = {0};
	int n;
	int total_len=0;

	while(1)
		{
			int len = kfifo_len(&event_fifo);

			if(len > 0)
				/* printf("P_write :--------------------------------------------------- len :%d_[%d] \n",len,  total_len); */

			n = (len<2048 ? len:2048);
			n = kfifo_out(&event_fifo, buf, n);

			fwrite(buf, 1, n, fp);

			if((0 == len) && (readDone_flag))
				{
					fclose(fp);
					pthread_exit(NULL);
				}
			total_len = total_len + n;
		}
}


//主函数主要是:给fifo申请内存空间, 创建读写线程,等待读写线程结束。其实5行代码就可以搞定。
int main()
{
	int ret;

	/* Create the FIFO */
	ret = kfifo_alloc(&event_fifo, 1024*1024*4); //4M
	if (ret)
		printf("Out of memory allocating event FIFO buffer\n");


	ret = pthread_create(&ReadId, NULL, P_read, NULL); //创建读线程
	if (ret)
		perror("pthread_create ReadId error :\n");
	else
		printf ("pthread_create ReadId success ! \n");

	ret =  pthread_create(&WriteId, NULL, P_write, NULL);//创建写线程
	if (ret)
		perror("pthread_create WriteId error :\n");
	else
		printf ("pthread_create WriteId success ! \n");


	ret = pthread_join(ReadId, NULL); //等待读线程结束
	if(0 != ret)
		perror("pthread_join ReadId error");
	else
		printf("ReadId pthread_join successed! \n");

	ret = pthread_join(WriteId, NULL); //等待写线程结束
	if(0 != ret)
		perror("pthread_join WriteId error");
	else
		printf("WriteId pthread_join successed! \n");


	kfifo_free(&event_fifo); //释放fifo
	return 0;
}






















