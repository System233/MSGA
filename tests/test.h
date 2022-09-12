// Copyright (c) 2022 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _MSGA_TEST_H
#define _MSGA_TEST_H

#include <stdio.h>
#include <stdlib.h>
#define ASSERT(COND) if(!(COND)){fprintf(stderr,"ASSERT FAIL:"#COND);abort();}
#endif //_MSGA_TEST_H