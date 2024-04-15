#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#ifndef NARGS
#define NARGS 4
#endif

#define RETURN_ERROR -1
#define EOF_READ_LINE -1


void
comprobar_ingreso_comandos(int argc)
{
	if (argc == 1) {
		printf("No hay comandos, porfavor ingrese uno\n");
		exit(EXIT_FAILURE);
	}
}

ssize_t
leer_linea(char **linea, size_t *tam_linea)
{
	return getline(linea, tam_linea, stdin);
}

int
se_leyo_contenido(int cant_caracteres)
{
	return cant_caracteres != EOF_READ_LINE;
}

int
ultima_linea_string_es_salto(char *linea, int cant_caracteres)
{
	return (linea[cant_caracteres - 1] == '\n');
}

int
sobraron_argumentos(int cant_args, char* linea)
{
	return (cant_args > 1 && linea != NULL);
}


int
el_comando_se_lleno_de_argumentos(int cant_args)
{
	return (cant_args == NARGS + 1);
}


void
resetear_lista(int* cant_argumentos){
	*cant_argumentos = 1;
}

void
agregar_argumento(char **args, int* cant_args, char* linea)
{
	*args[*cant_args] = *linea;
	(*cant_args)++;
}

int
es_proceso_hijo(pid_t pid)
{
	return (pid == 0);
}

int
es_proceso_padre(pid_t pid)
{
	return (pid > 0);
}


int
ejecutar_comando(char *args[])
{
	pid_t pid = fork();

	if (es_proceso_hijo(pid)) {
		execvp(args[0], args);
	} 
	
	else if (es_proceso_padre(pid)) {
		wait(NULL);
	}

	else {
		perror("Hubo error al hacer fork\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}

int
main(int argc, char *argv[])
{
	comprobar_ingreso_comandos(argc);

	char *linea;
	size_t tam_linea;
	int cant_caracteres;

	char *lista_args[NARGS + 2] = { 0 };
	lista_args[NARGS + 1] = NULL;

	lista_args[0] = argv[1];

	int cant_args = 1;
	cant_caracteres = getline(&linea, &tam_linea, stdin);


	while (se_leyo_contenido(cant_caracteres)) {
		if (ultima_linea_string_es_salto(linea, cant_caracteres)) {
			linea[cant_caracteres - 1] = '\0';
		}

		lista_args[cant_args] = linea;
		cant_args++;

		if (el_comando_se_lleno_de_argumentos(cant_args)) {
			ejecutar_comando(lista_args);
			resetear_lista(&cant_args);
		}

		linea = NULL;

		cant_caracteres = getline(&linea, &tam_linea, stdin);
	}


	if (cant_args > 1 && linea != NULL) {
		lista_args[cant_args] = NULL;	
		ejecutar_comando(lista_args);
	}

	return 0;
}


