### Proyecto Individual: Implementación de ChaCha20 en Ensamblador RISC-V

El presente proyecto implementa el algoritmo de cifrado de flujo ChaCha20 en ensamblador RISC-V de 32 bits, siguiendo las especificaciones del RFC 8439. Las tres funciones principales del algoritmo (`chacha20_quarter_round`, `chacha20_block` y `chacha20_encrypt`) están escritas en ensamblador RISC-V y son llamadas desde un programa en C que valida los resultados contra los vectores de prueba oficiales del RFC. El entorno de desarrollo utiliza un contenedor Docker con QEMU para emular el procesador RISC-V y GDB para depuración.

---

## 1. Estructura del proyecto

```
.
├── Dockerfile
├── README.md
├── run.sh
└── examples/
    ├── asm-only/                        # Ejemplo de ensamblador puro
    │   ├── build.sh
    │   ├── linker.ld
    │   ├── README.md
    │   ├── run-qemu.sh
    │   └── test.s                       # Suma del 1 al 10 en ensamblador
    └── c-asm/                           # Implementación ChaCha20 en C + ensamblador
        ├── startup.s                    # Inicialización de pila y llamada a main
        ├── linker.ld                    # Script de enlazado
        ├── build.sh                     # Compila el programa principal
        ├── build-test.sh                # Compila los tres programas de prueba unitaria
        ├── run-qemu.sh                  # Ejecuta el programa principal con servidor GDB
        ├── run-test.sh                  # Ejecuta las pruebas de chacha20_quarter_round
        ├── run-test-block.sh            # Ejecuta las pruebas de chacha20_block
        ├── run-test-encrypt.sh          # Ejecuta las pruebas de chacha20_encrypt
        ├── debug_test.gdb               # Script GDB con puntos de ruptura predefinidos
        ├── chacha20_quarter_round.s     # Operación quarter round en ensamblador RISC-V
        ├── block_chacha20.s             # Función chacha20_block en ensamblador RISC-V
        ├── chacha20_encrypt.s           # Función chacha20_encrypt en ensamblador RISC-V
        ├── chacha20_main.c              # Programa principal con vectores RFC 8439
        ├── test_quarter_round.c         # Prueba unitaria: quarter round
        ├── test_block_chacha20.c        # Prueba unitaria: bloque ChaCha20
        ├── test_encrypt_chacha20.c      # Prueba unitaria: cifrado y descifrado
        ├── example.c                    # Ejemplo básico de integración C + ensamblador
        └── math_asm.s                   # Ejemplo de función aritmética en ensamblador
```

- `examples/c-asm/` contiene la implementación completa de ChaCha20 y sus pruebas unitarias
- `examples/asm-only/` contiene un ejemplo introductorio de ensamblador puro
- `Dockerfile` define la imagen con el emulador QEMU y el toolchain RISC-V
- `run.sh` automatiza la construcción de la imagen y la ejecución del contenedor

---

## 2. Pasos para construir y ejecutar el proyecto

### Requisitos previos

- Instalar **Docker Desktop**:

    https://docs.docker.com/get-started/get-docker/

- Clonar el repositorio con git o descargar todo el proyecto en zip:

    ```bash
    git clone https://github.com/yen31b/chacha20-rvqemu-arqui-I.git
    ```

- Si el sistema operativo es **Windows**, instalar WSL (Windows Subsystem for Linux) para poder ejecutar los scripts de bash y las herramientas de Linux directamente desde Windows, los pasos de instalación:

    https://learn.microsoft.com/en-us/windows/wsl/install#install-wsl-command


- Si el sistema operativo es **Linux**, el proyecto puede correr sin necesidad de WSL.


- Para una mejor experiencia de usuario en terminal se recomienda descargar la app Terminal de Microsoft Store:

    https://apps.microsoft.com/detail/9N0DX20HK701?hl=en-us&gl=CR&ocid=pdpshare



### Paso 1: Construir el contenedor

El script `run.sh` construye la imagen `rvqemu` y crea un contenedor interactivo que monta el directorio del proyecto en `/home/rvqemu-dev/workspace`.

```bash
chmod +x run.sh

./run.sh
```

### Paso 2: Compilar la implementación ChaCha20

```bash
cd /home/rvqemu-dev/workspace/examples/c-asm

./build.sh
```

Esto genera `chacha20_main.elf`, el programa principal que ejecuta los tres vectores de prueba del RFC 8439 e imprime los resultados en terminal.

### Paso 3: Ejecutar el programa principal con QEMU y GDB

```bash
# En una terminal: iniciar QEMU con servidor GDB en puerto 1234
chmod +x run-qemu.sh
./run-qemu.sh

# En otra terminal dentro del mismo contenedor: conectar GDB
cd /home/rvqemu-dev/workspace/examples/c-asm
gdb-multiarch chacha20_main.elf
```

Una vez en GDB:

```gdb
target remote :1234
continue
```

El programa imprime los resultados de los tres vectores de prueba RFC 8439 directamente en la terminal donde corre QEMU.

---




## 3. Instrucciones para ejecutar los casos de prueba para cada funcion por separado

El proyecto incluye tres programas de prueba unitaria, uno por cada función del algoritmo. Cada uno verifica los resultados contra los vectores oficiales del RFC 8439 e imprime `PASS` o `FAIL`.

### Paso previo: compilar las pruebas

Las pruebas dependen de los archivos objeto generados por `build.sh`. Ejecutar primero:

```bash
cd /home/rvqemu-dev/workspace/examples/c-asm

./build.sh
./build-test.sh
```

Esto genera los tres ELF de prueba: `test_quarter_round.elf`, `test_block_chacha20.elf` y `test_encrypt_chacha20.elf`.

---

### Prueba 1: `chacha20_quarter_round` 

Ejecuta dos vectores de prueba sobre la función quarter round:

- **Vector 1 - RFC 8439 2.1.1. Test Vector for the ChaCha Quarter Round** : verifica la aritmética básica de la operación.
  - Entrada: `a=0x11111111  b=0x01020304  c=0x9b8d6f43  d=0x01234567`
  - Salida esperada: `a=0xea2a92f4  b=0xcb1cf8ce  c=0x4581472e  d=0x5881c4bb`

- **Vector 2 - RFC 8439 2.2.1 Test Vector for the Quarter Round on the ChaCha**: verifica `QUARTERROUND(2,7,8,13)` aplicado sobre el estado interno de ChaCha20.
  - Entrada: `a=0x516461b1  b=0x2a5f714c  c=0x53372767  d=0x3d631689`
  - Salida esperada: `a=0xbdb886dc  b=0xcfacafd2  c=0xe46bea80  d=0xccc07c79`

```bash
chmod +x run-test.sh
./run-test.sh
```

Presionar `Ctrl+A` y luego `X` para salir de QEMU.

---

### Prueba 2: `chacha20_block`

Ejecuta un vector de prueba sobre la función de bloque:

- **Vector 1 - RFC 8439 2.3.2 — ChaCha20 block function test vector**: verifica que el bloque genera los 64 bytes de keystream correctos.
  - Clave: `00 01 02 ... 1f` (32 bytes ascendentes), contador: `1`, nonce: `00000009 0000004a 00000000`
  - Compara las 16 palabras de salida contra los valores del RFC.

```bash
chmod +x run-test-block.sh
./run-test-block.sh
```

Presionar `Ctrl+C` para salir de QEMU.

---

### Prueba 3: `chacha20_encrypt` - 3 vectores RFC 8439 Appendix A.2

Ejecuta tres vectores de prueba sobre la función de cifrado:

- **Vector 1 (TV#1)**: clave todo ceros, nonce todo ceros, contador `0`, plaintext de 64 bytes en cero.
- **Vector 2 (TV#2)**: clave con último byte `0x01`, nonce con último byte `0x02`, contador `1`, plaintext de 375 bytes.
- **Vector 3 (TV#3)**: clave `1c9240a5...`, nonce con último byte `0x02`, contador `42`, plaintext de 127 bytes.

Cada vector verifica que el ciphertext generado coincida byte a byte con el esperado del RFC.

```bash
chmod +x run-test-encrypt.sh
./run-test-encrypt.sh
```

Presionar `Ctrl+C` para salir de QEMU.

---

## 4. Instrucciones para ejecutar los casos de prueba y verificar los vectores del RFC

El programa `chacha20_main.elf` ejecuta tres vectores de prueba del RFC 8439 (Appendix A.2) sobre la función `chacha20_encrypt`. Por cada vector imprime por UART la clave, nonce, contador, el keystream generado, el plaintext, el ciphertext obtenido, el ciphertext esperado según el RFC y el resultado del descifrado (round-trip), verificando que descifrar el ciphertext recupera el plaintext original.

Los tres vectores son:

| Vector | Clave | Nonce | Contador | Longitud |
|--------|-------|-------|----------|----------|
| TV#1 | todo ceros (32 bytes) | todo ceros (12 bytes) | 0 | 64 bytes |
| TV#2 | último byte `0x01` | último byte `0x02` | 1 | 375 bytes (texto IETF) |
| TV#3 | `1c9240a5...` | último byte `0x02` | 42 | 127 bytes (Jabberwocky) |

```bash
# En una terminal iniciar QEMU con servidor GDB
chmod +x run-qemu.sh
./run-qemu.sh

# En la otra terminal conectar GDB y continuar la ejecución
cd /home/rvqemu-dev/workspace/examples/c-asm
gdb-multiarch chacha20_main.elf
```

Una vez en GDB:

```gdb
target remote :1234
continue
```

El programa imprime los resultados directamente en la Terminal 1.

---

## 5. Instrucciones para abrir depuración con GDB

El archivo `debug_test.gdb` configura automáticamente los puntos de ruptura en las funciones principales del algoritmo.

### Paso 1: Iniciar QEMU con servidor GDB

```bash
cd /home/rvqemu-dev/workspace/examples/c-asm
./run-qemu.sh
```

### Paso 2: Conectar GDB desde otra terminal

```bash
cd /home/rvqemu-dev/workspace/examples/c-asm
gdb-multiarch chacha20_main.elf -x debug_test.gdb
```

El script `debug_test.gdb` establece puntos de ruptura en `_start`, `main`, `chacha20_quarter_round`, `chacha20_block` y `chacha20_encrypt`, y activa las vistas de ensamblador y registros automáticamente.

### Comandos útiles de GDB

```gdb
break main              - break at program entry
break run_vector        - break at each test vector (call three times for each test vector)
break chacha20_encrypt  - break dentro de la funcion de encriptado en ASM
info registers         - show register values
step / next / continue - commands to move through the code
layout asm             - ASM view
layout regs            - register view
monitor quit           - quit remote application (qemu)
q                      - quit GDB
Inspect buffers (size for each test vector):
x/64xb  &keystream     - TV#1 keystream  (64 bytes)
x/375xb &keystream     - TV#2 keystream  (375 bytes)
x/127xb &keystream     - TV#3 keystream  (127 bytes)
x/64xb  &ciphertext    - TV#1 ciphertext (64 bytes)
x/375xb &ciphertext    - TV#2 ciphertext (375 bytes)
x/127xb &ciphertext    - TV#3 ciphertext (127 bytes)
x/64xb  &recovered     - TV#1 recovered  (64 bytes)
x/375xb &recovered     - TV#2 recovered  (375 bytes)
x/127xb &recovered     - TV#3 recovered  (127 bytes)
```

---

