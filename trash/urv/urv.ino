#include <SoftwareSerial.h>
#include <avr/wdt.h>

// esp8266 conectado nos pinos 2 e 3 (TX e RX)
SoftwareSerial esp8266(2,3);
// debug do esp8266
#define DEBUG true

// SENSOR DE MOVIMENTACAO E DEBUG
#define PINMOV 7

// pinos usados pelo sensor UV ML8511
int PIN_UV = A0;  // pino da leitura do sensor
int PIN_3V3 = A1; // leitura de referência da alimentação 3V3

// conta o número de consultas
int count = 0;


void setup()
{
  Serial.begin(9600);

  pinMode(PIN_UV, INPUT);
  pinMode(PIN_3V3, INPUT);

  //DETERMINANDO ENTRADA DO PINO DO SENSOR DE MOVIMENTO
  pinMode(PINMOV, INPUT);

  esp8266.begin(9600);
  configWifi();
}

void configWifi()
{  
  sendData("AT+RST\r\n", 2000, DEBUG); // rst
  // Conecta a rede wireless
  sendData("AT+CWJAP=\"AndroidAP\",\"ronaldo123\"\r\n", 2000, DEBUG);
  delay(3000);
  // modo wifi número 1(estação-cliente)
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
  // Mostra o endereco IP
  sendData("AT+CIFSR\r\n", 1000, DEBUG);
  // Configura para multiplas conexoes(max=4)
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG);
}


// REALIZANDO REQUISICAO DA DETECCAO DE MOVIMENTO
void requestmov() {

  // AT+CIPCLOSE - ASSEGURA QUE A CONEXÃƒO 0 ESTÃ� LIVRE ASSIM PODENDO SER USADA
  sendData("AT+CIPCLOSE\r\n", 2000, true);

  // AT+CIPSTART - INICIA UMA CONEXAO COMO CLIENTE
  sendData("AT+CIPSTART=\"TCP\",\"safetyflask.herokuapp.com\",80\r\n", 2000, true);

  delay(1000);

  String request  = "";
  String body     = "arduino=DFEFEGNE24324";
  String command  = "";

  request += "POST /logs HTTP/1.1\r\n";
  request += "Host: safetyflask.herokuapp.com\r\n";
  request += "Authorization: Basic YWRtaW46c2VjcmV0\r\n";
  request += "Content-Type: application/x-www-form-urlencoded\r\n";
  request += "cache-control: no-cache\r\n";
  request += "Content-Length: "+String(body.length())+"\r\n";
  request += body + "\r\n";

  // AT+CIPSEND - DEFINIR O COMPRIMENTO DOS DADOS QUE SERAO ENVIADOS
  command = "AT+CIPSEND="+String(request.length())+"\r\n";

  sendData(command, 2000, true);

  sendData(request, 2000, true);

  delay(4000);

  sendData("AT+CIPCLOSE\r\n", 2000, true);

}

// envia uma resposta de erro
void errorResponse(int connId)
{
  // monta a pagina
  String page = "Not found";

  // monta a resposta
  String resp = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: ";
  resp += page.length();
  resp += "\r\n\r\n";
  resp += page;

  // comando do envio de dados
  String cmdSend = "AT+CIPSEND=";
  cmdSend += connId;
  cmdSend += ",";
  cmdSend += resp.length();
  cmdSend += "\r\n";

  sendData(cmdSend, 1000, DEBUG);
  sendData(resp, 1000, DEBUG);  
}

void server()
{
  if(esp8266.available())
  {
    String content = "";
    while(esp8266.available())
      content += (char)esp8266.read();

    Serial.print(content);
    
    if(esp8266.find("+IPD,"))
    {      
      delay(1000);
      // id da conexão
      int connId = esp8266.read() -48;

      if(esp8266.find("cmd"))
      {
        // comando de 5 caracteres
        char cmd[5];
        for(int i = 0; i < 5; ++i)
        {
          cmd[i] = esp8266.read();
        }

        if(strcmp(cmd, "/read") == 0)
        {
          // incrementa a contagem de consultas
          count++;
          
          // faz a leitura do sensor
          float uvindex = readUV();
    
          // monta a pagina
          String page = "UV: ";
          page += uvindex;
    
          // monta a resposta
          String resp = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
          resp += page.length();
          resp += "\r\n\r\n";
          resp += page;
    
          // comando do envio de dados
          String cmdSend = "AT+CIPSEND=";
          cmdSend += connId;
          cmdSend += ",";
          cmdSend += resp.length();
          cmdSend += "\r\n";
    
          sendData(cmdSend, 1000, DEBUG);
          sendData(resp, 1000, DEBUG);  
        }
        else if(strcmp(cmd, "/rst0") == 0)
        {
          // monta a pagina
          String page = "Device Reset";
    
          // monta a resposta
          String resp = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
          resp += page.length();
          resp += "\r\n\r\n";
          resp += page;
    
          // comando do envio de dados
          String cmdSend = "AT+CIPSEND=";
          cmdSend += connId;
          cmdSend += ",";
          cmdSend += resp.length();
          cmdSend += "\r\n";
    
          sendData(cmdSend, 1000, DEBUG);
          sendData(resp, 1000, DEBUG);  
          
          //reseta o arduino
          wdt_enable(WDTO_30MS);
        }
        else if(strcmp(cmd, "/cont") == 0)
        {
          // monta a pagina
          String page = "Contagem: ";
          page += count;
    
          // monta a resposta
          String resp = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
          resp += page.length();
          resp += "\r\n\r\n";
          resp += page;
    
          // comando do envio de dados
          String cmdSend = "AT+CIPSEND=";
          cmdSend += connId;
          cmdSend += ",";
          cmdSend += resp.length();
          cmdSend += "\r\n";
    
          sendData(cmdSend, 1000, DEBUG);
          sendData(resp, 1000, DEBUG);  
        }
        else
        {
          errorResponse(connId);
        }
      }
      else
      {
         errorResponse(connId);
      }

      // fecha a conexão
      String cmdClose = "AT+CIPCLOSE=";
      cmdClose += connId;
      cmdClose += "\r\n";

      sendData(cmdClose, 3000, DEBUG); 
    }
  }
}

void uploadReading()
{
  Serial.println("START--");

  // assegura que a conexão 0 está livre pra ser usada
  sendData("AT+CIPCLOSE=0\r\n", 2000, DEBUG);
  
  sendData("AT+CIPSTART=0,\"TCP\",\"35.199.84.135\",8000\r\n", 1000, DEBUG);
  delay(1000);

  String request = "POST /api/dados/ HTTP/1.1\r\nHost: 35.199.84.135\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";

  int uvindex = readUV();
  String content = "leituraUV=";
  content += uvindex;
  content += "&data=2018-10-16T00%3A00%3A00Z&dispositivo=1";

  request += content.length();
  request += "\r\n\r\n";
  request += content;
  request += "\r\n";
  
  String command = "AT+CIPSEND=0,";
  command += request.length();
  command += "\r\n";

  //Serial.print(command);
  //Serial.print(request);

  sendData(command, 1000, DEBUG);
  sendData(request, 1000, DEBUG);

  delay(5000);
  
  sendData("AT+CIPCLOSE=0\r\n", 1000, DEBUG);

  Serial.println("END--");
}

void loop()
{
  // envia uma leitura para o servidor
  uploadReading();

  //VERIFICANDO DETECÃ‡ÃƒO DE MOVIMENTO
  boolean GETMOV = digitalRead(PINMOV);

  delay(1000);

  if (GETMOV) {

    // REQUEST DE MOVIMENTO
    requestmov();

  }

  //configWifi();
  // liga o servidor escutando a porta 80
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG);

  // 1 minuto (60secs * 1000)rodando o server para atender as requisições
  unsigned long serverTime = 60000 + millis();
  while ( serverTime > millis())
  {
    server();
  }
}


float readUV()
{
  int uvVolt = averageAnalogRead(PIN_UV);
  int ref3v3 = averageAnalogRead(PIN_3V3);

  // usa referência pra "melhorar" a leitura
  float voltage = 3.3f / ref3v3 * uvVolt;

  // mapeia a leitura para o índice equivalente usando a
  // tabela de referência no datasheet do sensor
  float uvindex = mapfloat(voltage, 0.99, 2.9, 0.0, 15.0);

  return uvindex;
}


float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


// faz a média dos valores de 8 leituras, para evitar erros
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}

String sendData(String command, const int timeout, boolean debug)
{
  // Envio dos comandos AT para o modulo
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}
