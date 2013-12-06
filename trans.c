/*
 * trans.c
 *
 *  Created on: 13.03.2012
 *      Author: michaelbrenner
 */
#include "trans.h"

void trans_int2adr(int i,_fhe_int *adr,fhe_pk_t pk)
{
	for(int a=0,mask=1;a<ADR_BITS;a++,mask*=2)
	{
		if(i&mask)
			fhe_int_encrypt(&adr[a],pk,1);
		else
			fhe_int_encrypt(&adr[a],pk,0);

		(&adr[a])->noise=1;
	}
}

void trans_adr2int(int i,_fhe_int *adr,fhe_pk_t pk)
{

}

void trans_int2data(int i,_fhe_int *data,fhe_pk_t pk)
{
	for(int a=0,mask=1;a<DATA_BITS;a++,mask*=2)
	{
		if(i&mask)
			fhe_int_encrypt(&data[a],pk,1);
		else
			fhe_int_encrypt(&data[a],pk,0);

		(&data[a])->noise=1;
	}
}

void trans_int2vec(int i,_fhe_int *data,int size,fhe_pk_t pk)
{
	for(int a=0,mask=1;a<size;a++,mask*=2)
	{
		if(i&mask)
			fhe_int_encrypt(&data[a],pk,1);
		else
			fhe_int_encrypt(&data[a],pk,0);

		(&data[a])->noise=1;
	}
}

void trans_data2int(int i,_fhe_int *data,fhe_pk_t pk)
{

}

void trans_int2mem(int i,int memaddr,fhe_pk_t pk)
{
	trans_int2vec(i,&access_array[WORD_SIZE*memaddr],WORD_SIZE,pk);
}

void trans_print(_fhe_int *data,int len,fhe_sk_t sk)
{
	for(int i=0;i<len;i++)
		printf("%d ",fhe_decrypt((&data[i])->mpz,sk));
}
