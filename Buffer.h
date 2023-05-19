//
// Created by Gaetan on 2023-05-19.
//

#ifndef SERVER_BUFFER_H
#define SERVER_BUFFER_H

#include <stdlib.h>
#include <string.h>


char *get_buffer(unsigned long long int size);

char *IntToString(unsigned int number);

char *split(char *str, char delim);

#endif //SERVER_BUFFER_H
