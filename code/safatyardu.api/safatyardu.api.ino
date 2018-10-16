#include <SoftwareSerial.h>

// RX e TX DO ESP8266
SoftwareSerial ESP(2, 3);

// MOSTRAR UM LOG DETALHADO
bool DEBUG = true;

// TEMPO DE TIMEOUT
int RESPONSETIME = 10;

void setup(){

    // MARCANNDO PINO 13 PARA SAIDA
    pinMode(13, OUTPUT);

    // OPEN SERIAL COMMUNICATIONS AND WAIT FOR PORT TO OPEN ESP8266:
    Serial.begin(115200);

    while (!Serial){
        ; // ESPERANDO PORTA SERIAL SE CONECTAR
    }

    ESP.begin(115200);

    while (!ESP){
        ; // ESPERANDO PORTA SERIAL SE CONECTAR
    }

    sendwifi("AT+CWMODE=2", RESPONSETIME, DEBUG);               // CONFIGURE COMO PONTO DE ACESSO
    sendwifi("AT+CIFSR", RESPONSETIME, DEBUG);                  // OBTER ENDEREÇO DE IP
    sendwifi("AT+CIPMUX=1", RESPONSETIME, DEBUG);               // CONFIGURAR PARA MÚLTIPLAS CONEXÕES
    sendwifi("AT+CIPSERVER=1,80", RESPONSETIME, DEBUG);         // LIGUE O SERVIDOR NA PORTA 80

    sendserial("WIFI Conectando e executando !", RESPONSETIME, DEBUG);

}

void loop(){

    if (Serial.available() > 0){

        String message = readserial();

        if (find(message, "debugEsp8266:")){

            String result = sendwifi(message.substring(13, message.length()), RESPONSETIME, DEBUG);

            if (find(result, "OK")){
                senddata("\nOK");
            } else {
                senddata("\nEr");
            }

        }

    }

    if (ESP.available() > 0){

        String message = readwifi();

        if (find(message, "esp8266:")){

            String result = sendwifi(message.substring(8, message.length()), RESPONSETIME, DEBUG);

            if (find(result, "OK")){

                senddata("\n" + result);
                
            } else {
                // AT COMMAND ERROR CODE FOR FAILED EXECUTING STATEMENT
                senddata("\nErrRead");
            }

        } else if (find(message, "HELLO")){

            // RECEIVES HELLO FROM WIFI AND ARDUINO SAYS HI
            senddata("\\nHI!");

        } else if (find(message, "LEDON")){

            // LIGAR LED NO PINO 13
            digitalWrite(13, HIGH);

        } else if (find(message, "LEDOFF")){

            // DESLIGAR LED NO PINO 13
            digitalWrite(13, LOW);

        } else {

            // CÓDIGO DE ERRO DE COMANDO PARA NÃO SER LIDO
            senddata("\nErrRead");

        }

    }

    delay(RESPONSETIME);

}

// VERIFICAR SE EXISTE A SENTENÇA DENTRO DA DESCRPTION
boolean find(String description, String sentence){

    if (description.indexOf(sentence) >= 0){
        return true;
    } else {
        return false;
    }

}

// FUNÇÃO USADA PARA ENVIAR STRING PARA O CLIENTE TCP USANDO O CIPSEND
void senddata(String data){

    String len = "";
    len += data.length();

    sendwifi("AT+CIPSEND=0," + len, RESPONSETIME, DEBUG);
    delay(100);

    sendwifi(data, RESPONSETIME, DEBUG);
    delay(100);

    sendwifi("AT+CIPCLOSE=5", RESPONSETIME, DEBUG);

}

// ENVIAR MENSSAGENS PARA O TERMINAL
String sendserial(String command, long timeout, boolean debug){
    
    String response = "";
    long times      = millis();

    // ESCREVER NA PORTA SERIAL DO TERMINAL
    Serial.println(command);

    while ((times + timeout) > millis()){
        while (Serial.available()){

            // O SERIAL TERMINAL TEM CARACTER, EXIBE SEU RESULTADO PARA A JANELA SERIAL
            char c      = Serial.read(); 
            response    += c;

        }
    }

    // ESCREVENDO DEBUG NO TERMINAL
    if (debug){
        Serial.println(response);
    }

    return response;

}

// FUNÇAO PARA ENVIAR DADOS PARA O ESP8266.
String sendwifi(String command, long timeout, boolean debug){

    String response = "";
    long times      = millis();

    // ESCREVENDO NO MODULO ESP8266
    ESP.println(command); 
    
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

// LER DADOS DO ARDUINO SERIAL
String readserial(){

    char data[100];
    int inxcount = 0;

    while (Serial.available() > 0){

        data[inxcount] = Serial.read();
        inxcount++;
        // NULL TERMINATE THE STRING
        data[inxcount] = '\0';

    }

    String strings(data);
    strings.trim();

    return strings;

}

// LER DADOS DO ESP8266 SERIAL
String readwifi(){

    char data[100];
    int inxcount = 0;

    while (ESP.available() > 0){

        data[inxcount] = ESP.read();
        inxcount++;
        // NULL TERMINATE THE STRING
        data[inxcount] = '\0';

    }

    String strings(data);
    strings.trim();

    return strings;

}