#include "rsa.h"

/*
 * @param username - username of user
 * @param password - password of user
 * @param n_ - public key of user (optional) (if n_ == 0, generate the entire key pair)
 * @return key_pair - key pair
 * */
private_key* generate_key_private(char* username, char* password, mpz_t n_) {
    mpz_t seed, seed2;
    mpz_init(seed);
    mpz_init(seed2);
    mpz_set_str(seed, username, 62);
    mpz_set_str(seed2, password, 62);
    mpz_xor(seed, seed, seed2);
    mpz_clear(seed2);
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    gmp_randseed(state, seed);
    mpz_urandomb(seed, state, 2048);
    mpz_t random;
    mpz_init(random);
    mpz_clear(seed);

    mpz_t p, q;
    mpz_init(p);
    mpz_init(q);

    mpz_urandomb(random, state, 4096);
    mpz_nextprime(p, random);
    if (n_ != 0) {
        mpz_div(q, n_, p);
        mpz_t tmp;
        mpz_init(tmp);
        mpz_mul(tmp, p, q);
        if (mpz_cmp(n_, tmp) != 0) {
            printf("Error: n_ is not a product of two primes\n");
            mpz_urandomb(random, state, 4096);
            mpz_nextprime(q, random);
        }
        mpz_clear(tmp);
    } else {
        mpz_urandomb(random, state, 4096);
        mpz_nextprime(q, random);
    }

    mpz_clear(random);

    mpz_t phi, e;
    private_key* key = malloc(sizeof(private_key));
    mpz_init(phi);
    mpz_init(key->n);
    mpz_init(key->d);

    // e = 65537
    mpz_init_set_ui(e, 65537);

    mpz_mul(key->n, p, q);

    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi, p, q);

    mpz_invert(key->d, e, phi);

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(phi);
    mpz_clear(e);

    return key;
}

/*
 * @param private - private key to generate key pair from
 * @return key_pair - key pair
 * */
key_pair generate_key(char* username, char* password, mpz_t n_) {
    key_pair pair;
    pair.priv = generate_key_private(username, password, n_);
    pair.pub = malloc(sizeof(public_key));
    mpz_init(pair.pub->n);
    mpz_init(pair.pub->e);
    mpz_set(pair.pub->n, pair.priv->n);
    mpz_set_ui(pair.pub->e, 65537);
    return pair;
}

/*
 * @param msg - message to encrypt
 * @param key - public key to encrypt with
 * @return message - encrypted message
 * */
message rsa_encrypt(message msg, public_key key) {
    message encrypted;
    mpz_init(encrypted.msg);
    mpz_powm(encrypted.msg, msg.msg, key.e, key.n);
    return encrypted;
}

/*
 * @param msg - message to decrypt
 * @param key - private key to decrypt with
 * @return message - decrypted message
 * */
message rsa_decrypt(message msg, private_key key) {
    message decrypted;
    mpz_init(decrypted.msg);
    mpz_powm(decrypted.msg, msg.msg, key.d, key.n);
    return decrypted;
}

/*
 * @param pair - key pair to clear
 * */
void clear_keys(key_pair pair) {
    mpz_clear(pair.pub->n);
    mpz_clear(pair.pub->e);
    mpz_clear(pair.priv->n);
    mpz_clear(pair.priv->d);
    free(pair.pub);
}

/*
 * @param msg - message to clear
 * */
void clear_message(message msg) {
    mpz_clear(msg.msg);
}

//int main() {
//    clock_t start, end;
//    double cpu_time_used;
//
//    char* username = "frhvbhv";
//    char* password = "ufhuhruhfvbhfufhured";
//
//    mpz_t n;
//    mpz_init(n);
//
//    mpz_set_str(n,"5f22049597b3099452e8d219f09c709d145481fedfed7611e8b61c714ece701f1449aa1ab72b1d3456ddc6ef181139242f5bed03c72cc74d3b0d409c8d8566bb450856abef9fce07e56c02a45238de00631ea98bc2d4b13aed26450fad5a67aa4d502c3fa5f9a29256b18c0249ddf2f6c0f6c4649506f4ca612d9997f162aba5288db401648dfb704f2bd6e1e6f565e522a21540dec7f50504e0465c0e1db0aa5053af238a7e140e3221434f0f840d0ba90238061c2a3314a06f8ae7cd1364e63c1572f34a36be8d869523255adeb0794716864961231610768ce85e24c1e018ebacc7405d9db39c97c202a91de21683a392914f33093b449d4238984fa58e26d3a6564fa6e4f4b697ec8a76db44cfb01ff86865d88b3986b08195c17bc2f8a0e0fce3295fb8d94809dba6cee37d799d5af600253bddc10a7db7832f8b252197dfa80bbb215ae48ccfa03f61d384740aa332dff2d3343a6ff63964ac5b0f307997df0c4832fe6018d6cddfd2f6fd53034d71cc44c10c843b3725e1a95df7ae9355cf1e7b364e0a570cb2f23e33d6090edf5d2fec9218d783d66c2a3b4b42ea9aa3d3e5fa12ea7a362e076e70885a2be76fc193bd630bac117703473f58406605ae417ea933a8ef5cb77bd990b7e134b4586c05fa56786bb8d6e5112f2de279e6bed2cd8e4a1190c83891c6e322fa9b77486813f57d796cec82c10a54b677c8fcfb2f92bb43ea20211b8b826d98add1125432025b94fa47f709aac1d25e175038239819e5ee64dc5a411819d7b7fcdb5c1782d193c982e40127c415d51c70c8c90b23d36599f68d965d7955a387f0c94f4f4e17f32727fef5414ad64d3ef2a5a966df13856c7d06dae43abb5163a1a9145f5b8094b20d1f8008d33308bf318faab5f5df166684beccba5a2129808b0a1e2b253005a2428de8d2e3c562f6236637ad444926f376e4313fecd1d40f1691453f085c73ee960062d17ed5982e365b4a9cbe43609848fb56cd061a370c857239117ac36f8ae3db612f325047ed95c2edd7bd8dfb4002fbc02108b95b4aa2346f0c9f7e0ab00e6200c281a70dcf8dd922bfa1dfeb65b98c44698917c840c6f2305c64299c0dc120a8562f67732c238f6cb4b75e8c1d773c7c3c8897e5cc10274fa5d793806b059414098b1f597dda2fb3dfdb61ebdf616c2ab56b9631de7b7183d0a4da01648bb47ad700fe96d243e33f404b5fa2068c901ea5e41eb1e5e752679933fe12f6fa96c14a7cd8a6deb03bc9302d354b42643c5ba7affdc85dbb90a2acffa976330580575f2b5bc7c173d9f41cd1bd892df3974f168e0220676ff53bf9c8e734bcec38a099eeaba7d890e2c323a4f30630550e674c0282d820e95a4db9b0838341b9accabf0625fb1454af2c3c721d0366cdedd5f7db5a1d4e2f365dfcdcec21ef8870dca9b4a383cf7ab20d",16);
//
//    start = clock();
//    key_pair key = generate_key(username, password, n);
//
//    end = clock();
//
//    message msg;
//    mpz_init(msg.msg);
//    mpz_set_str(msg.msg, "123456789", 10);
//
//    message encrypted = encrypt(msg, *key.pub);
//    message decrypted = decrypt(encrypted, *key.priv);
//
//    gmp_printf("Decrypted: %Zd\n", decrypted.msg);
//
//    // convert number to string
//    char* decrypted_str = mpz_get_str(password, 10, decrypted.msg);
//
//    printf("Decrypted: %s\n", decrypted_str);
//
//    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
//    printf("%f", cpu_time_used);
//    printf("\n");
//
//    clear_keys(key);
//    return 0;
//}