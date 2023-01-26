//
// Created by Gaetan on 2022-11-13.
//

#include "Bytes.h"

enum bool{true=1, false=0};

bytes alloc_bytes(st size) {
    bytes b = {NULL, size};
    b.data = malloc(size);
    return b.data == NULL ? (bytes) {NULL, 0} : b;
}

int resize_bytes(bytes* b) {
    while (b->size > 1 && b->data[b->size - 1] == 0) b->size--;
    b->data = realloc(b->data, b->size * sizeof(byte));
    return b->data == NULL ? false : true;
}

bytes to_bytes(const unsigned long long n) {
    st size = 1;
    unsigned long long tmp = n;
    while (tmp /= 256) size++;
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

void print_bytes(const bytes b) {
    for (st i = b.size - 1; i < b.size; i--)
        printf("%02x", b.data[i]);
    printf("\n");
}

static bytes double_bytes(const bytes a) {
    bytes res = alloc_bytes(a.size + 1);
    for (st i = 0; i < res.size; i++) {
        res.data[i] = i == a.size ? 0 : (a.data[i] & 127) << 1; // shift the current byte
        res.data[i] += i == 0 ? 0 : ((a.data[i - 1] & 0x80) >> 7); // add the carry of the previous byte
    }
    return resize_bytes(&res) ? res : (bytes) {NULL, 0};
}

static bytes half(const bytes a) {
    bytes res = alloc_bytes(a.size);
    // shift the current byte and add the carry of the previous bytes in higher position
    for (st i = 0; i < res.size; i++) {
        res.data[i] = ((a.data[i]&254) >> 1) + ((i + 1) == a.size ? 0 : (a.data[i + 1] & 0x1) << 7);
    }
    return resize_bytes(&res) ? res : (bytes) {NULL, 0};
}

static bytes add(const bytes a, const bytes b) {
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
    while (size > 1 && data[size - 1] == 0) size--;
    data = realloc(data, size * sizeof(byte));
    return (bytes) {data, size};
}

static bytes mul(const bytes a, const bytes b) {
    bytes res = alloc_bytes(a.size + b.size);
    byte *clear;
    for (st i = 0; i < res.size; i++)
        res.data[i] = 0;
    for (st i = 0; i < a.size; i++) {
        bytes tmp = alloc_bytes(b.size + 1);
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
    return resize_bytes(&res) ? res : (bytes) {NULL, 0};
}

static int sup(const bytes a, const bytes b) {
    //a > b
    if (a.size > b.size)
        return true;
    if (a.size < b.size)
        return false;
    for (st i = a.size - 1; i < a.size; i--) {
        if (a.data[i] > b.data[i])
            return true;
        if (a.data[i] < b.data[i])
            return false;
    }
    return false;

}

static bytes sub(const bytes a, const bytes b) {
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
    return resize_bytes(&res) ? res : (bytes) {NULL, 0};
}

static bytes mod(const bytes a, const bytes b) {
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
    return resize_bytes(&res) ? res : (bytes) {NULL, 0};
}

static bytes pow_mod(const bytes b, const bytes e, const bytes m) {
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
    return resize_bytes(&res) ? res : (bytes) {NULL, 0};
}

int equals(const bytes a, const bytes b) {
    if (a.size != b.size)
        return false;
    for (st i = 0; i < a.size; i++)
        if (a.data[i] != b.data[i])
            return false;
    return true;
}

void random_int(long int *seed) {
    *seed = (*seed * 1103515245 + 12345) % 2147483648;
}

void shuffle_bytes(bytes *a, long int *seed) {
    for (st i = 0; i < a->size; i++) {
        st j = i + (*seed % (a->size - i));
        byte temp = a->data[i];
        a->data[i] = a->data[j];
        a->data[j] = temp;
    }
    while (a->data[a->size - 1] == 0) {
        a->data[a->size - 1] = *seed % 256;
        if (a->data[a->size - 1] == 0)
            random_int(seed);
    }
}

bytes random_bytes(const st size, long int *seed) {
    bytes res = {NULL, size};
    res.data = malloc(res.size * sizeof(byte));
    long int temp = *seed;
    long int temp2;
    for (st i = 0; i < res.size; i++) {
        temp2 = temp % 256;
        temp >>= 8;
        res.data[i] = temp2;
        if (temp == 0) {
            random_int(seed);
            temp = *seed;
        }
    }
    shuffle_bytes(&res, seed);
    return res;
}


bytes hex_to_bytes(const char *hex) {
    st size = (strlen(hex) + 1) / 2;
    bytes res = {NULL, size};
    res.data = malloc(res.size * sizeof(byte));
    for (st i = 0; i < res.size; i++) {
        char temp[3] = {'0', hex[strlen(hex) - 1 - i * 2], '\0'};
        if (strlen(hex) - 1 - i * 2 != 0)
            temp[0] = hex[strlen(hex) - 2 - i * 2];
        res.data[i] = strtol(temp, NULL, 16);

    }
    return res;
}

Bytes_lib *bytes_lib() {
    Bytes_lib *bytes_lib = malloc(sizeof(Bytes_lib));
    bytes_lib->add = add;
    bytes_lib->double_bytes = double_bytes;
    bytes_lib->to_bytes = to_bytes;
    bytes_lib->to_ull = to_ull;
    bytes_lib->print = print_bytes;
    bytes_lib->mod = mod;
    bytes_lib->pow_mod = pow_mod;
    bytes_lib->mul = mul;
    bytes_lib->sub = sub;
    bytes_lib->sup = sup;
    bytes_lib->half = half;
    bytes_lib->equals = equals;
    bytes_lib->random_bytes = random_bytes;
    bytes_lib->hex_to_bytes = hex_to_bytes;
    bytes_lib->seed = time(NULL);
    return bytes_lib;
}