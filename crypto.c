/*
 * crypto.c
 *
 *  Created on: 13.03.2012
 *      Author: michaelbrenner
 */

#include "crypto.h"

void crypto_add(mpz_t into,mpz_t a,mpz_t b,fhe_pk_t pk)
{
	fhe_add(into,a,b,pk);
}

void crypto_mul(mpz_t into,mpz_t a,mpz_t b,fhe_pk_t pk)
{
	fhe_mul(into,a,b,pk);
}

void crypto_xor(mpz_t into,mpz_t a,mpz_t b,fhe_pk_t pk)
{
	fhe_add(into,a,b,pk);
}

void crypto_and(mpz_t into,mpz_t a,mpz_t b,fhe_pk_t pk)
{
	fhe_mul(into,a,b,pk);
}

void crypto_or(mpz_t into,mpz_t a,mpz_t b,fhe_pk_t pk)
{
	mpz_t temp;
	mpz_init(temp);

	fhe_mul(temp,a,b,pk);
	fhe_add(into,a,b,pk);
	fhe_add(into,into,temp,pk);

	mpz_clear(temp);
}

void crypto_not(mpz_t into,mpz_t a,fhe_pk_t pk)
{
	mpz_t one;
	mpz_init(one);

	fhe_encrypt(one,pk,1);
	fhe_add(into,a,one,pk);

	mpz_clear(one);
}
