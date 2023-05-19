//
// Created by Gaetan on 2022-11-13.
//

#ifndef SERVER__BYTES_H
#define SERVER__BYTES_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

typedef unsigned char byte;
typedef unsigned long long int ull;
typedef size_t st;
typedef struct {
    byte* data;
    st size;
} bytes;

typedef struct {
    ull* data;
    st size;
    st alloc;
} num;

bytes convert_num(const num* n);

typedef struct {
    void (* add)(num* res, num a, num b);

    num* (* double_bytes)(num* res, num a);

    num (* to_num)(unsigned long long n);

    unsigned long long (* to_ull)(num a);

    void (* print)(num b);

    num (* mod)(num res, num a, num b);

    num (* pow_mod)(num res, num a, num b, num c);

    bytes (* mul)(bytes a, bytes b);

    bytes (* mul2)(bytes a, bytes b);

    num* (* sub)(num* res, num a, num b);

    int (* sup)(num a, num b);

    num* (* half)(num* res, num a);

    int (* equals)(num a, num b);

    bytes (* random_bytes)(st size, long int* seed);

    num (* hex_to_bytes)(const char* hex);

    int (* is_prime)(const bytes n, long int* seed);

    void (* free)(num* n);

    num* (*init_num)(num* b);

    long int seed;

} num_lib;

num_lib* bytes_lib();

#endif //SERVER__BYTES_H
