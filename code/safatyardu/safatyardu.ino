#include <SoftwareSerial.h>

// RX e TX DO ESP8266
SoftwareSerial ESP(2, 3);

// SENSOR DE MOVIMENTACAO E DEBUG
#define PINMOV 7
#define DEBUG false

void setup(){

    // OPEN SERIAL COMMUNICATIONS AND WAIT FOR PORT TO OPEN ESP8266:
    Serial.begin(9600);

    // CONECNTANDO AO ESP
    ESP.begin(9600);
	
	//DETERMINANDO ENTRADA DO PINO DO SENSOR DE MOVIMENTO
    pinMode(PINMOV, INPUT);

    // INICIALIZAR WIFI COM ESP
    initwifi();
	
}

// FUNCTION LOOP
void loop(){
	
	//VERIFICANDO DETECÇÃO DE MOVIMENTO
    boolean GETMOV = digitalRead(PINMOV);

    delay(1000);

    if (GETMOV){

        Serial.println("## DETECTION ##");
		
		// REQUEST DE MOVIMENTO
		requestmov();
		
    }

}

// REALIZANDO A CONEXAO COM O WIFI
void initwifi(){
    
    // RST
    sendwifi("AT+RST\r\n", 2000, DEBUG); 
    
    // CONECTA A REDE WIRELESS
    sendwifi("AT+CWJAP=\"SSID\",\"SENHA\"\r\n", 2000, DEBUG);
    
    delay(3000);

    // MODO WIFI NÚMERO 1( ESTAÇÃO-CLIENTE )
    sendwifi("AT+CWMODE=1\r\n", 1000, DEBUG);

    // MOSTRA O ENDERECO IP
    sendwifi("AT+CIFSR\r\n", 1000, DEBUG);

    // CONFIGURA PARA MULTIPLAS CONEXOES ( MAX=4 )
    sendwifi("AT+CIPMUX=1\r\n", 1000, DEBUG);

}

// FUNÇAO PARA ENVIAR DADOS PARA O ESP8266.
String sendwifi(String command, long timeout, boolean debug){

    String response = "";

    // ESCREVENDO NO MODULO ESP8266
    ESP.println(command); 

    // CAPTURA MILISEGUNDOS
    long times = millis();
    
    while ((times + timeout) > millis()){
        while (ESP.available()){

            // O ESP TEM DADOS, ASSIM MOSTRA A SUA SAÍDA PARA A JANELA SERIAL
            char c = ESP.read(); 
            response += c;

        }
    }

    // ESCREVENDO DEBUG NO TERMINAL
    if (debug){
        Serial.println(response);
    }

    return response;

}

// REALIZANDO REQUISICAO DA DETECCAO DE MOVIMENTO
void requestmov(){

    Serial.println("# INIT REQUEST");

    // ASSEGURA QUE A CONEXÃO 0 ESTÁ LIVRE PRA SER USADA
    sendwifi("AT+CIPCLOSE=0\r\n", 2000, DEBUG);

    sendwifi("AT+CIPSTART=0,\"TCP\",\"safetyflask.herokuapp.com\",80\r\n", 1000, DEBUG);
    
    delay(1000);

    String request  = "";
    String body     = "arduino=DFEFEGNE24324";
    String command  = "";

    request += "POST /logs HTTP/1.1\r\n"; 
    request += "Host: safetyflask.herokuapp.com\r\n";
    request += "Authorization: Basic YWRtaW46c2VjcmV0\r\n";
    request += "Content-Type: application/x-www-form-urlencoded\r\n";    
    request += "Content-Length: "+body.length()+"\r\n";
    request += body+"\r\n";

    command = "AT+CIPSEND=0,";
    command += request.length();
    command += "\r\n";

    sendwifi(command, 1000, DEBUG);

    sendwifi(request, 1000, DEBUG);

    delay(5000);

    sendwifi("AT+CIPCLOSE=0\r\n", 1000, DEBUG);

    Serial.println("# END REQUEST");

}
