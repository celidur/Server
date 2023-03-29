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

int main() {
    Bytes_lib *lib = bytes_lib();

    clock_t start, end;
    double cpu_time_used;
    start = clock();
    /*
    bytes a = lib->hex_to_bytes("b33cfdbcfc292eab088c80c88d548eddb99edfc38df1a5b9212e8c762acb0e5f96ff2e717db218bcfbf261bcdcd88899c172063f75e127fe9a86559aeb60e375ae2ff73dc33b37274b1f2a611d9ba19696bba1b411206d82c351c14ad82c65479b1baceeea250c4c8a5f0eccd505e2e99b03224d7fd5e21bfc1c2c33675fbd2b886bfe3ce35fa58976ca9ff25da090016a2172997120c95fe6df6df1de05dacad8c85f2ac06549fe37f5d868dccdd9c9946068a1bf24c660e4e9c5dc2a60a97455b4a27bfc0c0de2afd8684d2a19d1b4ac3b1ee4e70257b29959f37b8058eb9da2d5c8fb7bc8c8e68decaa62e071b6d2e1b8a7575a2dcfdcf10ade5b40e41a35");
    int i = lib->is_prime(a, &lib->seed);
    printf("%d\n", i);
    free(a.data);
     */
    bytes a = lib->hex_to_bytes("9eea44e6be967baf86f2cb6a08bb9d9cb346f92ed74de72e2fa80de6aa7d062cdd9eda1e4719a0ebe303c7fd0337cc0ebeecca4a92550f05c89eb621dc5020551a25cb34066ee49aef7c7f95b66a301f331eb51d3b580ca6eacfc657ec1bed305799bd0c2be1318ba6812a8b0108a716cc61428c3b371c019e1b657350b4dd7ec99d08ba9a7918a28d3239d0fa0a3cde5949f35cb183b95e8603305554898ff9ba70e378b27a9106d5ef194c974be9fce5057bd5ef64016b87e8c36ac01913f0b1a6d72444773b10b4ddb88e6b68d4faf89f4bb865dd60e0b4f65ace46b834d65881c5198c232ab1965af6af459498acbad2baa5ac0846febb2d3e834193b53e");
    bytes b = lib->hex_to_bytes("2ccf3f6f3f0a4baac2232032235523b76e67b7f0e37c696e484ba31d8ab2c397e5bfcb9c5f6c862f3efc986f37362226705c818fdd7849ffa6a19566bad838dd6b8bfdcf70cecdc9d2c7ca984766e865a5aee86d04481b60b0d47052b60b1951e6c6eb3bba8943132297c3b3354178ba66c0c8935ff57886ff070b0cd9d7ef4ae21aff8f38d7e9625db2a7fc976824005a885ca65c483257f9b7db7c778176b2b63217cab019527f8dfd761a3733767265181a286fc93198393a71770a982a5d156d289eff030378abf61a134a86746d2b0ec7b939c095eca6567cdee0163ae768b5723edef23239a37b2a98b81c6db4b86e29d5d68b73f73c42b796d039068d");
    bytes c = lib->hex_to_bytes("b33cfdbcfc292eab088c80c88d548eddb99edfc38df1a5b9212e8c762acb0e5f96ff2e717db218bcfbf261bcdcd88899c172063f75e127fe9a86559aeb60e375ae2ff73dc33b37274b1f2a611d9ba19696bba1b411206d82c351c14ad82c65479b1baceeea250c4c8a5f0eccd505e2e99b03224d7fd5e21bfc1c2c33675fbd2b886bfe3ce35fa58976ca9ff25da090016a2172997120c95fe6df6df1de05dacad8c85f2ac06549fe37f5d868dccdd9c9946068a1bf24c660e4e9c5dc2a60a97455b4a27bfc0c0de2afd8684d2a19d1b4ac3b1ee4e70257b29959f37b8058eb9da2d5c8fb7bc8c8e68decaa62e071b6d2e1b8a7575a2dcfdcf10ade5b40e41a35");
    bytes d = lib->pow_mod(a, b, c);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%f", cpu_time_used);
    printf("\n");
    lib->print(d);

    free(a.data);
    free(b.data);
    free(c.data);
    free(d.data);
    free(lib);
    return 0;
}