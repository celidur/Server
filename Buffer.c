//
// Created by Gaetan on 2023-05-19.
//

#include "Buffer.h"

char *get_buffer(unsigned long long int size) {
    char *buffer = malloc(size);
    if (buffer == NULL) {
        return NULL;
    }
    memset(buffer, 0, size);
    return buffer;
}

char *split(char *str, char delim) {
    int i;
    char *token;
    int end = 0;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == delim) {
            end = i;
            i++;
            break;
        }
    }
    if (end == 0) {
        return NULL;
    }
    token = get_buffer(strlen(str) - i);
    for (int j = i; j < strlen(str); ++j) {
        token[j - i] = str[j];
    }
    str[end] = '\0';
    return token;
}


char *IntToString(unsigned int number) {
    char *buffer = get_buffer(10 * sizeof(char));
    char digit[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    int i = 0;
    do {
        buffer[i] = digit[number % 10];
        number /= 10;
        i++;
    } while (number > 0);
    buffer[i] = '\0';
    for (int j = 0; j < i; ++j) {
        char temp = buffer[j];
        if (i - j - 1 < j)
            break;
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = temp;
    }
    return buffer;
}
