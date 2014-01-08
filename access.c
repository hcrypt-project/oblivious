/*
 * access.c
 *
 *  Created on: 13.03.2012
 *      Author: michaelbrenner
 */
#include "access.h"
#include "crypto.h"
#include "integer-fhe.h"
#include <sys/time.h>

void access_set(fhe_int_t m,int i,fhe_pk_t pk)
{
	fhe_int_encrypt(m,pk,i);
}

void access_init(int rows,int cols,fhe_pk_t pk)
{
	access_rowselect=malloc(rows*sizeof(_fhe_int));

	for(int i=0;i<rows;i++)
		fhe_int_init(&access_rowselect[i]);

	access_array=malloc(rows*cols*sizeof(_fhe_int));
	access_array_noise=malloc(rows*cols*sizeof(int));

	for(int i=0;i<rows*cols;i++)
	{
		fhe_int_init(&access_array[i]);
		access_array_noise[i]=1;
	}

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			access_set(&access_array[(i*cols)+j],(i+j)&1,pk);
		}
	}
}

void access_clear(int rows,int cols)
{
	for(int i=0;i<rows;i++)
		fhe_int_clear(&access_rowselect[i]);

	for(int i=0;i<rows*cols;i++)
		fhe_int_clear(&access_array[i]);

	free(access_rowselect);
	free(access_array);
	free(access_array_noise);
}

void access_dump(int rows,int cols,fhe_sk_t sk)
{
	static int count=0;
	printf("---dump %d start---\n",++count);
	for(int i=0;i<rows;i++)
	{
		printf("%d: ",i);
		for(int j=0;j<cols;j++)
		{
			printf("%d ",fhe_decrypt((&access_array[i*cols+j])->mpz,sk));
		}
		printf("\n");
	}
	printf("---dump %d end  ---\n",count);
}


// implements oblivious memory access
//
clock_t accessr(_fhe_int *adr,_fhe_int *data,fhe_pk_t pk,fhe_sk_t sk)
{
	//timeval start, end;
	clock_t _clock;
	// adr[0] is lsb
	fhe_int_t sel,nsel;
	fhe_int_t temp,temp2;
	//fhe_int_t nrw;
	_fhe_int *nadr;
	//fmpz *reg;
	//gettimeofday(&start, 0);
	_clock=clock();

	fhe_int_init(temp);
	fhe_int_init(temp2);
	fhe_int_init(sel);
	//fhe_int_init(nrw);
	fhe_int_init(nsel);

	//fhe_int_not(nrw,rw,pk);

	//generate inverse address bits
	nadr=vec_init(ADR_BITS);


	for(int i=0;i<ADR_BITS;i++)
	{
		fhe_int_not(&nadr[i],&adr[i],pk);
	}

	//reg=_fmpz_vec_init(DATA_BITS);

	printf("reg ");
	//for every row...
	for(int a=0;a<ROWS;a++)
	{
		printf(" %d(",a);
		fhe_int_encrypt(sel,pk,1);

		//...generate select signal (is a the address in adr?)
		for(int i=0,mask=1;i<ADR_BITS;i++,mask*=2)
		{
			fhe_int_and(sel,sel,((a&mask)>0)?&adr[i]:&nadr[i],pk);
		}
		fhe_int_not(nsel,sel,pk);

		//printf("row %d %d\n",a,fhe_decrypt(sel->mpz,sk));

		for(int i=0;i<WORD_SIZE;i++)
		{
			fhe_int_and(temp,&data[i],nsel,pk);
			fhe_int_and(temp2,&access_array[a*WORD_SIZE+i],sel,pk);
			fhe_int_xor(&data[i],temp,temp2,pk);
			printf("%d ",(&data[i])->noise);
		}
		printf(")");
	}
	printf("\n");
	vec_clear(nadr,ADR_BITS);
	//_fmpz_vec_clear(reg,DATA_BITS);
	fhe_int_clear(temp);
	fhe_int_clear(temp2);
	fhe_int_clear(sel);
	fhe_int_clear(nsel);
	//fhe_int_clear(nrw);

	//gettimeofday(&end, 0);

	return clock()-_clock;
}

// implements oblivious memory access
//
clock_t accessr_thr0(_fhe_int *adr,_fhe_int *data,fhe_pk_t pk,fhe_sk_t sk,int x,int y)
{
	//timeval start, end;
	clock_t _clock;
	// adr[0] is lsb
	fhe_int_t sel,nsel;
	fhe_int_t temp,temp2;
	//fhe_int_t nrw;
	_fhe_int *nadr;
	//fmpz *reg;
	//gettimeofday(&start, 0);
	_clock=clock();

	fhe_int_init(temp);
	fhe_int_init(temp2);
	fhe_int_init(sel);
	//fhe_int_init(nrw);
	fhe_int_init(nsel);

	//fhe_int_not(nrw,rw,pk);

	//generate inverse address bits
	nadr=vec_init(ADR_BITS);


	for(int i=0;i<ADR_BITS;i++)
	{
		fhe_int_not(&nadr[i],&adr[i],pk);
	}

	//reg=_fmpz_vec_init(DATA_BITS);

	//printf("reg ");
	//for every row...
	for(int a=x;a<y;a++)
	{
		//printf(" %d(",a);
		fhe_int_encrypt(sel,pk,1);

		//...generate select signal (is a the address in adr?)
		for(int i=0,mask=1;i<ADR_BITS;i++,mask*=2)
		{
			fhe_int_and(sel,sel,((a&mask)>0)?&adr[i]:&nadr[i],pk);
		}
		fhe_int_not(nsel,sel,pk);

		//printf("row %d %d\n",a,fhe_decrypt(sel->mpz,sk));

		for(int i=0;i<WORD_SIZE;i++)
		{
			fhe_int_and(temp,&data[i],nsel,pk);
			fhe_int_and(temp2,&access_array[a*WORD_SIZE+i],sel,pk);
			fhe_int_xor(&data[i],temp,temp2,pk);
			//printf("%d ",(&data[i])->noise);
		}
		//printf(")");
	}
	//printf("\n");
	vec_clear(nadr,ADR_BITS);
	//_fmpz_vec_clear(reg,DATA_BITS);
	fhe_int_clear(temp);
	fhe_int_clear(temp2);
	fhe_int_clear(sel);
	fhe_int_clear(nsel);
	//fhe_int_clear(nrw);

	//gettimeofday(&end, 0);

	return clock()-_clock;
}
clock_t accessr_thr1(_fhe_int *adr,_fhe_int *data,fhe_pk_t pk,fhe_sk_t sk,int x,int y)
{
	//timeval start, end;
	clock_t _clock;
	// adr[0] is lsb
	fhe_int_t sel,nsel;
	fhe_int_t temp,temp2;
	//fhe_int_t nrw;
	_fhe_int *nadr;
	//fmpz *reg;
	//gettimeofday(&start, 0);
	_clock=clock();

	fhe_int_init(temp);
	fhe_int_init(temp2);
	fhe_int_init(sel);
	//fhe_int_init(nrw);
	fhe_int_init(nsel);

	//fhe_int_not(nrw,rw,pk);

	//generate inverse address bits
	nadr=vec_init(ADR_BITS);


	for(int i=0;i<ADR_BITS;i++)
	{
		fhe_int_not(&nadr[i],&adr[i],pk);
	}

	//reg=_fmpz_vec_init(DATA_BITS);

	//printf("reg ");
	//for every row...
	for(int a=x;a<y;a++)
	{
		//printf(" %d(",a);
		fhe_int_encrypt(sel,pk,1);

		//...generate select signal (is a the address in adr?)
		for(int i=0,mask=1;i<ADR_BITS;i++,mask*=2)
		{
			fhe_int_and(sel,sel,((a&mask)>0)?&adr[i]:&nadr[i],pk);
		}
		fhe_int_not(nsel,sel,pk);

		//printf("row %d %d\n",a,fhe_decrypt(sel->mpz,sk));

		for(int i=0;i<WORD_SIZE;i++)
		{
			fhe_int_and(temp,&data[i],nsel,pk);
			fhe_int_and(temp2,&access_array[a*WORD_SIZE+i],sel,pk);
			fhe_int_xor(&data[i],temp,temp2,pk);
			//printf("%d ",(&data[i])->noise);
		}
		//printf(")");
	}
	//printf("\n");
	vec_clear(nadr,ADR_BITS);
	//_fmpz_vec_clear(reg,DATA_BITS);
	fhe_int_clear(temp);
	fhe_int_clear(temp2);
	fhe_int_clear(sel);
	fhe_int_clear(nsel);
	//fhe_int_clear(nrw);

	//gettimeofday(&end, 0);

	return clock()-_clock;
}
clock_t accessr_thr2(_fhe_int *adr,_fhe_int *data,fhe_pk_t pk,fhe_sk_t sk,int x,int y)
{
	//timeval start, end;
	clock_t _clock;
	// adr[0] is lsb
	fhe_int_t sel,nsel;
	fhe_int_t temp,temp2;
	//fhe_int_t nrw;
	_fhe_int *nadr;
	//fmpz *reg;
	//gettimeofday(&start, 0);
	_clock=clock();

	fhe_int_init(temp);
	fhe_int_init(temp2);
	fhe_int_init(sel);
	//fhe_int_init(nrw);
	fhe_int_init(nsel);

	//fhe_int_not(nrw,rw,pk);

	//generate inverse address bits
	nadr=vec_init(ADR_BITS);


	for(int i=0;i<ADR_BITS;i++)
	{
		fhe_int_not(&nadr[i],&adr[i],pk);
	}

	//reg=_fmpz_vec_init(DATA_BITS);

	//printf("reg ");
	//for every row...
	for(int a=x;a<y;a++)
	{
		//printf(" %d(",a);
		fhe_int_encrypt(sel,pk,1);

		//...generate select signal (is a the address in adr?)
		for(int i=0,mask=1;i<ADR_BITS;i++,mask*=2)
		{
			fhe_int_and(sel,sel,((a&mask)>0)?&adr[i]:&nadr[i],pk);
		}
		fhe_int_not(nsel,sel,pk);

		//printf("row %d %d\n",a,fhe_decrypt(sel->mpz,sk));

		for(int i=0;i<WORD_SIZE;i++)
		{
			fhe_int_and(temp,&data[i],nsel,pk);
			fhe_int_and(temp2,&access_array[a*WORD_SIZE+i],sel,pk);
			fhe_int_xor(&data[i],temp,temp2,pk);
			//printf("%d ",(&data[i])->noise);
		}
		//printf(")");
	}
	//printf("\n");
	vec_clear(nadr,ADR_BITS);
	//_fmpz_vec_clear(reg,DATA_BITS);
	fhe_int_clear(temp);
	fhe_int_clear(temp2);
	fhe_int_clear(sel);
	fhe_int_clear(nsel);
	//fhe_int_clear(nrw);

	//gettimeofday(&end, 0);

	return clock()-_clock;
}
clock_t accessr_thr3(_fhe_int *adr,_fhe_int *data,fhe_pk_t pk,fhe_sk_t sk,int x,int y)
{
	//timeval start, end;
	clock_t _clock;
	// adr[0] is lsb
	fhe_int_t sel,nsel;
	fhe_int_t temp,temp2;
	//fhe_int_t nrw;
	_fhe_int *nadr;
	//fmpz *reg;
	//gettimeofday(&start, 0);
	_clock=clock();

	fhe_int_init(temp);
	fhe_int_init(temp2);
	fhe_int_init(sel);
	//fhe_int_init(nrw);
	fhe_int_init(nsel);

	//fhe_int_not(nrw,rw,pk);

	//generate inverse address bits
	nadr=vec_init(ADR_BITS);


	for(int i=0;i<ADR_BITS;i++)
	{
		fhe_int_not(&nadr[i],&adr[i],pk);
	}

	//reg=_fmpz_vec_init(DATA_BITS);

	//printf("reg ");
	//for every row...
	for(int a=x;a<y;a++)
	{
		//printf(" %d(",a);
		fhe_int_encrypt(sel,pk,1);

		//...generate select signal (is a the address in adr?)
		for(int i=0,mask=1;i<ADR_BITS;i++,mask*=2)
		{
			fhe_int_and(sel,sel,((a&mask)>0)?&adr[i]:&nadr[i],pk);
		}
		fhe_int_not(nsel,sel,pk);

		//printf("row %d %d\n",a,fhe_decrypt(sel->mpz,sk));

		for(int i=0;i<WORD_SIZE;i++)
		{
			fhe_int_and(temp,&data[i],nsel,pk);
			fhe_int_and(temp2,&access_array[a*WORD_SIZE+i],sel,pk);
			fhe_int_xor(&data[i],temp,temp2,pk);
			//printf("%d ",(&data[i])->noise);
		}
		//printf(")");
	}
	//printf("\n");
	vec_clear(nadr,ADR_BITS);
	//_fmpz_vec_clear(reg,DATA_BITS);
	fhe_int_clear(temp);
	fhe_int_clear(temp2);
	fhe_int_clear(sel);
	fhe_int_clear(nsel);
	//fhe_int_clear(nrw);

	//gettimeofday(&end, 0);

	return clock()-_clock;
}
// implements oblivious memory access on memory bank 0
//
clock_t accessr0(_fhe_int *adr,_fhe_int *data,fhe_int_t *row,fhe_pk_t pk,fhe_sk_t sk)
{
	//timeval start, end;
	clock_t _clock;
	// adr[0] is lsb
	fhe_int_t sel,nsel;
	fhe_int_t temp,temp2;
	//fhe_int_t nrw;
	_fhe_int *nadr;
	//fmpz *reg;
	//gettimeofday(&start, 0);
	_clock=clock();

	fhe_int_init(temp);
	fhe_int_init(temp2);
	fhe_int_init(sel);
	//fhe_int_init(nrw);
	fhe_int_init(nsel);

	fhe_int_encrypt(*row,pk,0); //row signal aus bank 0

	//fhe_int_not(nrw,rw,pk);

	//generate inverse address bits
	nadr=vec_init(ADR_BITS);


	for(int i=0;i<ADR_BITS;i++)
	{
		fhe_int_not(&nadr[i],&adr[i],pk);
	}

	//reg=_fmpz_vec_init(DATA_BITS);

	//printf("reg ");
	//for every row...
	for(int a=0;a<ROWS/2;a++)
	{
		//printf(" %d(",a);
		fhe_int_encrypt(sel,pk,1);

		//...generate select signal (is a the address in adr?)
		for(int i=0,mask=1;i<ADR_BITS;i++,mask*=2)
		{
			fhe_int_and(sel,sel,((a&mask)>0)?&adr[i]:&nadr[i],pk);
		}
		fhe_int_not(nsel,sel,pk);
		fhe_int_xor(*row,*row,sel,pk);

		printf("bank0:rowsel%d=%d (row=%d)\n",a,fhe_decrypt(sel->mpz,sk),fhe_decrypt((*row)->mpz,sk));

		//printf("row %d %d\n",a,fhe_decrypt(sel->mpz,sk));

		for(int i=0;i<WORD_SIZE;i++)
		{
			fhe_int_and(temp,&data[i],nsel,pk);
			fhe_int_and(temp2,&access_array[a*WORD_SIZE+i],sel,pk);
			fhe_int_xor(&data[i],temp,temp2,pk);
			//printf("%d ",(&data[i])->noise);
		}
		//printf(")");
	}
	//printf("\n");
	vec_clear(nadr,ADR_BITS);
	//_fmpz_vec_clear(reg,DATA_BITS);
	fhe_int_clear(temp);
	fhe_int_clear(temp2);
	fhe_int_clear(sel);
	fhe_int_clear(nsel);
	//fhe_int_clear(nrw);

	//gettimeofday(&end, 0);

	return clock()-_clock;
}

// implements oblivious memory access on memory bank 1
//
clock_t accessr1(_fhe_int *adr,_fhe_int *data,fhe_int_t *row,fhe_pk_t pk,fhe_sk_t sk)
{
	//timeval start, end;
	clock_t _clock;
	// adr[0] is lsb
	fhe_int_t sel,nsel;
	fhe_int_t temp,temp2;
	//fhe_int_t nrw;
	_fhe_int *nadr;
	//fmpz *reg;
	//gettimeofday(&start, 0);
	_clock=clock();

	fhe_int_init(temp);
	fhe_int_init(temp2);
	fhe_int_init(sel);
	//fhe_int_init(nrw);
	fhe_int_init(nsel);

	fhe_int_encrypt(*row,pk,0); //row signal aus bank 1
	//fhe_int_not(nrw,rw,pk);

	//generate inverse address bits
	nadr=vec_init(ADR_BITS);


	for(int i=0;i<ADR_BITS;i++)
	{
		fhe_int_not(&nadr[i],&adr[i],pk);
	}

	//reg=_fmpz_vec_init(DATA_BITS);

	//printf("reg ");
	//for every row...
	for(int a=(ROWS/2)+1;a<ROWS;a++)
	{
		//printf(" %d(",a);
		fhe_int_encrypt(sel,pk,1);

		//...generate select signal (is a the address in adr?)
		for(int i=0,mask=1;i<ADR_BITS;i++,mask*=2)
		{
			fhe_int_and(sel,sel,((a&mask)>0)?&adr[i]:&nadr[i],pk);
		}
		fhe_int_not(nsel,sel,pk);
		fhe_int_xor(*row,*row,sel,pk);

		printf("bank1:rowsel%d=%d (row=%d)\n",a,fhe_decrypt(sel->mpz,sk),fhe_decrypt((*row)->mpz,sk));

		//printf("row %d %d\n",a,fhe_decrypt(sel->mpz,sk));

		for(int i=0;i<WORD_SIZE;i++)
		{
			fhe_int_and(temp,&data[i],nsel,pk);
			fhe_int_and(temp2,&access_array[a*WORD_SIZE+i],sel,pk);
			fhe_int_xor(&data[i],temp,temp2,pk);
			//printf("%d ",(&data[i])->noise);
		}
		//printf(")");
	}
	//printf("\n");
	vec_clear(nadr,ADR_BITS);
	//_fmpz_vec_clear(reg,DATA_BITS);
	fhe_int_clear(temp);
	fhe_int_clear(temp2);
	fhe_int_clear(sel);
	fhe_int_clear(nsel);
	//fhe_int_clear(nrw);

	//gettimeofday(&end, 0);

	return clock()-_clock;
}


clock_t accessw(/*fhe_int_t rw,*/_fhe_int *adr,_fhe_int *data,fhe_pk_t pk,fhe_sk_t sk)
{
	//timeval start, end;
	clock_t _clock;
	// adr[0] is lsb
	fhe_int_t sel,nsel;
	fhe_int_t temp,temp2;
	//fhe_int_t nrw;
	_fhe_int *nadr;
	//fmpz *reg;
	//gettimeofday(&start, 0);
	_clock=clock();

	fhe_int_init(temp);
	fhe_int_init(temp2);
	fhe_int_init(sel);
	//fhe_int_init(nrw);
	fhe_int_init(nsel);

	//fhe_int_not(nrw,rw,pk);

	//generate inverse address bits
	nadr=vec_init(ADR_BITS);


	for(int i=0;i<ADR_BITS;i++)
	{
		fhe_int_not(&nadr[i],&adr[i],pk);
	}

	//reg=_fmpz_vec_init(DATA_BITS);
	printf("W \n");
	//for every row...
	for(int a=0;a<ROWS;a++)
	{
		printf("R%d ",a);
		fhe_int_encrypt(sel,pk,1);

		//...generate select signal (is a the address in adr?)
		for(int i=0,mask=1;i<ADR_BITS;i++,mask*=2)
		{
			fhe_int_and(sel,sel,((a&mask)>0)?&adr[i]:&nadr[i],pk);
		}
		fhe_int_not(nsel,sel,pk);

		for(int i=0;i<WORD_SIZE;i++)
		{
			printf("W%d ",i);
			//set memory
			fhe_int_and(temp,&data[i],sel,pk);
			fhe_int_and(temp2,&access_array[a*WORD_SIZE+i],nsel,pk);
			fhe_int_xor(&access_array[a*WORD_SIZE+i],temp,temp2,pk);

		}
		printf("\n");
	}

	vec_clear(nadr,ADR_BITS);
	//_fmpz_vec_clear(reg,DATA_BITS);
	fhe_int_clear(temp);
	fhe_int_clear(temp2);
	fhe_int_clear(sel);
	fhe_int_clear(nsel);
	//fhe_int_clear(nrw);

	//gettimeofday(&end, 0);

	return clock()-_clock;
}

_fhe_int* vec_init(int size)
{
	_fhe_int *temp=malloc(sizeof(_fhe_int)*size);
	for(int i=0;i<size;i++)
		fhe_int_init(&temp[i]);
	return temp;
}

void vec_clear(_fhe_int *vec,int size)
{
	for(int i=0;i<size;i++)
		fhe_int_clear(&vec[i]);
	free(vec);
}
