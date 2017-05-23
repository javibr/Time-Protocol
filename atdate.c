#include "atdate.h"


int debug_mode = 0; // Variable que indica si estamos en modo depuracion. Si es 0 es false (no esta en modo debug)
int option = 1; // Modo consulta UDP por defecto
char *host = NULL; // Nombre del host.
int port = 37; //Puerto por defecto del protocolo Time
int socket_fd; // Descriptor del socket
int connected; // Variable que indica si estamos en modo conectado.
int veces=0;

/* Funcion que activa el modo consulta de la hora por protocolo UDP
 * Parametros:
 *     hostname: Puntero a nombre de host.
 *     port: Puerto por donde nos vamos a conectar.
 *
 */

void udp_client(char* hostname, int port){
	struct sockaddr_in serv_addr; // Estructura de la direccion del servidor
	struct hostent *host; // Estructura del host 
	char rec_buf[1024]; // Buffer de recepcion
	int bytes_read; // Bytes leidos
	int addr_len; // Longitud de la direccion
	int resp; // Variable donde guardamos la respuesta
	char string[50]; // Variable donde guardaremos la hora
	int i;
	// Almacenamos en host la IP del host pasado por parametro.
	host = (struct hostent*)gethostbyname(hostname);
	
	// Creamos un socket y guardamos su descriptor con los valores de la teoria.
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	// Comprobacion de errores
	  if (socket_fd == -1) {
		  printf("Error en la creacion del socket\n");
		  exit(1);
	  }
	
	  
	// Rellenamos la estructura  
	serv_addr.sin_family = AF_INET; // AF_INET segun la teoria.
	serv_addr.sin_port = htons(port); // Puerto 37 por defecto (TIME).
	serv_addr.sin_addr = *((struct in_addr *) host->h_addr); /*Almacenamos la IP del servidor. Tenemos que tener en cuenta que 
        la direccion real se almacena en la variable h_addr de la estructura hostent, por lo que empleamos el metodo del puntero.*/
	
	// Enviamos un datagrama vacio (lo indica el enunciado) a una direccion IP que es la que obtuvimos con la serv_addr de antes
	/* int sendto(int sockfd, const void *msg, int len, unsigned int flags,
 const struct sockaddr *to, int tolen); */
 
 
	sendto(socket_fd, NULL, 0, 0, (struct sockaddr *) &serv_addr,sizeof(struct sockaddr));
	
	// Tamaño de la estructura sockaddr que usaremos para los datagramas que recibamos
	addr_len = sizeof(struct sockaddr); 
	
	// Almacenamos el numero de bytes que hemos leido en bytes_read, ademas de la direccion en el buffer.
	bytes_read = recvfrom(socket_fd, &rec_buf, 1024, 0, (struct sockaddr *) &serv_addr, &addr_len);
	
	// Si estamos en modo depuracion
	  if (debug_mode == 1) {
		  printf("Bytes de servidor: %d\n", bytes_read);
	  }
	
	// Guardamos lo que hay en el buffer en respuesta para poder manipularlo mejor
	memcpy(&resp, &rec_buf, 4);
	
	// Convertimos a string
	hora_Texto(string, ntohl(resp));
	
	// Si estamos en modo depuracion
	  if (debug_mode == 1) {
		  printf("Bytes recibidos de servidor: [%X]\n", ntohl(resp));
	  }
	  
	// Imprimimos la hora
	printf("%s\n",string);
	
	// Cerramos el descriptor de socket
	close(socket_fd);
}
	
	void tcp_client(char* hostname, int port){
		
	struct sockaddr_in serv_addr; // Estructura de la direccion del servidor
	struct hostent *host; // Estructura del host 
	char rec_buf[1024]; // Buffer de recepcion
	int bytes_read; // Bytes leidos
	int addr_len; // Longitud de la direccion
	int resp; // Variable donde guardamos la respuesta
	char string[50]; // Variable donde guardaremos la hora
	int err; // Variable para comprobacion de errores
		
		int aux;
		
	// Almacenamos en host la IP del host pasado por parametro.
	host = (struct hostent*)gethostbyname(hostname);
	
	// Creamos un socket y guardamos su descriptor con los valores de la teoria.
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	// Comprobacion de errores
	  if (socket_fd == -1) {
		  printf("Error en la creacion del socket\n");
		  exit(1);
	  }
	
	  
	// Rellenamos la estructura  
	serv_addr.sin_family = AF_INET; // AF_INET segun la teoria.
	serv_addr.sin_port = htons(port); // Puerto 37 por defecto (TIME).
	serv_addr.sin_addr = *((struct in_addr *) host->h_addr); /*Almacenamos la IP del servidor. Tenemos que tener en cuenta que 
        la direccion real se almacena en la variable h_addr de la estructura hostent, por lo que empleamos el metodo del puntero.*/	
		
		// Vamos a conectarnos. Hay error si fallamos.
	err = connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
	  if (err == -1) {
		  printf("Error en la conexion\n");
		  exit(1); //Terminamos el programa.
	  }
	  else {
	// Si nos conectamos,nos mantenemos a la escucha (Bucle infinito).
		while (1) {
        		
		  // Al haber empleado connect(), no necesitamos hacer recvfrom, puesto que ya conocemos de quien recibimos.  
		  bytes_read = recv(socket_fd, rec_buf, 1024, 0); //recibo datos
		  // En TCP tenemos que verificar si estamos recibiendo algo o no (Conexion fiable)
			// Si recibimos algo
			if(bytes_read > 0){
				// Lo copiamos en la respuesta
				memcpy(&resp, &rec_buf, 4);
				
				
				
				/*Modificacion para que TCP_client envie al server lo que recibe
				
			    aux = send(socket_fd, &rec_buf, 1024, 0);
				printf("aux es %d\n",aux);
				*/
				
				
				
				// Lo convertimos a string para que lo podamos leer
            			hora_Texto(string, ntohl(resp)); 
				// Si estamos en modo depuracion
				if (debug_mode == 1) {
                			printf("Recibido en el servidor es: %X\n", ntohl(resp));
            			}
			// Imprimimos la cadena
            		printf("%s\n",string);
			
			}
			
			// Si no recibimos nada, podemos cerrar
			else {	
			  // Cerramos el descriptor de socket
			  close(socket_fd);
			  // Imprimimos mensaje de error
			  printf("ERROR: No hemos recibido nada\n");		
			  exit(1);
			}
		}
  	}	
}
	
	void server(int port){
		
	int flag = 1; // Variable que se usara en las opciones del descriptor de socket
	struct sockaddr_in serv_addr; // Estructura de la direccion del servidor en formato socket
	struct sockaddr_in client_addr; // Estructura de la direccion del cliente en formato socket
	int size; // Tamaño de la estructura sockaddr_in
	int check; // Variable que se usara en las opciones del descriptor de socket
	int process; // Proceso
	int control; // Variable para la comprobacion de errores cuando manejemos procesos 
	int time; // Variable para guardar la hora
	char buffer[50]; // Buffer para almacenar
	
	
	
	char rec_buf[1024];
	int bytes_read;
	int resp; // Variable donde guardamos la respuesta
	char string[50]; // Variable donde guardaremos la hora
	int h;
	
	printf ("Veces= %d\n",veces);
	
	// Creamos el socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	  if(socket_fd == -1){ // Comprobamos que se ha abierto bien el socket. Si no, nos da error.
		  printf("ERROR en la creacion de socket()");
		  exit(1);
	  }
	  
	/* Consecucion de las opciones del servidor:
	 * 	socket_fd: Descriptor de socket
	 * 	SOL_SOCKET: Nivel donde residen las opciones. Para manipular opciones de sockets, ponemos SOL_SOCKET
	 * 	SO_REUSEADDR: Permite reutilizar la direccion IP de la maquina.
	 * 	flag: Variable donde guardamos el puntero al buffer.
	 * 	sizeof(int): Tamaño de la variable flag. Va actualizandose.
	 *
	 */
	
    	check = setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&flag,sizeof(int));
	
	// Comprobacion de errores al poner las opciones
	  if (check == -1) { 
		  printf("ERROR en setsockopt()"); 
		  exit(1);
	  }
	  
	// Rellenamos las estructuras con los parametros de la teoria
	  serv_addr.sin_family = AF_INET; 
	  serv_addr.sin_port = htons(port);
	  serv_addr.sin_addr.s_addr = INADDR_ANY; // Cualquier direccion
	  

	  
	  check = bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)); // Nos asociamos al socket en cuestion
	    if (check == -1) { // Comprobamos que hacemos bien el bind() y sino, error
		    printf("ERROR: Ha ocurrido un error al hacer bind()\n");
		    exit(1);
	    }
	    // Una vez hecho bind(), hemos de encargarnos de que el socket escuche un numero maximo de conexiones pendientes. Seran 5
	 check = listen(socket_fd, 5);
	    if (check == -1) { 
		    printf("ERROR: Ha ocurrido un error al hacer listen()\n");
		    exit(1);
	    }
	    
	  // En este punto, el servidor ya esta a la escucha
	  
	  printf("El servidor a la escucha por el puerto %i\n", port);
	  
	  // Bucle infinito, se queda escuchando todo el tiempo
	  while(1) { 
		  size = sizeof(struct sockaddr_in); // Tamaño de la estructura sockaddr_in. Nos servira posteriormente
		  
		  // Con connected, asociamos el socket a una direccion IP y un puerto gracias a la funcion accept()
		  // Aceptamos la peticion, y si no hay ninguna, se bloquea esperando. Nos devuelve un descriptor
		  connected = accept(socket_fd, (struct sockaddr *) &client_addr, &size); 
		  // Imprimimos por pantalla esa conexion. Usamos la funcion inet_toa para traducir la IP de una estructura sock_addr a una IP de numeros separados por puntos.
		  printf("Se ha realizado la conexion con la direccion %s y el puerto %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		  // Creamos un proceso
		  process = fork(); 
		  
		  if (process == 0) { // Si el process en ese momento es 0 significa que es un hijo (No lo necesito y lo cierro) y pongo control = 0.
			  close(socket_fd); 
			  control=0;
			  
			  while (control >= 0) { //Si es el padre, pedimos la hora. Hacemos procesos para poder atender hasta el maximo de 5 peticiones.
				  time = hora(); // Pedimos la hora y la guardamos en la variable time
				  hora_Texto(buffer,time); // Lo ponemos en formato texto (string)
				  
				  // Si estamos en modo depuracion
				    if (debug_mode == 1) {
					    printf("La hora actual es = %s		[%X]\n", buffer, time); 
				    } 
				    time = htonl(time); //Lo pasamos a formato de red
				    // Enviamos datagrama 
				    control = send(connected, &time, sizeof(time), 0);
				    // Comprobamos errores al enviar. Si hay error, cerramos el socket
				      if (control < 0) {
					      printf("Error al enviar\n");
					      close(connected);
				      }
					  
					  
					  /*Modificacion que recibe la hora que le manda TCP_client
					  bytes_read = recv(connected, &rec_buf, 1024, 0); //recibo 
					  //printf("bytes_read vale%d\n",bytes_read);
					  if(bytes_read > 0){
					  // Lo copiamos en la respuesta
					  memcpy(&resp, &rec_buf, 4);
					  // Lo convertimos a string para que lo podamos leer
            			hora_Texto(string, ntohl(resp)); 
					  // Si estamos en modo depuracion
				      // Imprimimos la cadena
					 
            		    printf(" NUEVO es: %s\n",string);
					
			             }
					  */
					 
				      else sleep(1); // Esperamos un segundo para enviar otra vez
			  }
			  // Cerramos la conexion que establecimos con accept
			  close(connected);
			  exit(0); // Salimos del if
		  }
		  
		  // Cerramos la conexion porque a partir de aqui hay un proceso padre que no necesita la conexion abierta.
		  close(connected); 
	  }
	  // Cerramos el descriptor del socket.
	  close(socket_fd);	
		
	}
	
	void interrupcion(int sig){
	  close(connected); //Cerramos conexion
	  close(socket_fd); //Cerramos el socket
	  printf("\nSIGINT capturado....CERRAMOS\n");
	  (void)signal(SIGINT,SIG_DFL); //Tratamos la señal
	  exit(0);
 }

void hora_Texto(char* string, int hAct) {//convierte hAct en un string legible
	int err;
	time_t tiempo;//Creamos una variable de tipo temporal
	struct tm *tmp;
	tiempo = hAct-2208988800U; //Segundos transcurridos desde 1900 hasta 1970
	//tiempo = tiempo + 3600; // horario de invierno
	tmp=gmtime(&tiempo);//La función gmtime convierte el tiempo en formato condensado apuntado por timeval en el tiempo en formato separado, expresado como Tiempo Universal Coordenada (UTC).
	err=strftime(string,50,"%a  %b %d  %H:%M:%S %Z %Y",tmp);
/*La función strftime coloca caracteres en el array apuntado por cad controlado por la cadena apuntada por formato. La cadena formato consiste de cero o más especificadores de conversión y caracteres multibyte ordinarios. Todos los caracteres ordinarios (incluyendo el carácter nulo terminal) son copiados sin cambiar en el array. Si se copian entre objetos superpuestos, el comportamiento no está definido. No más de maxtam caracteres son colocados en el array. Cada especificador de conversión es reemplazado por los caracteres apropiados descritos en la siguiente lista. Los caracteres apropiados son determinados por la categoría LC_TIME  de la localidad actual y por los valores contenidos en la estructura apuntado por tiempoPtr.*/
}
int hora(){//Nos va a devolver el tiempo del sistema desde 1900
	time_t tiempo;// variabe time_t para capturar lo que devuelve la funcion time
	struct tm *temp; //estructura tm, que es lo que nos devuelve la funcion localtime
	tiempo = time(NULL);//nos devuelve el tiempo en formato condensado
	temp = localtime(&tiempo);//cambia el tiempo de formato condensado a formato separado
	if (temp == NULL) {//Si local time falla doy un error
		printf("Error al conseguir hora....");
		exit(1);//Catapum!
	}
	return tiempo+2208988800U+3600; //Sumando 2208988800U lo convierte a tiempo NTP (network time protocol) 
}
	
	
int main(int argc, char *argv[]) {
	// Poder pulsar CTRL+C para poder salir
	(void) signal(SIGINT, interrupcion); 
	// No matar al hijo al hacer send en el socket cerrado
	signal(SIGPIPE,SIG_IGN);
	//atdate [-h serverhost] [-p port] [-m cu | ct | s ] [-d]
	
	//Comprobamos los parametros de entrada
	int i;
	for (i=1; i<(argc); i++) {
		if (strcmp("-h", argv[i]) == 0) {
			host = argv[++i]; // El host se almacena directamente.
		}
		
		if (strcmp("-n", argv[i]) == 0) {
			veces = atoi(argv[++i]); // El host se almacena directamente.
	
		}

		
		
		if (strcmp("-p", argv[i]) == 0) {
			port = atoi(argv[++i]); // Lo almacenamos como entero.
		}
		if (strcmp("-m", argv[i]) == 0){ // Indicamos el modo del programa.
			i++;
			if (strcmp("cu", argv[i]) == 0){
				option = 1;  // Modo consulta UDP
				printf("Consulta UDP\n");
			}
			if (strcmp("ct", argv[i]) == 0){
				option = 2; // Modo consulta TCP
				printf("Consulta TCP\n");
			}
			if (strcmp("s",argv[i]) == 0){
				option = 3 ; // Modo servidor
				printf("Modo servidor\n");
			}
		}
		if (strcmp("-d",argv[i]) == 0){ // Habilitamos modo depurador
			debug_mode = 1; 
			printf("Modo depurador activado\n");
		}
	}
	// Comprobacion de errores (No hay host o elegimos una opcion que no esta en la lista)
	if ((host == NULL) && (option!= 3)){
		printf("Error\n");
		exit(1);
	}
	
	// El caso 1 sera para consultas UDP, el caso 2 para consultas TCP y el caso 3 sera para activar el modo servidor
	
	switch (option){
	case 1:
		udp_client(host, port);
		break;
	case 2:
		tcp_client(host, port);
		break;
	case 3: 
	default:
		if(debug_mode == 1){
			printf("Servidor a la espera\n");
		}
		if(port == 37){
                	port = 6482; // Numero de puerto que escucha nuestro servidor (6000 + numero de alumno)
                }
		server(port);
	}
	exit(1);	
}	
	
