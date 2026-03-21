# Entorno de desarrollo RISC-V con QEMU y GDB

Este proyecto proporciona un entorno completo para desarrollo y depuración de programas bare-metal en arquitectura RISC-V de 32 bits, utilizando QEMU y GDB dentro de un contenedor Docker.

---

## 1. Estructura del proyecto

```
.
├── Dockerfile
├── run.sh
├── examples/           # Ejemplos de código
│   ├── asm-only/      # Ejemplo de ensamblador puro
│   │   ├── test.s
│   │   ├── linker.ld
│   │   ├── build.sh
│   │   └── run-qemu.sh
│   └── c-asm/         # Ejemplo de C + ensamblador
│       ├── example.c
│       ├── math_asm.s
│       ├── linker.ld
│       ├── build.sh
│       └── run-qemu.sh
└── README.md
```

- `examples/` contiene diferentes ejemplos de programas RISC-V
- `Dockerfile` define la imagen que incluye el emulador QEMU y el toolchain RISC-V
- `run.sh` automatiza la construcción de la imagen y la ejecución del contenedor

## Ejemplos disponibles

### Ensamblador puro (`examples/asm-only/`)
test.s es un programa simple escrito completamente en ensamblador que calcula la suma del 1 al 10.

chacha20_quarter_round.s es el programa donde se implementó la operación quarter round.

chacha20_block programa principal del bloque



### C + Ensamblador (`examples/c-asm/`)
Programa en C que llama funciones escritas en ensamblador, demostrando la integración entre ambos lenguajes. Este ejemplo incluye un archivo de inicio (startup.s) que inicializa la pila y llama a la función main de C, ya que los programas C necesitan un entorno de ejecución básico antes de ejecutar el código principal.

---

## 2. Inicio rápido

### Paso 1: Construir el contenedor
```bash
chmod +x run.sh
./run.sh
```

### Paso 2: Elegir y compilar un ejemplo
```bash
# Para el ejemplo de ensamblador puro
cd /home/rvqemu-dev/workspace/examples/asm-only
./build.sh

# Para el ejemplo de C + ensamblador
cd /home/rvqemu-dev/workspace/examples/c-asm
./build.sh
```

### Paso 3: Ejecutar con QEMU y depurar
```bash
# En una terminal: iniciar QEMU con servidor GDB
./run-qemu.sh

# En otra terminal: conectar GDB
docker exec -it rvqemu /bin/bash
cd /home/rvqemu-dev/workspace/examples/[ejemplo-elegido]
gdb-multiarch [archivo-elf]
```

---

## 3. Uso detallado

### Construcción del contenedor
El script `run.sh` construye la imagen `rvqemu` y crea un contenedor interactivo que monta el directorio del proyecto en `/home/rvqemu-dev/workspace`.

### Compilación
Cada ejemplo incluye un script `build.sh` que maneja la compilación automáticamente.

**Opciones de compilación utilizadas**:
- `-march=rv32im`: arquitectura RISC-V 32 bits con extensiones I y M
- `-mabi=ilp32`: ABI ILP32
- `-nostdlib -ffreestanding`: entorno bare-metal
- `-g`: información de depuración para GDB

### Ejecución y depuración
1. **QEMU**: `run-qemu.sh` inicia QEMU con servidor GDB en puerto 1234
2. **GDB**: Conectar desde otra terminal para depuración interactiva

**Comandos útiles de GDB**:
```gdb
target remote :1234    # Conectar al servidor GDB
break _start           # Punto de ruptura al inicio
continue               # Continuar ejecución
layout asm             # Vista de ensamblador
layout regs            # Vista de registros
step                   # Ejecutar siguiente instrucción
info registers         # Mostrar registros
monitor quit           # Finalizar sesión
```

---

## 4. Detalles de los ejemplos

Para información específica sobre cada ejemplo, consultar:
- [`examples/asm-only/README.md`](examples/asm-only/README.md) - Ensamblador puro
- [`examples/c-asm/README.md`](examples/c-asm/README.md) - C + Ensamblador
- [`examples/README.md`](examples/README.md) - Información general
