/*
 * cu.c
 *
 *  Created on: 19.03.2012
 *      Author: michaelbrenner
 */

#include "cu.h"
extern clock_t _mem;

clock_t cu(fhe_pk_t pk,fhe_sk_t sk)
{
	clock_t _clock;
	int brk=0;
	_fhe_int *ac,*temp,*temp2,*alu_i,*alu_a;
	_fhe_int *pc;
	fhe_int_t carry;
	fhe_int_t zero;
	fhe_int_t read;
	fhe_int_t write;

	access_init(ROWS,WORD_SIZE,pk);

	fhe_int_init(carry);
	fhe_int_init(zero);
	fhe_int_init(read);
	fhe_int_init(write);
	ac=vec_init(DATA_BITS);
	pc=vec_init(ADR_BITS);
	temp=vec_init(WORD_SIZE);
	temp2=vec_init(WORD_SIZE);
	alu_i=vec_init(DATA_BITS);
	alu_a=vec_init(DATA_BITS);
	fhe_int_encrypt(read,pk,READ);
	fhe_int_encrypt(write,pk,WRITE);

	trans_int2data(3,ac,pk);
	trans_int2adr(0,pc,pk);
	trans_int2vec(0,temp,WORD_SIZE,pk);
	trans_int2vec(0,temp2,WORD_SIZE,pk);

	trans_int2mem(3+8,0,pk);
	trans_int2mem(16,1,pk);

	//access_dump(ROWS,WORD_SIZE,sk);

	_clock=clock();

	while(!brk)
	{
		clock_t c=0;
		//FETCH immediate
		c=accessr(pc,temp,pk,sk);
		//printf("CU access i = ");trans_print(temp,WORD_SIZE,sk);printf("\n");
		printf("m1=%ld ",c/CLOCKS_PER_SEC);fflush(stdout);
		_mem+=c;
		//printf("CU addr a   = ");trans_print(&temp[3],ADR_BITS,sk);printf("\n");
		//FETCH absolute
		c=accessr(&temp[3],temp2,pk,sk);
		//printf("CU access a = ");trans_print(temp2,WORD_SIZE,sk);printf("\n");
		printf("m2=%ld ",c/CLOCKS_PER_SEC);fflush(stdout);
		_mem+=c;
		//ALU immediate
		alu3(alu_i,temp,carry,ac,&temp[3],pk,sk);
		//printf("CU alu i    = ");trans_print(alu_i,DATA_BITS,sk);printf("\n");

		//ALU absolute
		alu3(alu_a,temp,carry,ac,&temp2[3],pk,sk);
		//printf("CU alu a    = ");trans_print(alu_a,DATA_BITS,sk);printf("\n");

		c=accessw(&temp[3],temp2,pk,sk);
		printf("m3=%ld ",c/CLOCKS_PER_SEC);fflush(stdout);
		_mem+=c;

		brk=1;
	}
	_clock=clock()-_clock;
	printf("%ld",_clock/CLOCKS_PER_SEC);

	fhe_int_clear(carry);
	fhe_int_clear(zero);
	fhe_int_clear(read);
	fhe_int_clear(write);
	vec_clear(ac,DATA_BITS);
	vec_clear(pc,ADR_BITS);
	vec_clear(temp,DATA_BITS);
	vec_clear(temp2,DATA_BITS);
	vec_clear(alu_i,DATA_BITS);
	vec_clear(alu_a,DATA_BITS);

	return _clock;
}
