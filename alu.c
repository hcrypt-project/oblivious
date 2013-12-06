/*
 * alu.c
 *
 *  Created on: 17.03.2012
 *      Author: michaelbrenner
 */

/* this ALU has 2-bit opcodes
 *
 * 00 or
 * 01 xor
 * 10 and
 * 11 add
 */

#include "alu.h"

clock_t alu3(_fhe_int *into,_fhe_int *opcode,fhe_int_t carry,
		_fhe_int *op1, _fhe_int *op2,fhe_pk_t pk,fhe_sk_t sk)
{
	clock_t _clock;
	_fhe_int *or;
	_fhe_int *xor;
	_fhe_int *and;
	_fhe_int *add;
	_fhe_int *nopcode;
	fhe_int_t temp;
	fhe_int_t sel_or;
	fhe_int_t sel_xor;
	fhe_int_t sel_and;
	fhe_int_t sel_add;

	_clock=clock();

	nopcode=vec_init(2);
	for(int i=0;i<2;i++)
	{
		fhe_int_not(&nopcode[i],&opcode[i],pk);
	}

	or=vec_init(DATA_BITS);
	xor=vec_init(DATA_BITS);
	and=vec_init(DATA_BITS);
	add=vec_init(DATA_BITS+1);
	fhe_int_init(temp);
	fhe_int_init(sel_or);
	fhe_int_init(sel_xor);
	fhe_int_init(sel_and);
	fhe_int_init(sel_add);

	for(int i=0;i<DATA_BITS;i++)
		fhe_int_encrypt(&into[i],pk,0);

	fhe_int_and(sel_or,&nopcode[0],&nopcode[1],pk);
	fhe_int_and(sel_xor,&opcode[0],&nopcode[1],pk);
	fhe_int_and(sel_and,&nopcode[0],&opcode[1],pk);
	fhe_int_and(sel_add,&opcode[0],&opcode[1],pk);

	for(int i=0;i<DATA_BITS;i++)
	{
		fhe_int_xor(&xor[i],&op1[i],&op2[i],pk); //xor
		fhe_int_and(&and[i],&op1[i],&op2[i],pk); //and
		fhe_int_xor(&or[i],&xor[i],&and[i],pk);  //or

		fhe_int_xor(&add[i],&xor[i],carry,pk);   //rc-add
		fhe_int_and(temp,&xor[i],carry,pk);
		fhe_int_or(carry,temp,&and[i],pk);

		fhe_int_and(temp,&or[i],sel_or,pk);		 //select output
		fhe_int_xor(&into[i],&into[i],temp,pk);
		fhe_int_and(temp,&xor[i],sel_xor,pk);
		fhe_int_xor(&into[i],&into[i],temp,pk);
		fhe_int_and(temp,&and[i],sel_and,pk);
		fhe_int_xor(&into[i],&into[i],temp,pk);
		fhe_int_and(temp,&add[i],sel_add,pk);
		fhe_int_xor(&into[i],&into[i],temp,pk);
	}

	vec_clear(or,DATA_BITS);
	vec_clear(xor,DATA_BITS);
	vec_clear(and,DATA_BITS);
	vec_clear(add,DATA_BITS);
	fhe_int_clear(temp);
	fhe_int_clear(sel_or);
	fhe_int_clear(sel_xor);
	fhe_int_clear(sel_and);
	fhe_int_clear(sel_add);

	return clock()-_clock;
}
