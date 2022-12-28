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
typedef size_t st;
typedef struct {
    byte *data;
    st size;
} bytes;

typedef struct {
    bytes (*add)(bytes a, bytes b);

    bytes (*double_bytes)(bytes a);

    bytes (*to_bytes)(unsigned long long n);

    unsigned long long (*to_ull)(bytes a);

    void (*print)(bytes b);

    bytes (*mod)(bytes a, bytes b);

    bytes (*pow_mod)(bytes a, bytes b, bytes c);

    bytes (*mul)(bytes a, bytes b);

    bytes (*sub)(bytes a, bytes b);

    int (*sup)(bytes a, bytes b);

    bytes (*half)(bytes a);

    int (*equals)(bytes a, bytes b);

    bytes (*random_bytes)(st size, long int *seed);

    bytes (*hex_to_bytes)(const char *hex);

    long int seed;
} Bytes_lib;

Bytes_lib *bytes_lib();

#endif //SERVER__BYTES_H
