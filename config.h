//
// Created by zhang on 2019/6/5.
//

#ifndef HGA_CONFIG_H
#define HGA_CONFIG_H

const int SEED = 1;
const int POP_NUM = 500;
const double CROSS_RATE = .9;
const double MUTATE_RATE = .05;
const int MAX_FIT = 1<<30;
const int HASH_SIZE = 1<<24;
const int THREAD_NUM = 4;
const int MAX_GEN = 1e6;
//#ifndef SEED
//#define SEED 1
//#endif
//
//#ifndef POP_NUM
//#define POP_NUM 500
//#endif
//
//#ifndef CROSS_RATE
//#define CROSS_RATE 0.9
//#endif
//
//#ifndef MUTATE_RATE
//#define MUTATE_RATE 0.05
//#endif
//
//#ifndef MAX_FIT
//#define MAX_FIT (1<<30)
//#endif
//
//#ifndef MAX_GEN
//#define MAX_GEN 10000000
//#endif
//
//#ifndef HASH_SIZE
//#define HASH_SIZE (1<<24)
//#endif
//
//#ifndef THREAD_NUM
//#define THREAD_NUM 4
//#endif

#endif //HGA_CONFIG_H
