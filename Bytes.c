#include "Bytes.h"

enum bool{true=1, false=0};

bytes* exec_fuc_1(bytes(*func)(const bytes a), bytes* a) {
    byte* temp = a->data;
    *a = func(*a);
    free(temp);
    return a;
}

bytes* exec_fuc_2(bytes(*func)(const bytes a, const bytes b), bytes* a, const bytes* b) {
    byte* temp = a->data;
    *a = func(*a, *b);
    free(temp);
    return a;
}

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

// make a one with memmove
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
    for (st i = 0; i < res.size; i++) {
        res.data[i] = (a.data[i]&254) >> 1; // shift the current byte
        res.data[i] += ((i + 1) == a.size ? 0 : (a.data[i + 1] & 0x1) << 7); // add the lower bit of the next byte
    }
    return resize_bytes(&res) ? res : (bytes) {NULL, 0};
}

static bytes add(const bytes a, const bytes b) {
    bytes res = alloc_bytes((a.size > b.size ? a.size : b.size) + 1);
    byte carry = 0;
    for (st i = 0; i < res.size; i++) {
        int data = carry + (i < a.size ? a.data[i] : 0) + (i < b.size ? b.data[i] : 0);
        res.data[i] = data & 255;
        carry = data >> 8;
    }
    return resize_bytes(&res) ? res : (bytes) {NULL, 0};
}
// TODO: Simplify this function
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
    bytes res = alloc_bytes(a.size);
    byte carry = 0;
    for (st i = 0; i < res.size; i++) {
        int temp = a.data[i] - (i >= b.size ? 0 : b.data[i]) - carry; // a - b - carry
        res.data[i] = temp < 0 ? temp + 256 : temp; // if temp < 0, add 256 to get the correct value
        carry = temp < 0 ? 1 : 0; // if temp < 0, carry = 1
    }
    return resize_bytes(&res) ? res : (bytes) {NULL, 0};
}

static bytes mod(const bytes a, const bytes b) {
    bytes x = alloc_bytes(b.size);
    memmove(x.data, b.data, b.size); // x = b
    bytes res = alloc_bytes(a.size);
    memmove(res.data, a.data, a.size); // res = a
    bytes y = half(a);
    st size = a.size <= 1 + b.size ? 0 : a.size - b.size - 1;
    // use memmove to shift faster
    x.data = realloc(x.data, x.size + size);
    x.size += size;
    memmove(x.data + size, x.data, x.size - size);
    for (st i = 0; i < size; i++)
        x.data[i] = 0;
    while (sup(x, y) == 0) {
        exec_fuc_1(&double_bytes, &x); // x = x << 1
    }
    free(y.data);
    while (sup(b, res) == 0) {
        if (sup(x, res) == 0) {
            exec_fuc_2(&sub, &res, &x); // res = res - x
        }
        exec_fuc_1(&half, &x); // x = x >> 1
    }
    free(x.data);
    return resize_bytes(&res) ? res : (bytes) {NULL, 0};
}

static bytes pow_mod(const bytes b, const bytes e, const bytes m) {
    if (m.size == 1 && m.data[0] == 0)
        return to_bytes(0);
    bytes res = alloc_bytes(1);
    res.data[0] = 1;
    bytes base = mod(b, m);
    bytes exp = alloc_bytes(e.size);
    memmove(exp.data, e.data, e.size); // copy e to exp
    while (sup(exp, to_bytes(0))) {
        if (exp.data[0] % 2 == 1)
            exec_fuc_2(&mod, exec_fuc_2(&mul, &res, &base), &m); // res = (res * base) % m
        exec_fuc_2(&mod, exec_fuc_2(&mul, &base, &base), &m); // base = (base * base) % m
        exec_fuc_1(&half, &exp); // exp = half(exp)
    }
    free(base.data);
    free(exp.data);
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