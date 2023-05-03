// Copyright (c) 2022 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _MSGA_TEST_H
#define _MSGA_TEST_H

#include <stdio.h>
#include <stdlib.h>
#define ASSERT(NAME,COND) if(!(COND)){fprintf(stderr,"%s:%d: TEST FAIL:%s:%s\n",__FILE__,__LINE__,NAME,#COND);exit(-1);}
#define TEST(X) ASSERT("Should be true",X)
#define TEST_F(X) ASSERT("Should be false",X==false)
#define TEST_EQ(X,Y) ASSERT("Should be equals",X==Y)
#define TEST_NEQ(X,Y) ASSERT("Should be not equals",X==Y)

#define SHOW_FUNC(X) printf("[%s] %p\n",#X,X);
#define WAIT_FOR_USER(MSG) {printf("[WAIT] %s\n",#MSG);getchar();}

#define SHOW_ARGS(ARGC,ARGV) for(int i=0;i<ARGC;++i)fprintf(stderr,"argv[%d]=%s\n",i,ARGV[i])

#endif //_MSGA_TEST_H