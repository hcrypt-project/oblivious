/*
 * access.h
 *
 *  Created on: 13.03.2012
 *      Author: michaelbrenner
 */

#ifndef ACCESS_H_
#define ACCESS_H_

#include "kit.h"
#include "integer-fhe.h"
#include <stdio.h>
#include <time.h>

#define RUNS 10
#define WORD_SIZE 11
#define DATA_BITS 8
#define ADR_BITS 5
// ROWS = 2^ADR_BITS
#define ROWS 32
#define READ 1
#define WRITE 0

int access_array_noises[RUNS][ROWS*WORD_SIZE];
int access_reg_noises[RUNS];
_fhe_int* access_array;
_fhe_int* access_rowselect;
int *access_array_noise;

clock_t accessr_thr0(_fhe_int *adr,_fhe_int *data,fhe_pk_t pk,fhe_sk_t sk,int x,int y);
clock_t accessr_thr1(_fhe_int *adr,_fhe_int *data,fhe_pk_t pk,fhe_sk_t sk,int x,int y);
clock_t accessr_thr2(_fhe_int *adr,_fhe_int *data,fhe_pk_t pk,fhe_sk_t sk,int x,int y);
clock_t accessr_thr3(_fhe_int *adr,_fhe_int *data,fhe_pk_t pk,fhe_sk_t sk,int x,int y);
clock_t accessr(/*fhe_int_t rw*/_fhe_int *adr, _fhe_int *data,fhe_pk_t pk,fhe_sk_t sk);
clock_t accessr0(/*fhe_int_t rw*/_fhe_int *adr, _fhe_int *data,fhe_int_t *row,fhe_pk_t pk,fhe_sk_t sk);
clock_t accessr1(/*fhe_int_t rw*/_fhe_int *adr, _fhe_int *data,fhe_int_t *row,fhe_pk_t pk,fhe_sk_t sk);
clock_t accessw(/*fhe_int_t rw*/_fhe_int *adr, _fhe_int *data,fhe_pk_t pk,fhe_sk_t sk);
void access_init(int rows,int cols,fhe_pk_t pk);
void access_clear(int rows,int cols);
void access_dump(int rows,int cols,fhe_sk_t sk);
void access_set(fhe_int_t m,int i,fhe_pk_t pk);
_fhe_int* vec_init(int size);
void vec_clear(_fhe_int *vec,int size);

#endif /* ACCESS_H_ */
