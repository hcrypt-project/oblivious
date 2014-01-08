/*
 * alu.h
 *
 *  Created on: 17.03.2012
 *      Author: michaelbrenner
 */

#ifndef ALU_H_
#define ALU_H_

#include "kit.h"
#include "access.h"
#include "trans.h"
#include "integer-fhe.h"
#include <time.h>

#define OPCODE_OR 0
#define OPCODE_XOR 1
#define OPCODE_AND 2
#define OPCODE_ADD 3

clock_t alu3(_fhe_int *into,_fhe_int *opcode,fhe_int_t carry,
		_fhe_int *op1, _fhe_int *op2,fhe_pk_t pk,fhe_sk_t sk);

#endif /* ALU_H_ */
