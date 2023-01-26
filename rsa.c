#include <stdio.h>
#include <stdlib.h>
#include "rsa.h"
#include <time.h>


bytes crypt(const bytes m, const bytes key_0, const bytes key_1) {
    Bytes_lib *lib = bytes_lib();
    bytes res = lib->pow_mod(m, key_0, key_1);
    free(lib);
    return res;
}

int miller_rabin(const bytes n) {
    Bytes_lib *lib = bytes_lib();
    bytes num_2 = lib->to_bytes(2);
    bytes num_1 = lib->to_bytes(1);
    bytes num_4 = lib->to_bytes(4);
    bytes s = lib->sub(n, num_1);
    bytes t = lib->to_bytes(0);
    bytes calc;
    byte *free_memory = NULL;
    while (s.data[0] == 0){
        free_memory = s.data;
        s = lib->half(s);
        free(free_memory);
        free_memory = t.data;
        t = lib->add(t, num_1);
        free(free_memory);
    }

    for (int i = 0; i < 10; i++)
    {
        bytes a = lib->random_bytes(n.size * 2, &lib->seed);
        free_memory = a.data;
        calc = lib->sub(n,num_4);
        a = lib->mod(a, calc);
        free(free_memory);
        free_memory = a.data;
        a = lib->add(a, num_2);
        free(free_memory);
        free(calc.data);

        bytes x = lib->pow_mod(a, s, n);
        free(a.data);
        if (x.size == 1 && x.data[0] == 1) {
            return 0;
        }
        calc = lib->sub(n, num_1);
        bytes j = lib->to_bytes(0);
        while (lib->equals(x, calc) == 0) {
            free(calc.data);
            calc = lib->sub(t, num_1);
            if (lib->equals(j, calc) == 1) {
                free(calc.data);
                free(j.data);
                free(t.data);
                free(s.data);
                free(num_1.data);
                free(num_2.data);
                free(num_4.data);
                free(x.data);
                free(lib);
                return 0;
            }
            free(calc.data);
            free_memory = j.data;
            j = lib->add(j, num_1);
            free(free_memory);
            free_memory = x.data;
            x = lib->pow_mod(x, num_2, n);
            free(free_memory);
            calc = lib->sub(n, num_1);
        }
        free(j.data);
        free(calc.data);
        free(x.data);
    }
    free(lib);
    free(s.data);
    free(t.data);
    free(num_2.data);
    free(num_1.data);
    free(num_4.data);
    return 1;
}

int is_prime(const bytes n) {
    Bytes_lib *lib = bytes_lib();
    if (n.size == 1 && (n.data[0] == 0 || n.data[0] == 1))
        return 0;
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
        bytes low_prime = lib->to_bytes(low_primes[i]);
        if (lib->equals(n, low_prime)) {
            free(low_prime.data);
            free(lib);
            return 1;
        }
        bytes calc = lib->mod(n, low_prime);
        if (calc.size == 1 && calc.data[0] == 0) {
            free(calc.data);
            free(low_prime.data);
            free(lib);
            return 0;
        }
        free(calc.data);
        free(low_prime.data);
    }
    free(lib);
    return miller_rabin(n);
}


int main() {
    Bytes_lib *lib = bytes_lib();

    clock_t start, end;
    double cpu_time_used;
    start = clock();

    bytes a = lib->hex_to_bytes("b33cfdbcfc292eab088c80c88d548eddb99edfc38df1a59212e8c762acb0e5f96ff2e717db218bcfbf261bcdcd88899c172063f75e127fe9a86559aeb60e375ae2ff73dc33b37274b1f2a611d9ba19696bba1b411206d82c351c14ad82c65479b1baceeea250c4c8a5f0eccd505e2e99b03224d7fd5e21bfc1c2c33675fbd2b886bfe3ce35fa58976ca9ff25da090016a2172997120c95fe6df6df1de05dacad8c85f2ac06549fe37f5d868dccdd9c9946068a1bf24c660e4e9c5dc2a60a97455b4a27bfc0c0de2afd8684d2a19d1b4ac3b1ee4e70257b29959f37b8058eb9da2d5c8fb7bc8c8e68decaa62e071b6d2e1b8a7575a2dcfdcf10ade5b40e41a35");
    //printf("%d\n", is_prime(a));
    bytes b = lib->to_bytes(511);
    lib->print(b);
    printf("\n");
    //bytes c = lib->mod(a, b);
    bytes c = lib->half(a);
    lib->print(c);
    free(c.data);
    free(b.data);
    free(a.data);
    a = lib->to_bytes(7);
    //printf("\n%d\n", is_prime(a));


    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    //printf("%f", cpu_time_used);
    printf("\n");

    free(a.data);

    free(lib);
    return 0;
}