#include <stdio.h>
#include <stdlib.h>
#include "rsa.h"
#include <time.h>

bytes to_bytes(const unsigned long long n) {
    st size = 1;
    unsigned long long tmp = n;
    while (tmp /= 256)
        size++;
    byte *data = malloc(size * sizeof(byte));
    tmp = n;
    for (st i = 0; i < size; i++) {
        data[i] = tmp % 256;
        tmp /= 256;
    }
    return (bytes) {data, size};
}

unsigned long long to_ull(const bytes a) {
    unsigned long long n = 0;
    unsigned long long tmp = 1;
    for (st i = 0; i < a.size; i++) {
        n += a.data[i] * tmp;
        tmp <<= 8;
    }
    return n;
}

void print_bytes(bytes b) {
    for (st i = b.size - 1; i < b.size; i--)
        printf("%02x", b.data[i]);
}

bytes double_bytes(const bytes a) {
    bytes res = {NULL, a.size + 1};
    res.data = malloc(res.size * sizeof(byte));
    for (st i = 0; i < res.size; i++) {
        int temp = a.data[i] << 1;
        res.data[i] += temp % 256;
        if (i + 1 < res.size)
            res.data[i + 1] += temp / 256;
    }
    while (res.size > 1 && res.data[res.size - 1] == 0) res.size--;
    res.data = realloc(res.data, res.size * sizeof(byte));
    return res;
}

bytes add(const bytes a, const bytes b) {
    st size = (a.size > b.size ? a.size : b.size) + 1;
    byte *data = malloc(size * sizeof(byte));
    for (st i = 0; i < size; i++)
        data[i] = 0;
    for (st i = 0; i < size; i++) {
        int d = data[i] + (i < a.size ? a.data[i] : 0) + (i < b.size ? b.data[i] : 0);
        data[i] = d % 256;
        d /= 256;
        int k = 1;
        while (d > 0) {
            d = data[i + k] + d;
            data[i + k] = d % 256;
            d /= 256;
        }
    }
    return (bytes) {data, size};
}

bytes mul(const bytes a, const bytes b) {
    bytes res = {NULL, a.size + b.size};
    byte *clear;
    res.data = malloc(res.size * sizeof(byte));
    for (st i = 0; i < res.size; i++)
        res.data[i] = 0;
    for (st i = 0; i < a.size; i++) {
        bytes tmp = {NULL, b.size + 1};
        tmp.data = malloc(tmp.size * sizeof(byte));
        for (st j = 0; j < b.size; j++) {
            int t = a.data[i] * b.data[j] + tmp.data[j];
            tmp.data[j] = t % 256;
            tmp.data[j + 1] = t / 256;
        }
        for (int j = 0; j < 8 * i; ++j) {
            clear = tmp.data;
            tmp = double_bytes(tmp);
            free(clear);
        }
        clear = res.data;
        res = add(res, tmp);
        free(tmp.data);
        free(clear);
    }
    while (res.data[res.size - 1] == 0 && res.size > 1) res.size--;
    res.data = realloc(res.data, res.size * sizeof(byte));
    return res;
}

bytes half(const bytes a) {
    bytes res = {NULL, a.size};
    res.data = malloc(res.size * sizeof(byte));
    for (st i = 0; i < res.size; i++) {
        res.data[i] = a.data[i] >> 1;
        if (i + 1 < res.size)
            res.data[i] += a.data[i + 1] << 7;
    }
    while (res.size > 1 && res.data[res.size - 1] == 0) res.size--;
    res.data = realloc(res.data, res.size * sizeof(byte));
    return res;

}



int sup(const bytes a, const bytes b) {
    //a > b
    if (a.size > b.size)
        return 1;
    if (a.size < b.size)
        return 0;
    for (st i = a.size - 1; i < a.size; i--) {
        if (a.data[i] > b.data[i])
            return 1;
        if (a.data[i] < b.data[i])
            return 0;
    }
    return 0;

}

bytes sub(const bytes a, const bytes b) {
    if (sup(b, a)) {
        printf("Error: a < b\n");
        return (bytes) {NULL, 0};
    }
    bytes res = {NULL, a.size > b.size ? a.size : b.size};
    res.data = malloc(res.size * sizeof(byte));
    for (st i = 0; i < res.size; i++) {
        res.data[i] = a.data[i];
    }
    for (st i = 0; i < b.size; i++) {
        int temp = res.data[i] - b.data[i];
        if (temp < 0) {
            temp += 256;
            st j = i + 1;
            while (res.data[j] == 0) {
                res.data[j] = 255;
                j++;
            }
            res.data[j]--;
        }
        res.data[i] = temp;
    }
    while (res.size > 1 && res.data[res.size - 1] == 0) res.size--;
    res.data = realloc(res.data, res.size * sizeof(byte));
    return res;
}

bytes mod(const bytes a, bytes b) {
    byte *temp = NULL;
    bytes x = {NULL, b.size};
    x.data = malloc(x.size * sizeof(byte));
    for (st i = 0; i < x.size; i++) {
        x.data[i] = b.data[i];
    }
    bytes res = {NULL, a.size};
    res.data = malloc(res.size * sizeof(byte));
    for (st i = 0; i < res.size; i++) {
        res.data[i] = a.data[i];
    }
    bytes y = half(a);
    while (sup(x, y) == 0) {
        temp = x.data;
        x = double_bytes(x);
        free(temp);
    }
    free(y.data);
    while (sup(b, res) == 0) {
        if (sup(x, res) == 0) {
            temp = res.data;
            res = sub(res, x);
            free(temp);
        }
        temp = x.data;
        x = half(x);
        free(temp);
    }
    free(x.data);
    res.data = realloc(res.data, res.size * sizeof(byte));
    return res;
}

bytes pow_mod(const bytes b, const bytes e, const bytes m) {
    if (m.size == 1 && m.data[0] == 0)
        return to_bytes(0);
    byte *temp = NULL;
    bytes res = {NULL, 1};
    bytes clear;
    res.data = malloc(res.size * sizeof(byte));
    res.data[0] = 1;
    bytes base = mod(b, m);
    bytes exp = {NULL, e.size};
    exp.data = malloc(exp.size * sizeof(byte));
    for (st i = 0; i < exp.size; i++)
        exp.data[i] = e.data[i];
    while (sup(exp, to_bytes(0))) {
        if (exp.data[0] % 2 == 1) {
            temp = res.data;
            clear = mul(res, base);
            res = mod(clear, m);
            free(temp);
            free(clear.data);
        }
        temp = base.data;
        clear = mul(base, base);
        base = mod(clear, m);
        free(clear.data);
        free(temp);
        temp = exp.data;
        exp = half(exp);
        free(temp);
    }
    return res;
}


int main() {
    bytes a = to_bytes(2467327685);
    bytes b = to_bytes(1224214);
    bytes c = to_bytes(23452138);
    print_bytes(pow_mod(a, b, c));
}