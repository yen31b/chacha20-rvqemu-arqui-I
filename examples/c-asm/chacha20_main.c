/* Programa principal en C
* Este programa ejecuta los vectores de prueba del RFC 8439 Apendice A.2
* Muestra la clave, nonce, contador, keystream, plaintext, ciphertext obtenido
* del encriptado, el ciphertext expected, y tambien verifica que el descifrado 
* recupera el plaintext o mensaje original
*/ 


#include <stdint.h>

// ChaCha20 es simetrico: cifrar y descifrar usan la misma operacion
// la llamada con el mismo key/nonce/counter en el ciphertext devuelve el plaintext

extern void chacha20_encrypt(
    uint8_t        *output,
    const uint8_t  *input,
    uint32_t        len,
    const uint32_t *key,
    uint32_t        counter,
    const uint32_t *nonce);

// Prints necesarios

static void print_char(char c) {
    volatile char *uart = (volatile char *)0x10000000;
    *uart = c;
}

static void print_string(const char *str) {
    while (*str) print_char(*str++);
}

static void print_hex8(uint8_t val) {
    const char hex[] = "0123456789abcdef";
    print_char(hex[(val >> 4) & 0xF]);
    print_char(hex[val & 0xF]);
}

static void print_dec(int n) {
    if (n == 0) { print_char('0'); return; }
    char buf[10]; int i = 0;
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    while (i > 0) print_char(buf[--i]);
}

// Muestra buffer en hex + columna ASCII (16 bytes por linea)
static void print_buf_hex(const uint8_t *buf, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        if (i % 16 == 0) print_string("  ");
        print_hex8(buf[i]);
        print_char(' ');
        if ((i + 1) % 16 == 0) {
            print_char(' ');
            for (uint32_t j = i - 15; j <= i; j++) {
                uint8_t c = buf[j];
                print_char((c >= 0x20 && c < 0x7f) ? (char)c : '.');
            }
            print_char('\n');
        }
    }
    uint32_t rem = len % 16;
    if (rem) {
        for (uint32_t i = rem; i < 16; i++) print_string("   ");
        print_char(' ');
        for (uint32_t j = len - rem; j < len; j++) {
            uint8_t c = buf[j];
            print_char((c >= 0x20 && c < 0x7f) ? (char)c : '.');
        }
        print_char('\n');
    }
}

// Imprime clave (8 palabras little-endian) como bytes en orden de memoria
static void print_key(const uint32_t *k) {
    for (int i = 0; i < 8; i++) {
        if (i % 4 == 0) print_string("  ");
        print_hex8((uint8_t)(k[i]));        print_char(' ');
        print_hex8((uint8_t)(k[i] >>  8));  print_char(' ');
        print_hex8((uint8_t)(k[i] >> 16));  print_char(' ');
        print_hex8((uint8_t)(k[i] >> 24));  print_char(' ');
        if ((i + 1) % 4 == 0) print_char('\n');
    }
}

// Imprime nonce (3 palabras little-endian) como bytes en orden de memoria
static void print_nonce(const uint32_t *n) {
    print_string("  ");
    for (int i = 0; i < 3; i++) {
        print_hex8((uint8_t)(n[i]));        print_char(' ');
        print_hex8((uint8_t)(n[i] >>  8));  print_char(' ');
        print_hex8((uint8_t)(n[i] >> 16));  print_char(' ');
        print_hex8((uint8_t)(n[i] >> 24));  print_char(' ');
    }
    print_char('\n');
}

// Verificacion de pruebas

static int tests_run    = 0;
static int tests_passed = 0;

static void check_buf(const char *label, const uint8_t *got,
                      const uint8_t *expected, uint32_t len) {
    int fails = 0;
    for (uint32_t i = 0; i < len; i++) {
        tests_run++;
        if (got[i] == expected[i]) {
            tests_passed++;
        } else {
            fails++;
            print_string("\033[1;31m  [FAIL] \033[0m");
            print_string(label);
            print_string(" byte["); print_dec((int)i); print_string("]: got=0x");
            print_hex8(got[i]); print_string(" expected=0x"); print_hex8(expected[i]);
            print_char('\n');
        }
    }
    if (fails == 0) {
        print_string("\033[1;32m  [PASS] \033[0m");
        print_string(label);
        print_string(" ("); print_dec((int)len); print_string(" bytes correctos)\n");
    } else {
        print_string("\033[1;31m  [FAIL] \033[0m");
        print_string(label);
        print_string(": "); print_dec(fails); print_char('/');
        print_dec((int)len); print_string(" bytes incorrectos\n");
    }
}


// Para cada test vector
//Define la clave 256 bits (8 palabras de 32 bits)
// Define el nonce de 96 bits (3 palabras de 32 bits) como arrays de uint32_t

// TV #1: key=0, nonce=0, counter=0, plaintext=64 ceros 
static const uint32_t tv1_key[8]   = { 0,0,0,0, 0,0,0,0 };
static const uint32_t tv1_nonce[3] = { 0,0,0 };
#define TV1_CTR 0
#define TV1_LEN 64
static const uint8_t tv1_plaintext[TV1_LEN] = { 0 };
static const uint8_t tv1_expected[TV1_LEN] = {
    0x76,0xb8,0xe0,0xad, 0xa0,0xf1,0x3d,0x90, 0x40,0x5d,0x6a,0xe5, 0x53,0x86,0xbd,0x28,
    0xbd,0xd2,0x19,0xb8, 0xa0,0x8d,0xed,0x1a, 0xa8,0x36,0xef,0xcc, 0x8b,0x77,0x0d,0xc7,
    0xda,0x41,0x59,0x7c, 0x51,0x57,0x48,0x8d, 0x77,0x24,0xe0,0x3f, 0xb8,0xd8,0x4a,0x37,
    0x6a,0x43,0xb8,0xf4, 0x15,0x18,0xa1,0x1c, 0xc3,0x87,0xb6,0x69, 0xb2,0xee,0x65,0x86
};

// TV #2: key=0..01, nonce=0..02, counter=1, plaintext IETF text 
static const uint32_t tv2_key[8]   = {
    0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x01000000
};
static const uint32_t tv2_nonce[3] = { 0x00000000,0x00000000,0x02000000 };
#define TV2_CTR 1
#define TV2_LEN 375
static const uint8_t tv2_plaintext[TV2_LEN] =
    "Any submission to the IETF intended by the Contributor for publi"
    "cation as all or part of an IETF Internet-Draft or RFC and any s"
    "tatement made within the context of an IETF activity is consider"
    "ed an \"IETF Contribution\". Such statements include oral statemen"
    "ts in IETF sessions, as well as written and electronic communica"
    "tions made at any time or place, which are addressed to";
static const uint8_t tv2_expected[TV2_LEN] = {
    0xa3,0xfb,0xf0,0x7d,0xf3,0xfa,0x2f,0xde,0x4f,0x37,0x6c,0xa2,0x3e,0x82,0x73,0x70,
    0x41,0x60,0x5d,0x9f,0x4f,0x4f,0x57,0xbd,0x8c,0xff,0x2c,0x1d,0x4b,0x79,0x55,0xec,
    0x2a,0x97,0x94,0x8b,0xd3,0x72,0x29,0x15,0xc8,0xf3,0xd3,0x37,0xf7,0xd3,0x70,0x05,
    0x0e,0x9e,0x96,0xd6,0x47,0xb7,0xc3,0x9f,0x56,0xe0,0x31,0xca,0x5e,0xb6,0x25,0x0d,
    0x40,0x42,0xe0,0x27,0x85,0xec,0xec,0xfa,0x4b,0x4b,0xb5,0xe8,0xea,0xd0,0x44,0x0e,
    0x20,0xb6,0xe8,0xdb,0x09,0xd8,0x81,0xa7,0xc6,0x13,0x2f,0x42,0x0e,0x52,0x79,0x50,
    0x42,0xbd,0xfa,0x77,0x73,0xd8,0xa9,0x05,0x14,0x47,0xb3,0x29,0x1c,0xe1,0x41,0x1c,
    0x68,0x04,0x65,0x55,0x2a,0xa6,0xc4,0x05,0xb7,0x76,0x4d,0x5e,0x87,0xbe,0xa8,0x5a,
    0xd0,0x0f,0x84,0x49,0xed,0x8f,0x72,0xd0,0xd6,0x62,0xab,0x05,0x26,0x91,0xca,0x66,
    0x42,0x4b,0xc8,0x6d,0x2d,0xf8,0x0e,0xa4,0x1f,0x43,0xab,0xf9,0x37,0xd3,0x25,0x9d,
    0xc4,0xb2,0xd0,0xdf,0xb4,0x8a,0x6c,0x91,0x39,0xdd,0xd7,0xf7,0x69,0x66,0xe9,0x28,
    0xe6,0x35,0x55,0x3b,0xa7,0x6c,0x5c,0x87,0x9d,0x7b,0x35,0xd4,0x9e,0xb2,0xe6,0x2b,
    0x08,0x71,0xcd,0xac,0x63,0x89,0x39,0xe2,0x5e,0x8a,0x1e,0x0e,0xf9,0xd5,0x28,0x0f,
    0xa8,0xca,0x32,0x8b,0x35,0x1c,0x3c,0x76,0x59,0x89,0xcb,0xcf,0x3d,0xaa,0x8b,0x6c,
    0xcc,0x3a,0xaf,0x9f,0x39,0x79,0xc9,0x2b,0x37,0x20,0xfc,0x88,0xdc,0x95,0xed,0x84,
    0xa1,0xbe,0x05,0x9c,0x64,0x99,0xb9,0xfd,0xa2,0x36,0xe7,0xe8,0x18,0xb0,0x4b,0x0b,
    0xc3,0x9c,0x1e,0x87,0x6b,0x19,0x3b,0xfe,0x55,0x69,0x75,0x3f,0x88,0x12,0x8c,0xc0,
    0x8a,0xaa,0x9b,0x63,0xd1,0xa1,0x6f,0x80,0xef,0x25,0x54,0xd7,0x18,0x9c,0x41,0x1f,
    0x58,0x69,0xca,0x52,0xc5,0xb8,0x3f,0xa3,0x6f,0xf2,0x16,0xb9,0xc1,0xd3,0x00,0x62,
    0xbe,0xbc,0xfd,0x2d,0xc5,0xbc,0xe0,0x91,0x19,0x34,0xfd,0xa7,0x9a,0x86,0xf6,0xe6,
    0x98,0xce,0xd7,0x59,0xc3,0xff,0x9b,0x64,0x77,0x33,0x8f,0x3d,0xa4,0xf9,0xcd,0x85,
    0x14,0xea,0x99,0x82,0xcc,0xaf,0xb3,0x41,0xb2,0x38,0x4d,0xd9,0x02,0xf3,0xd1,0xab,
    0x7a,0xc6,0x1d,0xd2,0x9c,0x6f,0x21,0xba,0x5b,0x86,0x2f,0x37,0x30,0xe3,0x7c,0xfd,
    0xc4,0xfd,0x80,0x6c,0x22,0xf2,0x21
};

// ---- TV #3: key=1c9240a5..., nonce=0..02, counter=42, Jabberwocky ----
static const uint32_t tv3_key[8]   = {
    0xa540921c,0x8ad355eb,0x868833f3,0xf0b5f604,
    0xc1173947,0x09802b40,0xbc5cca9d,0xc0757020
};
static const uint32_t tv3_nonce[3] = { 0x00000000,0x00000000,0x02000000 };
#define TV3_CTR 42
#define TV3_LEN 127
static const uint8_t tv3_plaintext[TV3_LEN] =
    "'Twas brillig, and the slithy toves\n"
    "Did gyre and gimble in the wabe:\n"
    "All mimsy were the borogoves,\n"
    "And the mome raths outgrabe.";
static const uint8_t tv3_expected[TV3_LEN] = {
    0x62,0xe6,0x34,0x7f,0x95,0xed,0x87,0xa4,0x5f,0xfa,0xe7,0x42,0x6f,0x27,0xa1,0xdf,
    0x5f,0xb6,0x91,0x10,0x04,0x4c,0x0d,0x73,0x11,0x8e,0xff,0xa9,0x5b,0x01,0xe5,0xcf,
    0x16,0x6d,0x3d,0xf2,0xd7,0x21,0xca,0xf9,0xb2,0x1e,0x5f,0xb1,0x4c,0x61,0x68,0x71,
    0xfd,0x84,0xc5,0x4f,0x9d,0x65,0xb2,0x83,0x19,0x6c,0x7f,0xe4,0xf6,0x05,0x53,0xeb,
    0xf3,0x9c,0x64,0x02,0xc4,0x22,0x34,0xe3,0x2a,0x35,0x6b,0x3e,0x76,0x43,0x12,0xa6,
    0x1a,0x55,0x32,0x05,0x57,0x16,0xea,0xd6,0x96,0x25,0x68,0xf8,0x7d,0x3f,0x3f,0x77,
    0x04,0xc6,0xa8,0xd1,0xbc,0xd1,0xbf,0x4d,0x50,0xd6,0x15,0x4b,0x6d,0xa7,0x31,0xb1,
    0x87,0xb5,0x8d,0xfd,0x72,0x8a,0xfa,0x36,0x75,0x7a,0x79,0x7a,0xc1,0x88,0xd1
};


//  Buffers (max size es de TV2_LEN = 375) 
#define BUF_MAX TV2_LEN
static uint8_t zeros[BUF_MAX];      // ceros para generar keystream
static uint8_t keystream[BUF_MAX];  // keystream puro (encrypt(zeros))
static uint8_t ciphertext[BUF_MAX]; // texto cifrado
static uint8_t recovered[BUF_MAX];  // texto recuperado (decrypt = encrypt)

/* run_vector: Funcion que ejecuta un vector completo:
*  Muestra clave, nonce, contador, keystream, ciphertext y verifica que se recupere el mensaje original
*  Parametros:
*  - title: titulo descriptivo del vector de prueba
*  - key: clave de 256 bits (8 palabras de 32 bits)
*  - nonce: nonce de 96 bits (3 palabras de 32 bits)
*  - counter: contador inicial
*  - plaintext: mensaje original a cifrar
*  - expected: ciphertext esperado del RFC 8439
*  - len: longitud del plaintext/ciphertext a procesar
*  Pasos:
* 1. Muestra clave, nonce, contador
* 2. Genera y muestra el keystream (cifrar ceros con los mismos parametros)
* 3. Cifra el plaintext y lo compara con el esperado del RFC
* 4. Descifra y verifica la recuperacion del mensaje original

*/

static void run_vector(const char     *title,
                       const uint32_t *key,
                       const uint32_t *nonce,
                       uint32_t        counter,
                       const uint8_t  *plaintext,
                       const uint8_t  *expected,
                       uint32_t        len)
{
    print_string("\033[1;35m"); print_string(title); print_string("\033[0m\n");

    // Parametros del vector
    print_string("\033[1;33m  Clave (256 bits):\033[0m\n");
    print_key(key);
    print_string("\033[1;33m  Nonce (96 bits):\033[0m\n");
    print_nonce(nonce);
    print_string("\033[1;33m  Contador inicial: \033[0m");
    print_dec((int)counter); print_char('\n');

    // -- Keystream: cifrar ceros con los mismos parametros --
    print_string("\033[1;33m  Keystream (primeros "); print_dec((int)len);
    print_string(" bytes):\033[0m\n");
    chacha20_encrypt(keystream, zeros, len, key, counter, nonce);
    print_buf_hex(keystream, len);

    // -- Cifrado --
    print_string("\033[1;33m  Plaintext:\033[0m\n");
    print_buf_hex(plaintext, len);

    chacha20_encrypt(ciphertext, plaintext, len, key, counter, nonce);

    print_string("\033[1;33m  Ciphertext obtenido:\033[0m\n");
    print_buf_hex(ciphertext, len);

    print_string("\033[1;33m  Ciphertext esperado (RFC 8439):\033[0m\n");
    print_buf_hex(expected, len);

    check_buf("cifrado vs RFC", ciphertext, expected, len);

    // -- Descifrado (round-trip) --
    chacha20_encrypt(recovered, ciphertext, len, key, counter, nonce);

    print_string("\033[1;33m  Descifrado (recovered):\033[0m\n");
    print_buf_hex(recovered, len);

    check_buf("round-trip: recovered == plaintext", recovered, plaintext, len);

    print_char('\n');
}


void main(void) {
    print_string("\033[1;35m-------Programa Principal en C con los vectores de prueba de RFC 8439 Apendice A.2 ---------\033[0m\n\n");

    run_vector("--- TV #1: key=0, nonce=0, counter=0, plaintext=64 ceros ---",
               tv1_key, tv1_nonce, TV1_CTR,
               tv1_plaintext, tv1_expected, TV1_LEN);

    run_vector("--- TV #2: key=0..01, nonce=0..02, counter=1, texto IETF ---",
               tv2_key, tv2_nonce, TV2_CTR,
               tv2_plaintext, tv2_expected, TV2_LEN);

    run_vector("--- TV #3: key=1c9240a5, nonce=0..02, counter=42, Jabberwocky ---",
               tv3_key, tv3_nonce, TV3_CTR,
               tv3_plaintext, tv3_expected, TV3_LEN);

    print_string("\033[1;35m==========================================================\033[0m\n");
    print_string("Total: ");
    print_dec(tests_passed); print_char('/'); print_dec(tests_run);
    print_string(" bytes correctos\n");

    if (tests_passed == tests_run)
        print_string("\033[1;32m[PASSED] Todos los vectores RFC 8439 A.2 verificados\033[0m\n");
    else
        print_string("\033[1;31m[FAILED] Alguna verificacion fallo\033[0m\n");

    while (1) { __asm__ volatile("nop"); }
}
