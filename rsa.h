#pragma once
#ifndef RSA_H

#include <stdlib.h>
#include <time.h>
#include <gmp.h>


typedef struct {
    mpz_t n;
    mpz_t e;
} public_key;

typedef struct {
    mpz_t n;
    mpz_t d;
} private_key;

typedef struct {
    public_key* pub;
    private_key* priv;
} key_pair;

typedef struct {
    mpz_t msg;
} message;

key_pair generate_key(char* username, char* password, mpz_t n_);

message rsa_encrypt(message msg, public_key key);

message rsa_decrypt(message msg, private_key key);

void clear_keys(key_pair pair);

void clear_message(message msg);


#endif
#define RSA_H