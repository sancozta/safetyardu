#include <SoftwareSerial.h>
#include <avr/wdt.h>

// DEBUG DO ESP8266
#define DEBUG false

// ESP8266 CONECTADO NOS PINOS 2 E 3 (TX E RX)
SoftwareSerial esp8266(2, 3);

// PINOS USADOS PELO SENSOR UV ML8511

int PIN_UV = A0;  // PINO DA LEITURA DO SENSOR
int PIN_3V3 = A1; // LEITURA DE REFERÊNCIA DA ALIMENTAÇÃO 3V3

// CONTA O NÚMERO DE CONSULTAS
int count = 0;

void setup(){
	
    Serial.begin(9600);

    pinMode(PIN_UV, INPUT);
    pinMode(PIN_3V3, INPUT);

    configWifi();

    // LIGA O SERVIDOR ESCUTANDO A PORTA 80
    sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG);
	
}

void configWifi(){
	
    esp8266.begin(115200);

	// RST
    sendData("AT+RST\r\n", 2000, DEBUG); 
    
	// CONECTA A REDE WIRELESS
    sendData("AT+CWJAP=\"SSID\",\"SENHA\"\r\n", 2000, DEBUG);
    
	delay(3000);
    
	// MODO WIFI NÚMERO 1 (ESTAÇÃO CLIENTE)
    sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
    
	// MOSTRA O ENDERECO IP
    sendData("AT+CIFSR\r\n", 1000, DEBUG);
    
	// CONFIGURA PARA MULTIPLAS CONEXOES (MAX=4)
    sendData("AT+CIPMUX=1\r\n", 1000, DEBUG);
	
}

// ENVIA UMA RESPOSTA DE ERRO
void errorResponse(int connId){
    
	// MONTA A PAGINA
    String page = "Not found";

    // MONTA A RESPOSTA
    String resp = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: ";
    resp += page.length();
    resp += "\r\n\r\n";
    resp += page;

    // COMANDO DO ENVIO DE DADOS
    String cmdSend = "AT+CIPSEND=";
    cmdSend += connId;
    cmdSend += ",";
    cmdSend += resp.length();
    cmdSend += "\r\n";

    sendData(cmdSend, 1000, DEBUG);
    sendData(resp, 1000, DEBUG);
	
}

void loop(){
	
    if (esp8266.available()){
		
        if (esp8266.find("+IPD,")){
			
			// ESPERA UMA REQUISIÇÃO CHEGAR
            delay(1000);
            
			// ID DA CONEXÃO
            int connId = esp8266.read() - 48;

            if (esp8266.find("cmd")){
				
                // COMANDO DE 5 CARACTERES
                char cmd[5];
				
                for (int i = 0; i < 5; ++i){
                    cmd[i] = esp8266.read();
                }

                if (strcmp(cmd, "/read") == 0){
					
                    // INCREMENTA A CONTAGEM DE CONSULTAS
                    count++;

                    // FAZ A LEITURA DO SENSOR
                    float uvindex = readUV();

                    // MONTA A PAGINA
                    String page = "UV: ";
                    page += uvindex;

                    // MONTA A RESPOSTA
                    String resp = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
                    resp += page.length();
                    resp += "\r\n\r\n";
                    resp += page;

                    // COMANDO DO ENVIO DE DADOS
                    String cmdSend = "AT+CIPSEND=";
                    cmdSend += connId;
                    cmdSend += ",";
                    cmdSend += resp.length();
                    cmdSend += "\r\n";

                    sendData(cmdSend, 1000, DEBUG);
                    sendData(resp, 1000, DEBUG);
					
                } else if (strcmp(cmd, "/rst0") == 0){
                    
					// MONTA A PAGINA
                    String page = "Device Reset";

                    // MONTA A RESPOSTA
                    String resp = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
                    resp += page.length();
                    resp += "\r\n\r\n";
                    resp += page;

                    // COMANDO DO ENVIO DE DADOS
                    String cmdSend = "AT+CIPSEND=";
                    cmdSend += connId;
                    cmdSend += ",";
                    cmdSend += resp.length();
                    cmdSend += "\r\n";

                    sendData(cmdSend, 1000, DEBUG);
                    sendData(resp, 1000, DEBUG);

                    //RESETA O ARDUINO
                    wdt_enable(WDTO_30MS);
					
                } else if (strcmp(cmd, "/cont") == 0){
                    
					// MONTA A PAGINA
                    String page = "Contagem: ";
                    page += count;

                    // MONTA A RESPOSTA
                    String resp = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
                    resp += page.length();
                    resp += "\r\n\r\n";
                    resp += page;

                    // COMANDO DO ENVIO DE DADOS
                    String cmdSend = "AT+CIPSEND=";
                    cmdSend += connId;
                    cmdSend += ",";
                    cmdSend += resp.length();
                    cmdSend += "\r\n";

                    sendData(cmdSend, 1000, DEBUG);
                    sendData(resp, 1000, DEBUG);
					
                } else {
					
                    errorResponse(connId);
					
                }
				
            } else {
				
                errorResponse(connId);
				
            }

            // FECHA A CONEXÃO
            String cmdClose = "AT+CIPCLOSE=";
            cmdClose += connId;
            cmdClose += "\r\n";
            sendData(cmdClose, 3000, DEBUG);
			
        }
    }
}

float readUV(){
	
    int uvVolt = averageAnalogRead(PIN_UV);
    int ref3v3 = averageAnalogRead(PIN_3V3);

    // USA REFERÊNCIA PRA "MELHORAR" A LEITURA
    float voltage = 3.3f / ref3v3 * uvVolt;

    // MAPEIA A LEITURA PARA O ÍNDICE EQUIVALENTE USANDO A
    // TABELA DE REFERÊNCIA NO DATASHEET DO SENSOR
    float uvindex = mapFloat(voltage, 0.99, 2.9, 0.0, 15.0);

    return uvindex;
	
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max){
	
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	
}

// FAZ A MÉDIA DOS VALORES DE 8 LEITURAS, PARA EVITAR ERROS
int averageAnalogRead(int pinToRead){
    
	byte numberOfReadings = 8;
    unsigned int runningValue = 0;

    for (int x = 0; x < numberOfReadings; x++)
        runningValue += analogRead(pinToRead);
    runningValue /= numberOfReadings;

    return (runningValue);
	
}

String sendData(String command, const int timeout, boolean debug){
    
	// ENVIO DOS COMANDOS AT PARA O MODULO
    String response = "";
    esp8266.print(command);
    long int time = millis();
    while ((time + timeout) > millis()){
        while (esp8266.available()){
            // THE ESP HAS DATA SO DISPLAY ITS OUTPUT TO THE SERIAL WINDOW
			// READ THE NEXT CHARACTER.
            char c = esp8266.read();
            response += c;
        }
    }
    
	if (debug){
        Serial.print(response);
    }
	
    return response;
	
}