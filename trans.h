/*
 * trans.h
 *
 *  Created on: 13.03.2012
 *      Author: michaelbrenner
 */

#ifndef TRANS_H_
#define TRANS_H_

#include "access.h"
#include "kit.h"
#include "integer-fhe.h"

void trans_int2adr(int i,_fhe_int *adr,fhe_pk_t pk);

void trans_adr2int(int i,_fhe_int *adr,fhe_pk_t pk);

void trans_int2data(int i,_fhe_int *data,fhe_pk_t pk);

void trans_data2int(int i,_fhe_int *data,fhe_pk_t pk);

void trans_int2vec(int i,_fhe_int *data,int size,fhe_pk_t pk);

void trans_print(_fhe_int *data,int len,fhe_sk_t sk);

void trans_int2mem(int i,int memaddr,fhe_pk_t pk);

#endif /* TRANS_H_ */
