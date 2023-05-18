#include "Bytes.h"

enum bool {
    true = 1, false = 0
};

bytes* exec_fn_1(bytes(* func)(const bytes a), bytes* a) {
    byte* temp = a->data;
    *a = func(*a);
    free(temp);
    return a;
}

bytes* exec_fn_2(bytes(* func)(const bytes a, const bytes b), bytes* a, const bytes* b) {
    byte* temp = a->data;
    *a = func(*a, *b);
    free(temp);
    return a;
}

bytes* exec_fn_3(bytes(* func)(const bytes a, const bytes b, const bytes c), bytes* a, const bytes* b, const bytes* c) {
    byte* temp = a->data;
    *a = func(*a, *b, *c);
    free(temp);
    return a;
}

void print_bytes(const bytes b) {
    printf("0x");
    for (st i = b.size - 1; i < b.size; i--)
        printf("%02x", b.data[i]);
    printf("\n");
}

void print_ull(const ulls b) {
    print_bytes((bytes) {(byte*) b.data, b.size << 3});
}

bytes alloc_bytes(st size) {
    size = ((size >> 3) + 1) << 3;
    bytes b = {NULL, size};
    b.data = malloc(size);
    if (b.data == NULL)
        printf("Error: alloc_bytes\n");
    return b.data == NULL ? (bytes) {NULL, 0} : b;
}

ulls alloc_ulls(st size) {
    ulls b = {NULL, size};
    b.data = malloc(size * sizeof(ull));
    if (b.data == NULL)
        printf("Error: alloc_ulls\n");
    return b.data == NULL ? (ulls) {NULL, 0} : b;
}

void free_bytes(bytes* b) {
    free(b->data);
    b->data = NULL;
    b->size = 0;
}

int resize_bytes(bytes* b) {
    while (b->size > 1 && b->data[b->size - 1] == 0) b->size--;
    st size = b->size & 7 ? ((b->size >> 3) + 1) << 3 : b->size;
    b->data = realloc(b->data, size * sizeof(byte));
    if (b->data == NULL) {
        b->size = 0;
        printf("Error: resize_bytes\n");
    }
    return b->data != NULL;
}

bytes to_bytes(const unsigned long long n) {
    st size = 1;
    unsigned long long tmp = n;
    while (tmp /= 256) size++;
    bytes b = alloc_bytes(size);
    b.data = memcpy(b.data, &n, size);
    resize_bytes(&b);
    return b;
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
    ulls a_ = convert_bytes(&a);
    ulls b_ = convert_bytes(&b);
    ulls res_ = alloc_ulls(a_.size);
    res_.data = memcpy(res_.data, a_.data, a_.size * sizeof(ull));
    byte carry = 0;
    for (st i = 0; i < b_.size; i++) {
        ull carry2 = (a_.data[i] < b_.data[i] + carry);
        res_.data[i] -= ~carry2 + 1 + b_.data[i] - carry + carry2 ;
        carry = carry2;
    }
    res_.data[b_.size] -= carry;
    bytes res = convert_ull(&res_);
    return resize_bytes(&res) ? res : (bytes) {NULL, 0};
}

static bytes double_bytes(const bytes a) {
    bytes res = alloc_bytes(a.size + 1);
    res.data[0] = a.data[0] & 127; // shift the first byte
    for (st i = 1; i < a.size; i++) {
        res.data[i] = (a.data[i] & 127) << 1; // shift the current byte
        res.data[i] += (a.data[i - 1] & 0x80) >> 7; // add the carry of the previous byte
    }
    res.data[a.size] += (a.data[a.size - 1] & 0x80) >> 7; // add the carry of the previous byte
    resize_bytes(&res);
    return res;
}

static bytes half(const bytes a) {
    bytes res = alloc_bytes(a.size);
    for (st i = 0; i < res.size - 1; i++) {
        res.data[i] += (a.data[i + 1] & 0x1) << 7; // add the lower bit of the next byte
        res.data[i] = (a.data[i] & 0xfe) >> 1; // shift the current byte
    }
    res.data[a.size - 1] = (a.data[a.size - 1] & 0xfe) >> 1; // shift the current byte
    return resize_bytes(&res) ? res : (bytes) {NULL, 0};
}

static bytes add(const bytes a, const bytes b) {
    if (a.size < b.size)
        return add(b, a);
    ulls a_ = convert_bytes(&a);
    ulls b_ = convert_bytes(&b);
    ulls res_ = alloc_ulls(a_.size + 1);
    res_.data = memcpy(res_.data, a_.data, a_.size * sizeof(ull));
    byte carry = 0;
    for (st i = 0; i < b_.size; i++) {
        res_.data[i] += carry + b_.data[i];
        carry = a_.data[i] > 0xffffffffffffffff - b_.data[i];
    }
    res_.data[b_.size] = carry;
    bytes res = convert_ull(&res_);
    return res;
}

// TODO: Simplify this function
static bytes mul(const bytes a, const bytes b) {
    bytes res = alloc_bytes(a.size + b.size);
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
            exec_fn_1(&double_bytes, &tmp);
        }
        exec_fn_2(&add, &res, &tmp);
        free_bytes(&tmp);
    }
    return resize_bytes(&res) ? res : (bytes) {NULL, 0};
}


static bytes mul2(const bytes a, const bytes b) {
    if (a.size <= 8 || b.size <= 8) {
        return sup(a, b) ? mul(a, b) : mul(b, a);
    }

    st size = a.size > b.size ? a.size : b.size;
    st half_size = size / 2;

    bytes a0 = alloc_bytes(half_size);
    bytes a1 = alloc_bytes(size - half_size);
    bytes b0 = alloc_bytes(half_size);
    bytes b1 = alloc_bytes(size - half_size);

    for (st i = 0; i < half_size; i++) {
        a0.data[i] = a.size > i ? a.data[i] : 0;
        b0.data[i] = b.size > i ? b.data[i] : 0;
    }
    for (st i = 0; i < size - half_size; i++) {
        a1.data[i] = a.size > i + half_size ? a.data[i + half_size] : 0;
        b1.data[i] = b.size > i + half_size ? b.data[i + half_size] : 0;
    }

    bytes z0 = mul2(a0, b0);
    bytes z1 = mul2(add(a0, a1), add(b0, b1));
    bytes z2 = mul2(a1, b1);

    int signe = sup(z1, z0) ? 1 : -1;
    byte* c;
    bytes temp = signe == 1 ? sub(z1, z0) : sub(z0, z1);
    c = temp.data;
    int singe2 = sup(temp, z2) ? 1 : -1;
    temp = signe == 1 ? singe2 == 1 ? sub(temp, z2) : sub(z2, temp) : add(temp, z2);
    free(c);
    for (int i = 0; i < half_size; ++i) {
        exec_fn_1(double_bytes, &temp);
    }
    free_bytes(&temp);
    for (int i = 0; i < size; ++i) {
        exec_fn_1(double_bytes, &z2);
    }
    bytes res = add(z0, z2);
    exec_fn_2(singe2 == 1 ? add : sub, &res, &temp);
    free_bytes(&a0);
    free_bytes(&a1);
    free_bytes(&b0);
    free_bytes(&b1);
    free_bytes(&z0);
    free_bytes(&z1);
    free_bytes(&z2);
    return res;
}

static bytes mod(const bytes a, const bytes b) {
    bytes x = alloc_bytes(b.size);
    memmove(x.data, b.data, b.size); // x = b
    bytes res = alloc_bytes(a.size);
    memmove(res.data, a.data, a.size); // res = a
    bytes y = half(a);
    // use memmove to shift faster
    st size = a.size <= 1 + b.size ? 0 : a.size - b.size - 1;
    x.data = realloc(x.data, x.size + size);
    x.size += size;
    memmove(x.data + size, x.data, x.size - size);
    for (st i = 0; i < size; i++)
        x.data[i] = 0;
    while (sup(x, y) == 0) {
        exec_fn_1(&double_bytes, &x); // x = x << 1
    }
    free_bytes(&y);
    while (sup(b, res) == 0) {
        if (sup(x, res) == 0) {
            exec_fn_2(&sub, &res, &x); // res = res - x
        }
        exec_fn_1(&half, &x); // x = x >> 1
    }
    free_bytes(&x);
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
    bytes n_0 = alloc_bytes(1);
    n_0.data[0] = 0;
    while (sup(exp, n_0)) {
        if (exp.data[0] % 2 == 1)
            exec_fn_2(&mod, exec_fn_2(&mul, &res, &base), &m); // res = (res * base) % m
        exec_fn_2(&mod, exec_fn_2(&mul, &base, &base), &m); // base = (base * base) % m
        exec_fn_1(&half, &exp); // exp = half(exp)
        printf("size of exp: %zu\n", exp.size);
    }
    free_bytes(&n_0);
    free_bytes(&base);
    free_bytes(&exp);
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

void random_int(long int* seed) {
    *seed = (*seed * 1103515245 + 12345) % 2147483648;
}

void shuffle_bytes(bytes* a, long int* seed) {
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

bytes random_bytes(const st size, long int* seed) {
    bytes res = alloc_bytes(size);
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


bytes hex_to_bytes(const char* hex) {
    st size = (strlen(hex) + 1) / 2;
    bytes res = alloc_bytes(size);
    for (st i = 0; i < size; i++) {
        char temp[3] = {'0', hex[strlen(hex) - 1 - i * 2], '\0'};
        if (strlen(hex) - 1 - i * 2 != 0)
            temp[0] = hex[strlen(hex) - 2 - i * 2];
        res.data[i] = strtol(temp, NULL, 16);
    }
    resize_bytes(&res);
    return res;
}

int miller_rabin(const bytes n, long int* seed) {
    bytes num_2 = to_bytes(2);
    bytes num_1 = to_bytes(1);
    bytes num_4 = to_bytes(4);
    bytes s = sub(n, num_1);
    bytes t = to_bytes(0);
    bytes calc;
    while (s.data[0] % 2 == 0) {
        exec_fn_1(&half, &s);
        exec_fn_2(&add, &t, &num_1);
    }

    for (int i = 0; i < 10; i++) {
        bytes a = random_bytes(n.size * 2, seed);
        // a = random_bytes
        calc = sub(n, num_4);
        exec_fn_2(&mod, &a, &calc);
        free_bytes(&calc);
        exec_fn_2(&add, &a, &num_2);
        // a = (a % (n - 4)) + 2;
        print_bytes(a);
        print_bytes(s);
        print_bytes(n);
        bytes x = pow_mod(a, s, n);
        free_bytes(&a);
        free_bytes(&s);

        if (x.size == 1 && x.data[0] == 1) {
            continue;
        }
        calc = sub(n, num_1);
        bytes j = to_bytes(0);
        while (equals(x, calc) == false) {
            free_bytes(&calc);
            calc = sub(t, num_1);
            if (equals(j, calc) == true) {
                free_bytes(&calc);
                free_bytes(&j);
                free_bytes(&t);
                free_bytes(&num_1);
                free_bytes(&num_2);
                free_bytes(&num_4);
                free_bytes(&x);
                return false;
            }
            free_bytes(&calc);
            exec_fn_2(&add, &j, &num_1); // j += 1;
            exec_fn_3(&pow_mod, &x, &num_2, &n); // x = pow_mod(x, 2, n);
            calc = sub(n, num_1);
        }
        free_bytes(&j);
        free_bytes(&calc);
        free_bytes(&x);
    }
    free_bytes(&t);
    free_bytes(&num_2);
    free_bytes(&num_1);
    free_bytes(&num_4);
    printf("prime\n");
    return true;
}

int is_prime(const bytes n, long int* seed) {
    if (n.size == 1 && (n.data[0] == 0 || n.data[0] == 1))
        return false;
    int low_primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97,
                        101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193,
                        197,
                        199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311,
                        313,
                        317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433,
                        439,
                        443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569,
                        571,
                        577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683,
                        691,
                        701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827,
                        829,
                        839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971,
                        977,
                        983, 991, 997};
    for (int i = 0; i < 168; i++) {
        bytes low_prime = to_bytes(low_primes[i]);
        if (equals(n, low_prime)) {
            free_bytes(&low_prime);
            return true;
        }
        bytes calc = mod(n, low_prime);
        free_bytes(&low_prime);
        if (calc.size == 1 && calc.data[0] == 0) {
            free_bytes(&calc);
            return false;
        }
        free_bytes(&calc);
    }
    printf("miller_rabin\n");
    return miller_rabin(n, seed);
}

Bytes_lib* bytes_lib() {
    Bytes_lib* bytes_lib = malloc(sizeof(Bytes_lib));
    bytes_lib->add = add;
    bytes_lib->double_bytes = double_bytes;
    bytes_lib->to_bytes = to_bytes;
    bytes_lib->to_ull = to_ull;
    bytes_lib->print = print_bytes;
    bytes_lib->mod = mod;
    bytes_lib->pow_mod = pow_mod;
    bytes_lib->mul = mul;
    bytes_lib->mul2 = mul2;
    bytes_lib->sub = sub;
    bytes_lib->sup = sup;
    bytes_lib->half = half;
    bytes_lib->equals = equals;
    bytes_lib->random_bytes = random_bytes;
    bytes_lib->hex_to_bytes = hex_to_bytes;
    bytes_lib->seed = time(NULL);
    bytes_lib->is_prime = is_prime;
    return bytes_lib;
}

bytes convert_ull(const ulls* n) {
    bytes b = {NULL, n->size << 3};
    b.data = (byte*) n->data;
    resize_bytes(&b);
    return b;
}

ulls convert_bytes(const bytes* b) {
    size_t size = b->size >> 3;
    size_t i = b->size & 7;
    ulls n = {NULL, size + (i != 0)};
    n.data = (ull*) b->data;
    return n;
}
