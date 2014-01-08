/*
 * test.c
 *
 *  Created on: 13.03.2012
 *      Author: michaelbrenner
 */
#include "access.h"
#include "alu.h"
#include "trans.h"
#include "crypto.h"
#include "cu.h"
#include <time.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <pthread.h>

struct access_parm
{
	fhe_pk_t pk;
	fhe_sk_t sk;
	_fhe_int *adr;
	_fhe_int *data;
	int fromrow;
	int torow;
};

int test_access(fhe_pk_t pk,fhe_sk_t sk);
int test_access_par(fhe_pk_t pk,fhe_sk_t sk);
int test_access_thr();
int test_alu3(fhe_pk_t pk,fhe_sk_t sk);
int test_cu(fhe_pk_t pk,fhe_sk_t sk);
int test_rec(fhe_pk_t pk,fhe_sk_t sk);
int test_search(fhe_pk_t pk,fhe_sk_t sk);

void *thread0(void *parm);
void *thread1(void *parm);
void *thread2(void *parm);
void *thread3(void *parm);

pthread_t t0,t1,t2,t3;
clock_t _mem;

fhe_pk_t globalpk;
fhe_sk_t globalsk;

int main(int argc,char **argv)
{
	fhe_sk_t sk;
	fhe_pk_t pk;

	fhe_pk_init(globalpk);
	fhe_sk_init(globalsk);

	//puts("keygen...");fflush(stdout);
	//fhe_keygen(pk,sk);

	fhe_sk_load(globalsk,"key512.sk");
	fhe_pk_load(globalpk,"key512.pk");

	//test_search(pk,sk);
	//test_rec(pk,sk);
	//test_cu(pk,sk);
	//test_access_par(globalpk,globalsk);
	test_access(globalpk,globalsk);
	//test_alu3(pk,sk);
/*
	fhe_sk_clear(sk);
	fhe_pk_clear(pk);

	fhe_pk_init(pk);
	fhe_sk_init(sk);

	fhe_sk_load(sk,"key512.sk");
	fhe_pk_load(pk,"key512.pk");

	test_cu(pk,sk);

	fhe_sk_clear(sk);
	fhe_pk_clear(pk);


	fhe_pk_init(pk);
	fhe_sk_init(sk);

	fhe_sk_load(sk,"key768.sk");
	fhe_pk_load(pk,"key768.pk");

	test_cu(pk,sk);

	fhe_sk_clear(sk);
	fhe_pk_clear(pk);


	fhe_pk_init(pk);
	fhe_sk_init(sk);

	fhe_sk_load(sk,"key1024.sk");
	fhe_pk_load(pk,"key1024.pk");

	test_cu(pk,sk);

	fhe_sk_clear(sk);
	fhe_pk_clear(pk);
*/
}

int test_rec(fhe_pk_t pk,fhe_sk_t sk)
{
	clock_t _clock=0;

	fhe_int_t f;
	fhe_int_init(f);
	fhe_int_encrypt(f,pk,1);

	puts("recrypt");fflush(stdout);

	_clock=clock();
	for(int i=0;i<1000;i++)
	{
		fhe_recrypt(f->mpz,pk);
	}
	_clock=clock()-_clock;

	fhe_int_clear(f);

	printf("1k recrypt=%ld\n",(_clock/CLOCKS_PER_SEC));

	return 0;
}

int test_alu3(fhe_pk_t pk,fhe_sk_t sk)
{
	_fhe_int *op1;
	_fhe_int *op2;
	_fhe_int *opcode;
	_fhe_int *res;
	fhe_int_t carry;
	long t=0;

	fhe_int_init(carry);
	fhe_int_encrypt(carry,pk,1);

	op1=vec_init(DATA_BITS);
	op2=vec_init(DATA_BITS);
	res=vec_init(DATA_BITS);
	opcode=vec_init(2);

	trans_int2data(1,op1,pk);
	trans_int2data(7,op2,pk);
	trans_int2vec(OPCODE_XOR,opcode,2,pk);

	for(int i=0;i<100;i++)
	{
		t+=alu3(res,opcode,carry,op1,op2,pk,sk);
	}
	printf("avg=%f\n",(float)t/CLOCKS_PER_SEC/100);

	trans_print(res,DATA_BITS,sk);

	return 0;
}

//void *thread0(void *parm)
//{
//	fhe_pk_t mypk;
//
//	struct access_parm *p=(struct access_parm*)parm;
//
//
//	printf("thread0 start %d %d\n",p->fromrow,p->torow);
//	accessr_thr0(p->adr,p->data,globalpk,globalsk,p->fromrow,p->torow);
//	printf("thread0 end\n");
//
//	return NULL;
//}
//
//void *thread1(void *parm)
//{
//	struct access_parm *p=(struct access_parm*)parm;
//
//	printf("thread1 start %d %d\n",p->fromrow,p->torow);
//	accessr_thr1(p->adr,p->data,globalpk,globalsk,p->fromrow,p->torow);
//	printf("thread1 end\n");
//
//	return NULL;
//}
//
//void *thread2(void *parm)
//{
//	struct access_parm *p=(struct access_parm*)parm;
//
//	printf("thread2 start %d %d\n",p->fromrow,p->torow);
//	accessr_thr2(p->adr,p->data,globalpk,globalsk,p->fromrow,p->torow);
//	printf("thread2 end\n");
//
//	return NULL;
//}
//
//void *thread3(void *parm)
//{
//	struct access_parm *p=(struct access_parm*)parm;
//
//	printf("thread3 start %d %d\n",p->fromrow,p->torow);
//	accessr_thr3(p->adr,p->data,globalpk,globalsk,p->fromrow,p->torow);
//	printf("thread3 end\n");
//
//	return NULL;
//}

//int test_access_thr()
//{
//	_fhe_int *data0,*data1,*data2,*data3;
//	_fhe_int* adr;
//	//fhe_int_t rw;
//	clock_t timer=0;
//	fhe_int_t row0,row1,row2,row3;
//	struct access_parm parm0,parm1,parm2,parm3;
//
//	access_init(ROWS,WORD_SIZE,globalpk);
//
//	data0=vec_init(WORD_SIZE);
//	trans_int2data(128+31,data0,globalpk);
//
//	data1=vec_init(WORD_SIZE);
//	trans_int2data(128+31,data1,globalpk);
//
//	data2=vec_init(WORD_SIZE);
//	trans_int2data(128+31,data2,globalpk);
//
//	data3=vec_init(WORD_SIZE);
//	trans_int2data(128+31,data3,globalpk);
//
//	adr =vec_init(ADR_BITS);
//	trans_int2adr(23,adr,globalpk);
//
//	parm0.adr=parm1.adr=parm2.adr=parm3.adr=adr;
//	parm0.data=data0;parm1.data=data1;parm2.data=data2;parm3.data=data3;
//
//	parm0.fromrow=0;parm0.torow=8;
//	parm1.fromrow=8;parm1.torow=16;
//	parm2.fromrow=16;parm2.torow=24;
//	parm3.fromrow=24;parm3.torow=32;
//
//	printf("master thread\n");
//
//	printf("master pk=%08x sk=%08x\n",globalpk,globalsk);
//
//	if(pthread_create(&t0, NULL, thread0, &parm0)) {
//		fprintf(stderr, "Error creating thread 0\n");
//		return 1;
//	}
//
//	if(pthread_create(&t1, NULL, thread1, &parm1)) {
//		fprintf(stderr, "Error creating thread 1\n");
//		return 1;
//	}
//
//	if(pthread_create(&t2, NULL, thread2, &parm2)) {
//		fprintf(stderr, "Error creating thread 2\n");
//		return 1;
//	}
//
//	if(pthread_create(&t3, NULL, thread3, &parm3)) {
//		fprintf(stderr, "Error creating thread 3\n");
//		return 1;
//	}
//
//	printf("threads created\n");
//
//	if(pthread_join(t0, NULL)) {
//		fprintf(stderr, "Error joining thread 0\n");
//		return 2;
//	}
//
//	if(pthread_join(t1, NULL)) {
//		fprintf(stderr, "Error joining thread 1\n");
//		return 2;
//	}
//
//	if(pthread_join(t2, NULL)) {
//		fprintf(stderr, "Error joining thread 2\n");
//		return 2;
//	}
//
//	if(pthread_join(t3, NULL)) {
//		fprintf(stderr, "Error joining thread 3\n");
//		return 2;
//	}
//
//	return 0;
//}

int test_access_par(fhe_pk_t pk,fhe_sk_t sk)
{
	_fhe_int* data;
	_fhe_int* adr;
	//fhe_int_t rw;
	clock_t timer=0;
	fhe_int_t row0,row1;
	int status;
	char *shmaddr;

	access_init(ROWS,WORD_SIZE,pk);

	data=vec_init(WORD_SIZE);
	trans_int2data(128+31,data,pk);

	adr =vec_init(ADR_BITS);
	trans_int2adr(16,adr,pk);

	//accessr(adr,data,pk,sk);printf("\n");

	fhe_int_init(row0);
	fhe_int_init(row1);

	int shmid=shmget(0x07081974, 65536, IPC_CREAT|0600 );

	int pid=fork();

	if(pid==0) //child
	{
		timer+=accessr0(adr,data,&row0,pk,sk);
		printf("row0=%d\n",fhe_decrypt(row0->mpz,sk));
		shmaddr=shmat(shmid,NULL,0);

		mpz_init(*((mpz_t*)shmaddr));
		mpz_set(*((mpz_t*)shmaddr),row0->mpz);

		shmdt(shmaddr);

		exit(0);
	}
	else
	{
		timer+=accessr1(adr,data,&row1,pk,sk);
		printf("row1=%d\n",fhe_decrypt(row1->mpz,sk));

		wait(&status);
	}

	shmaddr=shmat(shmid,NULL,0);
	//mpz_out_str(stdout,10,*((mpz_t*)shmaddr));


	printf("avg clock=%ld\n",timer/CLOCKS_PER_SEC/2);
	printf("bank0=%d bank1=%d\n",fhe_decrypt(row0->mpz,sk),fhe_decrypt(row1->mpz,sk));
	shmdt(shmaddr);

	//access_dump(ROWS,WORD_SIZE,sk);

	printf("AC:");trans_print(data,WORD_SIZE,sk);printf("\n");

	//fhe_int_clear(rw);
	vec_clear(data,WORD_SIZE);
	vec_clear(adr,ADR_BITS);
	access_clear(ROWS,WORD_SIZE);
	return 0;
}

int test_access(fhe_pk_t pk,fhe_sk_t sk)
{
	_fhe_int* data;
	_fhe_int* adr;
	//fhe_int_t rw;
	clock_t timer=0;

	puts("test_access");
	//fhe_int_init(rw);

	access_init(ROWS,WORD_SIZE,pk);
	//access_dump(ROWS,WORD_SIZE,sk);

	//access_set(access_array[3*DATA_BITS+0],1,pk);
	//access_set(access_array[3*DATA_BITS+2],1,pk);

	//access_dump(ROWS,DATA_BITS,sk);

	data=vec_init(WORD_SIZE);
	trans_int2data(128+31,data,pk);

	adr =vec_init(ADR_BITS);
	trans_int2adr(3,adr,pk);

	//fhe_int_encrypt(rw,pk,WRITE);

	//printf("adr=");
	//trans_print(adr,ADR_BITS,sk);

	//puts("");

	//printf("AC:");trans_print(data,WORD_SIZE,sk);printf("\n");

	//printf("bit size=%ld\n",mpz_sizeinbase((&data[0])->mpz,2));
	//printf("memory size=%ld\n",mpz_sizeinbase((&data[0])->mpz,2)*ROWS*WORD_SIZE/8192);

	accessw(adr,data,pk,sk);
	for(int i=0;i<10;i++)
	{
		printf("RUN%d ",i);

		//timer+=accessw(adr,data,pk,sk);
		timer+=accessr(adr,data,pk,sk);
		printf("AC:");trans_print(data,WORD_SIZE,sk);printf("\n");
		//printf("\n");

	}
	printf("avg clock=%ld\n",timer/CLOCKS_PER_SEC/RUNS);


	access_dump(ROWS,WORD_SIZE,sk);

	printf("AC:");trans_print(data,WORD_SIZE,sk);printf("\n");

	//fhe_int_clear(rw);
	vec_clear(data,WORD_SIZE);
	vec_clear(adr,ADR_BITS);
	access_clear(ROWS,WORD_SIZE);
	return 0;
}

int test_cu(fhe_pk_t pk,fhe_sk_t sk)
{
	clock_t clock=0;

	puts("TEST CU");fflush(stdout);

	_mem=0;

	for(int i=0;i<1;i++)
	{
		clock+=cu(pk,sk);
		printf(".");fflush(stdout);
	}
	printf("avg cu=%ld avg mem=%ld\n",clock/CLOCKS_PER_SEC,_mem/CLOCKS_PER_SEC/3);

	return 0;
}

int test_search(fhe_pk_t pk,fhe_sk_t sk)
{
	clock_t clockt,_clock;
	_fhe_int *a,*b,*res;
	fhe_int_t one;
	int sizes[4]={5,16,32,64};

	puts("TEST SEARCH");fflush(stdout);

	fhe_int_init(one);
	fhe_int_encrypt(one,pk,1);

	for(int x=0;x<4;x++)
	{
		int i=sizes[x];

		printf("%d: ",i);fflush(stdout);
		a=vec_init(i);
		b=vec_init(i);
		res=vec_init(i);
		clockt=0;

		for(int j=0;j<i;j++)
		{
			fhe_int_encrypt(&a[j],pk,rand()&1);
			fhe_int_encrypt(&b[j],pk,rand()&1);
		}

		for(int j=0;j<1000;j++)
		{
			_clock=clock();

			for(int k=0;k<i;k++)
			{
				fhe_int_xor(&res[k],&a[k],&b[k],pk);
				fhe_int_xor(&res[k],&res[k],one,pk);
			}

			for(int k=1;k<i;k++)
			{
				fhe_int_and(&res[0],&res[0],&res[k],pk);
			}

			//printf(".");fflush(stdout);
			clockt+=clock()-_clock;
		}

		vec_clear(a,i);
		vec_clear(b,i);

		printf("avg %f\n",(float)clockt/CLOCKS_PER_SEC);
	}


	return 0;

}
