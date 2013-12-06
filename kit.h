/*
 * kit.h
 *
 *  Created on: 24.02.2012
 *      Author: michaelbrenner
 */

#ifndef KIT_H_
#define KIT_H_

#define NOISE_THRESHOLD 7 //37
#define NOISE_RESET 1

#include <stdio.h>
#include "integer-fhe.h"

typedef struct {
	mpz_t mpz;
	int noise;
} _fhe_int;

typedef _fhe_int fhe_int_t[1];

void fhe_lib_init(fhe_sk_t *sk2);

void fhe_int_init(fhe_int_t i);

void fhe_int_clear(fhe_int_t i);

void fhe_int_set(fhe_int_t to,fhe_int_t from);

/** I/O **/
void fhe_pk_store(fhe_pk_t pk,char *filename);

void fhe_sk_store(fhe_sk_t sk,char *filename);

int fhe_pk_load(fhe_pk_t pk,char *filename);

int fhe_sk_load(fhe_sk_t sk,char *filename);

void fhe_int_add(fhe_int_t res, fhe_int_t a, fhe_int_t b, fhe_pk_t pk);

void fhe_int_mul(fhe_int_t res, fhe_int_t a, fhe_int_t b, fhe_pk_t pk);

void fhe_autorecrypt(fhe_int_t a,fhe_pk_t pk);

void fhe_int_xor(fhe_int_t res,fhe_int_t a,fhe_int_t b,fhe_pk_t pk);

void fhe_int_or(fhe_int_t res,fhe_int_t a,fhe_int_t b,fhe_pk_t pk);

void fhe_int_and(fhe_int_t res,fhe_int_t a,fhe_int_t b,fhe_pk_t pk);

void fhe_int_not(fhe_int_t res,fhe_int_t a,fhe_pk_t pk);

void fhe_int_encrypt(fhe_int_t res,fhe_pk_t pk,int m);

#endif /* KIT_H_ */
