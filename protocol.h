#ifndef protocolostpte_practicas_headerfile
#define protocolostpte_practicas_headerfile
#endif

// COMANDOS DE APLICACION
#define HELO "HELO"  // 
#define MF "MAIL FROM:"//Remitente
#define RC "RCPT TO:"//Destinatario
#define US "USER"//Usuario
#define PW "PASSWORD" // Contraseña.
#define AS "SUBJECT:"
#define RE "FROM:"
#define DE "TO:"
#define HO "DATE:"
#define MS "MENSAJE"
#define SD  "QUIT"  // Finalizacion de la conexion de aplicacion
#define EX "EXIT"  // Finalizacion de la conexion de aplicacion 
#define EC "ECHO" // Definicion del comando "ECHO" para el servicio de eco
#define DA "DATA" // Definición de DATA.
#define RS "RSET" // Definición de RSET.





// RESPUESTAS A COMANDOS DE APLICACION
#define OK  "2" // Ok.
#define OKDATA "3"
#define ER  "5"

//FIN DE RESPUESTA
#define CRLF "\r\n"

//ESTADOS
#define S_WELC 0
#define S_HELO 1
#define S_MF 2
#define S_RCPT 3
#define S_DATA 4
#define S_MENSAJE 5
#define S_QUIT 7
#define S_EXIT 8
#define S_RSET 9


//PUERTO DEL SERVICIO
#define        SMTP_SERVICE_PORT    25 // Cambiamos el puerto de servicio al de SMTP.

// NOMBRE Y PASSWORD AUTORIZADOS
#define USER        "alumno"
#define PASSWORD    "123456"

#define mod "mod"
