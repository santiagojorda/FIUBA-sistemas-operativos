# shell

Repositorio para el esqueleto del [TP: shell](https://fisop.github.io/website/tps/shell) del curso Mendez-Fresia de **Sistemas Operativos (7508) - FIUBA**

## Respuestas teóricas

Utilizar el archivo `shell.md` provisto en el repositorio

## Compilar

```bash
make
```

## Pruebas

- Ejecutar todas las pruebas

```bash
make test
```

- Ejecutar una **única** prueba

```bash
make test-TEST_NAME
```

Por ejemplo:

```bash
make test-env_empty_variable
```

Cada identificador de una prueba se muestra entre paréntesis `(TEST_NAME)` al lado de cada _test_ cuando se ejecutan todas las pruebas.

```
=== Temporary files will be stored in: /tmp/tmp0l10br1k-shell-test ===

PASS 1/26: cd . and cd .. work correctly by checking pwd (no prompt) (cd_back)
PASS 2/26: cd works correctly by checking pwd (no prompt) (cd_basic)
PASS 3/26: cd with no arguments takes you home (/proc/sys :D) (cd_home)
PASS 4/26: empty variables are not substituted (env_empty_variable)
...
```

## Ejecutar

```bash
./sh
```

## Linter

```bash
make format
```

Para efectivamente subir los cambios producidos por `make format`, hay que hacer `git add .` y `git commit`.
