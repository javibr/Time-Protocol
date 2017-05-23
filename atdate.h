#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <arpa/inet.h>
#include <signal.h>

//Declaracion de funciones principales
void udp_client(char* hostname, int port); //Funcion que recoge el codigo para consultas UDP.
void tcp_client(char* hostname, int port); //Funcion que recoge el codigo para consultas TCP.
void server(int port);//Funcion que recoge el codigo para modo servidor.

//Declaracion de cabeceras de las funciones auxiliares:
void hora_Texto(char* string, int hAct);
int hora();
void interrupcion(int sig);
int entrada(int argc, char *argv[]);
