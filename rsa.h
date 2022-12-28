#pragma once
#ifndef RSA_H

#include <stdlib.h>
#include "Bytes.h"

bytes crypt(bytes m, bytes key_0, bytes key_1);

int is_prime(bytes n);


#endif
#define RSA_H