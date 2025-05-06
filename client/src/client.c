#include "client.h"
#include <readline/readline.h>
#include <readline/history.h>

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"
	log_info(logger, "Hola! Soy un log");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();
	if (config == NULL) {
        log_error(logger, "No se pudo abrir el archivo cliente.config");
        abort();
    }

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "VALOR");

	// Loggeamos el valor de config
	log_info(logger, "IP:%s", ip);	
	log_info(logger, "PUERTO:%s", puerto);
	log_info(logger, "VALOR:%s", valor);
	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);
	if (conexion == -1){
		log_error(logger,"No se pudo establecer la conexion");
		return -1;
	}

	// Enviamos al servidor el valor de CLAVE como mensaje
	char* valor_clave = config_get_string_value(config, "CLAVE");
	enviar_mensaje(valor_clave, conexion);

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
	return 0;
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger = log_create("tp0.log", "cliente", true, LOG_LEVEL_INFO);
	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config = config_create("cliente.config");
	
	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;

	// La primera te la dejo de yapa
	//leido = readline("> ");

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while (1)
	{
		leido = readline("> ");

		if (leido == NULL || strcmp(leido, "") == 0){
			free(leido);
			break;
		}
		if(leido){
			add_history(leido);
		}
		if(!strncmp(leido, "exit", 4)){
			free(leido);
			break;
		}

		log_info(logger, "%s", leido);
		free(leido);
	}
	
	// ¡No te olvides de liberar las lineas antes de regresar!

}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete= crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete
	while (1){
		leido = readline("> ");

		if (leido == NULL || strlen(leido) == 0) {
			free(leido);
			break;
		}

		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		free(leido);
	}

	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */

	//Liberamos la conexion con el sv
	liberar_conexion(conexion);

	//Destruimos el logger y la config
	if(logger != NULL){
		log_destroy(logger);
	}
	if(config != NULL){
		config_destroy(config);
	}
}
