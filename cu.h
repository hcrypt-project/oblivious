/*
 * cu.h
 *
 *  Created on: 19.03.2012
 *      Author: michaelbrenner
 */

#ifndef CU_H_
#define CU_H_

#include "access.h"
#include "alu.h"
#include "trans.h"
#include "crypto.h"
#include <time.h>

clock_t cu(fhe_pk_t pk,fhe_sk_t sk);

#endif /* CU_H_ */
