#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
 
#define FD_READ 0
#define FD_WRITE 1

void
comprobar_argumentos_entrada(int argc)
{
    if (argc < 2) {
        perror("No se ingreso ningun numero.\n");
        exit(EXIT_FAILURE);
    }
}

int
obtener_numero_ingresado(int argc, char const *argv[])
{
    comprobar_argumentos_entrada(argc);
    return atoi(argv[1]);
}

void
crear_pipe(int* fds)
{
    ssize_t res = pipe(fds);
    if (res < 0) {
        perror("Hubo un error al crear pipe\n");
        exit(EXIT_FAILURE);
    }
}

int
es_proceso_padre(pid_t pid)
{
    return pid > 0;
}

int
es_proceso_hijo(pid_t pid)
{
    return pid == 0;
}

void
cerrar_pipe(int* fds)
{
    close(fds[FD_WRITE]);
    close(fds[FD_READ]);
}

int
se_pudo_leer(ssize_t res_lectura)
{
    return res_lectura > 0;
}

int
no_pudo_escribir(ssize_t res_escritura)
{
    return res_escritura < 0;
}

void
lanzar_error_lectura()
{
    perror("Hubo error de escritura\n");
    exit(EXIT_FAILURE);
}

void
lanzar_error_escritura()
{
    perror("Hubo error de escritura\n");
    exit(EXIT_FAILURE);
}

ssize_t
escribir(int* fds, int valor)
{
    return write(fds[FD_WRITE], &valor, sizeof(valor));
}

ssize_t
leer(int* fds, int* valor)
{
    return read(fds[FD_READ], valor, sizeof(*valor));
}

int
no_hay_mas_para_leer(ssize_t res)
{
    return res == 0;
}

void
lanzar_error_fork(int* fds)
{
    cerrar_pipe(fds);
    perror("Hubo erro al crear fork\n");
    exit(EXIT_FAILURE);
}

void
finalizar_filtro(int* fds_izquierda)
{ 
    close(fds_izquierda[FD_READ]);
    exit(EXIT_SUCCESS);
}

int
no_son_multiplos(int numerador, int denominador)
{
    return (numerador % denominador != 0);
}

void
filtrar(int* fds_izquierda)
{

    int primo;
    ssize_t res_lectura = leer(fds_izquierda, &primo); 

    if (no_hay_mas_para_leer(res_lectura)) {
        finalizar_filtro(fds_izquierda);
    }

    printf("primo %d\n", primo);
    fflush(stdout);

    int fds_derecha[2];
    crear_pipe(fds_derecha);

    pid_t pid = fork();

    if (es_proceso_padre(pid)) {
        close(fds_derecha[FD_READ]);
        int numero_leido;
        res_lectura = leer(fds_izquierda, &numero_leido);
        while (se_pudo_leer(res_lectura)) {   
            if (no_son_multiplos(numero_leido, primo)) {
                ssize_t res_escritura = escribir(fds_derecha, numero_leido);
                if( no_pudo_escribir(res_escritura)) {
                    close(fds_derecha[FD_WRITE]);
                    close(fds_izquierda[FD_READ]);
                    lanzar_error_escritura();
                }
            }
            res_lectura = leer(fds_izquierda, &numero_leido);
        }
        close(fds_derecha[FD_WRITE]);
        close(fds_izquierda[FD_READ]);
        wait(NULL);
    }

    else if (es_proceso_hijo(pid)) {
        close(fds_derecha[FD_WRITE]);
        close(fds_izquierda[FD_READ]);
        filtrar(fds_derecha);
        close(fds_derecha[FD_READ]);
    }

    else {
        close(fds_izquierda[FD_READ]);
        lanzar_error_fork(fds_derecha);
    }

    return;
}

int
main(int argc, char const *argv[])
{
    int numero_final = obtener_numero_ingresado(argc, argv);

    int fds_derecha[2];
    crear_pipe(fds_derecha);

    pid_t pid = fork();

    if (es_proceso_padre(pid)) {
        close(fds_derecha[FD_READ]);
        for (int i = 2; i <= numero_final; i++) {   
            ssize_t res_escritura = escribir(fds_derecha, i);
            if (no_pudo_escribir(res_escritura)) {
                close(fds_derecha[FD_WRITE]);
                lanzar_error_escritura();
            }
        }
        close(fds_derecha[FD_WRITE]);
        wait(NULL);
    } 
    
    else if (es_proceso_hijo(pid)) {
        close(fds_derecha[FD_WRITE]);
        filtrar(fds_derecha);
        close(fds_derecha[FD_READ]);
    }
    
    else {
        lanzar_error_fork(fds_derecha);
    }

    return 0;
}

