/*******************************************************
Protocolos de Transporte
Grado en Ingenier�a Telem�tica
Dpto. Ingen�er�a de Telecomunicaci�n
Univerisdad de Ja�n

Fichero: cliente.c
Versi�n: 2.0
Fecha: 09/2017
Descripci�n:
	Cliente sencillo TCP.

Autores: Antonio Jim�nez S�nchez y Pablo Mart�nez Bruque.

*******************************************************/
#include <stdio.h>
#include <ws2tcpip.h>//Necesaria para las funciones IPv6
#include <conio.h>
#include <time.h>
#include "protocol.h"


#pragma comment(lib, "Ws2_32.lib")

int main(int *argc, char *argv[])
{
	SOCKET sockfd;
	struct sockaddr *server_in;
	struct sockaddr_in server_in4;
	struct sockaddr_in6 server_in6;
	int address_size = sizeof(server_in4);
	char buffer_in[1024], buffer_out[1024], input[1024], inputsn[1024], inputDat[1024];
	int recibidos=0,enviados=0;
	int estado=S_HELO;
	int tiempo;
	char option;
	char caracter;
	int ipversion=AF_INET;//IPv4 por defecto
	char ipdest[256];
	char ipdestdominio = ""; //A�adimos esta direcci�n para la parte 5 de los dominios.
	char default_ip4[16]="127.0.0.1";
	char default_ip6[64]="::1";
	time_t t;
	struct tm *tm;
	char fechayhora[100];
	int flag;


	//A�adidos.
	struct in_addr address; //Para parte 5 dominios.
	struct hostent *host; //Para parte 5 dominios.

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
   
	//Inicializaci�n Windows sockets - SOLO WINDOWS
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
		return(0);

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1){
		WSACleanup();
		return(0);
	}
	//Fin: Inicializaci�n Windows sockets
	
	printf("**************\r\nCLIENTE TCP SENCILLO SOBRE IPv4 o IPv6\r\n*************\r\n");
	

	do{

		printf("CLIENTE> �Qu� versi�n de IP desea usar? 6 para IPv6, 4 para IPv4 [por defecto] ");
		gets_s(ipdest, sizeof(ipdest));

		if (strcmp(ipdest, "6") == 0) {
			ipversion = AF_INET6;

		}
		else { //Distinto de 6 se elige la versi�n 4
			ipversion = AF_INET;
		}
		//Crea el descriptor socket.
		sockfd=socket(ipversion,SOCK_STREAM,0);
		if(sockfd==INVALID_SOCKET){
			printf("CLIENTE> ERROR\r\n");
			exit(-1);
		}
		else{
			//ESTO TIENE QUE ESTAR EN  UN BUCLE
			do {

				printf("CLIENTE> Introduzca la IP destino (o dominio, parte 5) (pulsar enter para IP por defecto): ");
				gets_s(ipdest, sizeof(ipdest));
				//SESION 5
				//Codigo profesor diapositivas.

				ipdestdominio = inet_addr(ipdest); // Convierte a direcci�n IP o a cadena 
				if (ipdestdominio == INADDR_NONE) {
					//La direcci�n introducida por teclado no es correcta o
					//corresponde con un dominio.
					struct hostent *host;
					host = gethostbyname(ipdest); //Pruebo si es dominio
					if (host != NULL) { //Si es distinto de null, es dominio
						
						//**El control de errores de dominio me lo explic� Juan N��ez Lerma, por lo que podr�an parecerse.
						//La bandera controla el comportamiento de la funci�n.
						//Una bandera con valor 0 indicar� un funcionamiento normal del protocolo.
						//Por esta raz�n, colocaremos en el else un valor diferente a 0 en caso de que falle, y vuelva a repetir la operaci�n.


						flag = 0;
						memcpy(&address, host->h_addr_list[0], 4);
						printf("Direccion %s\n", inet_ntoa(address));
					}
					else {
						//MENSAJE DE ERROR
						printf("Dominio no v�lido\n");
						flag = 1;

					}
					
				}
			} while (flag == 1);

			//Direcci�n por defecto seg�n la familia
			if(strcmp(ipdest,"")==0 && ipversion==AF_INET)
				strcpy_s(ipdest,sizeof(ipdest),default_ip4);

			if(strcmp(ipdest,"")==0 && ipversion==AF_INET6)
				strcpy_s(ipdest, sizeof(ipdest),default_ip6);

			if(ipversion==AF_INET){
				server_in4.sin_family=AF_INET;
				server_in4.sin_port=htons(SMTP_SERVICE_PORT);
				//server_in4.sin_addr.s_addr=inet_addr(ipdest);
				inet_pton(ipversion,ipdest,&server_in4.sin_addr.s_addr);
				server_in=(struct sockaddr*)&server_in4;
				address_size = sizeof(server_in4);
			}

			if(ipversion==AF_INET6){
				memset(&server_in6, 0, sizeof(server_in6));
				server_in6.sin6_family=AF_INET6;
				server_in6.sin6_port=htons(SMTP_SERVICE_PORT);
				inet_pton(ipversion,ipdest,&server_in6.sin6_addr);
				server_in=(struct sockaddr*)&server_in6;
				address_size = sizeof(server_in6);
			}

			estado=S_WELC;

			if(connect(sockfd, server_in, address_size)==0){
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n",ipdest,SMTP_SERVICE_PORT);
				//recibidos = recv(sockfd, buffer_in, 512, 0);
			
				//Inicio de la m�quina de estados
				do {
					switch (estado) {
					case S_WELC:
						
				
						break;
					case S_HELO:
						// Se recibe el mensaje de bienvenida                        
						printf("CLIENTE,HELO> Bienvenido : \n");
						sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", HELO, CRLF);
					
						break;
						

					case S_MF:
						// 
						printf("CLIENTE> Introduzca el remitente (enter para salir y un 3 para reset): ");
						gets_s(input, sizeof(input));

						if (strlen(input) == 0) {
							//Si est� vac�o, se sale.
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", SD, CRLF);
							estado = S_QUIT;
						}
						//Escribe RSET y pasa a ese estado.
						else if ((strcmp(input, "3") == 0)) {
							sprintf_s(buffer_out, sizeof(buffer_out), "RSET%s", CRLF);
							estado = S_RSET;
						}
						else {
							//Escribe en el servidor.
							sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", MF, input, CRLF);
						
							//
						}
						
						break;
						
					case S_RCPT:
						printf("CLIENTE> Introduzca el destinatario (enter para salir y un 3 para reset): \n");
						gets_s(input, sizeof(input)); //Recoge lo que le introducimos
						if (strlen(input) == 0) { //Si no hemos introducido nada, va al estado QUIT y sale.
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", SD, CRLF);
							estado = S_QUIT;
						}
						//Escribe RSET y pasa a ese estado.
						else if ((strcmp(input, "3") == 0)) {
							sprintf_s(buffer_out, sizeof(buffer_out), "RSET%s", CRLF);
							estado = S_RSET;
						}
						else {
							//Escribe en el servidor.

							sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", RC, input, CRLF);
							
						}
						//NO SE CAMBIA DE ESTADO
						//SE CAMBIA CUANDO SE RECIBE LA RESPUESTA
						break;
				

					case S_DATA:
						
						//En data, solo escribimos el caso DATA y pasamos al siguiente estado.
						sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", DA, CRLF);
					
						
						
						break;
						

					case S_MENSAJE: 
						//Vamos a seguir el siguiente proceso, aprovechando el c�digo anterior :
						//1- Introducir lo pedido.
						//2- Obtener lo anteriormente introducido.
						//3- Dar formato y almacenarlo en un buffer con sprintf_s.
						//**Para futuros usos : La funci�n sprintf_s da formato y almacena una serie de caracteres y valores en buffer.
						//Cada argument (si existe) se convierte y sale seg�n la especificaci�n de formato correspondiente de format.
							//El formato consta de caracteres ordinarios y tiene el mismo formato y funci�n que el argumento format para printf. 
							//Un car�cter null se anexa despu�s del �ltimo car�cter escrito. 
							//Si la copia tiene lugar entre cadenas que se superponen, el comportamiento es indefinido.
						//4- Enviar.

						//Cabecera HORA:
						t = time(NULL);
						tm = localtime(&t);
						strftime(fechayhora, 100, "%d/%m/%Y a las %H:%M", tm);
						printf("Fecha env�o: %s\n", fechayhora);
						sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", HO, input, CRLF);
						enviados = send(sockfd, buffer_out, (int)strlen(buffer_out), 0); //Envio

						//Cabecera asunto.
						printf("Introduzca el ASUNTO : "); //Obtengo
						gets_s(input, sizeof(input)); //Guardo
						sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", AS, input, CRLF); //Escribo
						enviados = send(sockfd, buffer_out, (int)strlen(buffer_out), 0); //Envio 
					
						//Cabecera remitente.
						printf("Introduzca REMITENTE : "); 
						gets_s(input, sizeof(input)); 
						sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", RE, input, CRLF); 
						enviados = send(sockfd, buffer_out, (int)strlen(buffer_out), 0); //Envio

						//Cabecer destinatario
						printf("Introduzca  DESTINATARIO: ");
						gets_s(input, sizeof(input)); 
						sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", DE, input, CRLF, CRLF); 
						enviados = send(sockfd, buffer_out, (int)strlen(buffer_out), 0); //Envio

						printf("Introduzca el CUERPO del mensaje (. y enter para acabar la redaccion):\r\n");

						//Bucle para continuar escribiendo hasta escribir un punto.
						do {
							printf(">");
							gets_s(input, sizeof(input)); 
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", input, CRLF); 
							enviados = send(sockfd, buffer_out, (int)strlen(buffer_out), 0); //Contin�a enviando.
							
						} while (strcmp(input, ".") != 0);
						break;



					}
			


						
				
				

					if(estado!=S_WELC && estado!=S_MENSAJE){
						enviados=send(sockfd,buffer_out,(int)strlen(buffer_out),0);
						if(enviados==SOCKET_ERROR){
							 estado=S_QUIT;
							 continue;
						}
					}
						
					recibidos=recv(sockfd,buffer_in,512,0);
					if(recibidos<=0){
						DWORD error=GetLastError();
						if(recibidos<0){
							printf("CLIENTE> Error %d en la recepci�n de datos\r\n",error);
							estado=S_QUIT;
						}
						else{
							printf("CLIENTE> Conexi�n con el servidor cerrada\r\n");
							estado=S_QUIT;
						}
					}else{
						buffer_in[recibidos]=0x00;
						printf(buffer_in);
						//switch para realizar determinadas acciones seg�n el estado en el que estemos.
						switch (estado) {

							case S_WELC:
							case S_HELO:
							case S_MF:
								//Si la respuesta empieza por 2, aumenta el estado en 1.
								if (buffer_in[0] == '2') 
									estado++;
								break;
							case S_RCPT:
								//Si la respuesta empieza por 5, indicamos que es un error.
								if (buffer_in[0] == '5') {
									printf("Usuario no permitido : Volviendo a pedir usuarios.\n");
									sprintf_s(buffer_out, sizeof(buffer_out), "RSET%s", CRLF);
									estado = S_RSET;
									break;
								}



								//En caso de que no haya error:

								printf("�Qui�res a�adir otro destinatario? S S� cualquier otro para no: ");
								//Si introducimos s�, vuelve al estado RCPT.
								gets_s(input, sizeof(input)); //Nombre del destinatario.
								if (strcmp(input, "S") == 0) {
									estado = S_RCPT;
									break;
								}

								else {
									if (buffer_in[0] == '2') {
										estado++;
									}

								}
								break;
							case S_DATA:
								//Si la respuesta empieza por 3, aumenta el estado en 1.
								if (buffer_in[0] == '3')
									estado++;
								break;
							case S_MENSAJE:
								if (buffer_in[0] == '2') {

									//Preguntar si quiere enviar m�s correos, y si quiere ir al estado mail from y si no quiere enviar un QUIT


									printf("CLIENTE, CASO MENSAJE> Has enviado tu correo, �quieres enviar otro? S para s�, CUALQUIER OTRO para no: ");
									gets_s(inputsn, sizeof(inputsn)); //Recoge lo que le introducimos
									if ((strcmp(inputsn, "S") == 0)) {
										printf("CLIENTE,NUEVA PETICI�N DE MENSAJE : Volviendo\n");
										sprintf_s(buffer_out, sizeof(buffer_out), "RSET%s", CRLF);
										estado = S_RSET;

									}
									else {
										printf("CLIENTE,NUEVA PETICI�N DE MENSAJE : Has pulsado no, saliendo.");

										sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", SD, CRLF);
										estado = S_QUIT;


									}
								}
								break;
							case S_RSET:
								//El caso S_RSET vuelve al estado MF tras escribir RSET en el servidor.
								estado = S_MF;
								break;
								break;
							}
						}
						
					

				}while(estado!=S_QUIT);		
			}
			else{
				int error_code=GetLastError();
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n",ipdest,SMTP_SERVICE_PORT);
			}		
			// fin de la conexion de transporte
			closesocket(sockfd);
			
		}	
		printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n");
		option=_getche();

	}while(option!='n' && option!='N');

	WSACleanup();
	return(0);
}
