#include <SoftwareSerial.h>

// RX e TX DO ESP8266
SoftwareSerial ESP(2, 3);

// SENSOR DE MOVIMENTACAO E DEBUG
#define PINMOV 7

// FUNCTION SETUP DO ARDUINO
void setup()
{

    // OPEN SERIAL COMMUNICATIONS AND WAIT FOR PORT TO OPEN ESP8266:
    Serial.begin(9600);

    // CONECNTANDO AO ESP
    ESP.begin(9600);

    //DETERMINANDO ENTRADA DO PINO DO SENSOR DE MOVIMENTO
    pinMode(PINMOV, INPUT);

    // INICIALIZAR WIFI COM ESP
    initwifi();

}

// FUNCTION LOOP DO ARDUINO
void loop()
{

    //VERIFICANDO DETECÃ‡ÃƒO DE MOVIMENTO
    boolean GETMOV = digitalRead(PINMOV);

    delay(1000);

    if (GETMOV)
    {

        // REQUEST DE MOVIMENTO
        requestmov();

    }
    
}

// REALIZANDO A CONEXAO COM O WIFI
void initwifi()
{

    // AT - TEST ACTION
    sendwifi("AT\r\n", 2000);

    // AT+RST - REST MODULE
    sendwifi("AT+RST\r\n", 2000);

    // AT+CWJAP= - CONECTAR A REDE WIRELESS
    sendwifi("AT+CWJAP=\"SSID\",\"PASSWORD\"\r\n", 2000);

    delay(4000);

    // AT+CWMODE=1 - DEFININDO MODO WIFI COMO ESTACAO CLIENTE (1)
    sendwifi("AT+CWMODE=1\r\n", 2000);

    // AT+CIFSR - MOSTRA O ENDERECO IP COMO UM CLIENTE
    sendwifi("AT+CIFSR\r\n", 2000);

    // AT+CIPMUX - CONFIGURA PARA MULTIPLAS CONEXOES ( MAX=4 )
    sendwifi("AT+CIPMUX=0\r\n", 2000);

}

// REALIZANDO REQUISICAO DA DETECCAO DE MOVIMENTO
void requestmov()
{

    // AT+CIPCLOSE - ASSEGURA QUE A CONEXAO 0 ESTAO LIVRE ASSIM PODENDO SER USADA
    sendwifi("AT+CIPCLOSE\r\n", 2000);

    // AT+CIPSTART - INICIA UMA CONEXAO COMO CLIENTE
    sendwifi("AT+CIPSTART=\"TCP\",\"safetyflask.herokuapp.com\",80\r\n", 2000);

    delay(1000);

    String request  = "";
    String body     = "arduino=DFEFEGNE24324";
    String command  = "";

    request += "POST /logs HTTP/1.1\r\n";
    request += "Host: safetyflask.herokuapp.com\r\n";
    request += "Authorization: Basic YWRtaW46c2VjcmV0\r\n";
    request += "Content-Type: application/x-www-form-urlencoded\r\n";
    request += "cache-control: no-cache\r\n";
    request += "Content-Length: " + String(body.length()) + "\r\n\r\n";
    request += body + "\r\n";

    // AT+CIPSEND - DEFINIR O COMPRIMENTO DOS DADOS QUE SERAO ENVIADOS
    command = "AT+CIPSEND=" + String(request.length()) + "\r\n";

    sendwifi(command, 2000);

    sendwifi(request, 2000);

    delay(4000);

    sendwifi("AT+CIPCLOSE\r\n", 2000);

}

// FUNCAO PARA ENVIAR DADOS PARA O ESP8266.
void sendwifi(String command, long timeout)
{

    // ESCREVENDO COMANDO NO MONITOR SERIAL
    Serial.println(command);

    // ESCREVENDO NO MODULO ESP8266
    ESP.print(command);

    // CAPTURA MILISEGUNDOS
    long int times = millis();

    // STRING DE RETORNO
    String response = "";

    while ((times + timeout) > millis())
    {
        while (ESP.available())
        {

            // O ESP TEM DADOS, ASSIM MOSTRA A SUA SAÃ�DA PARA A JANELA SERIAL
            char c = (char) ESP.read();
            response += c;

        }
    }

    // ESCREVENDO DEBUG NO TERMINAL
    Serial.println(response);

}
