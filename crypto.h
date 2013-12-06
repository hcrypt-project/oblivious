/*
 * crypto.h
 *
 *  Created on: 13.03.2012
 *      Author: michaelbrenner
 */

#ifndef CRYPTO_H_
#define CRYPTO_H_

#include <integer-fhe.h>

void crypto_xor(mpz_t into,mpz_t a,mpz_t b,fhe_pk_t pk);

void crypto_and(mpz_t into,mpz_t a,mpz_t b,fhe_pk_t pk);

void crypto_add(mpz_t into,mpz_t a,mpz_t b,fhe_pk_t pk);

void crypto_mul(mpz_t into,mpz_t a,mpz_t b,fhe_pk_t pk);

void crypto_or(mpz_t into,mpz_t a,mpz_t b,fhe_pk_t pk);

void crypto_not(mpz_t into,mpz_t a,fhe_pk_t pk);

#endif /* CRYPTO_H_ */
