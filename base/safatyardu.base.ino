#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

// DEFINIÇÕES DA REDE WIFI
const char* SSID 		= "NameWifi";
const char* PASSWORD 	= "PasswordWifi";

// ENDEREÇO IP LOCAL DO SERVIDOR WEB INSTALADO NA RASPBERRY PI 3
// ONDE SERÁ EXIBIDA A PÁGINA WEB
const char* rpiHost = "192.168.0.24";  

// SERVIDOR QUE DISPONIBILIZA SERVIÇO DE GEOLOCALIZAÇÃO VIA IP    
const char* IpApiHost = "ip-api.com";

WiFiClient client;

// FUNÇÃO QUE CONECTA O NODEMCU NA REDE WIFI
// SSID E PASSWORD DEVEM SER INDICADOS NAS VARIÁVEIS

void reconnectWiFi(){
	
	if(WiFi.status() == WL_CONNECTED){
		return;
	}
	
	WiFi.begin(SSID, PASSWORD);

	while(WiFi.status() != WL_CONNECTED) {
		delay(100);
		Serial.print(".");
	}

	Serial.println();
	Serial.print("Conectado com sucesso na rede: ");
	Serial.println(SSID);
	Serial.print("IP obtido: ");
	Serial.println(WiFi.localIP());
	
}

void initWiFi(){
	
	Serial.println("\nIniciando configuração WiFi\n");
	
	delay(10);
	
	Serial.print("Conectando-se na rede: ");
	Serial.println(SSID);
	Serial.println("Aguarde");

	reconnectWiFi();
	
}

// FUNÇÃO QUE REALIZA GET REQUEST NO SITE IP-API.COM
// ESSE SITE DISPONIBILIZA UMA API DE GEOLOCALIZAÇÃO VIA IP
// A FUNÇÃO RETORNA UM JSON COM DADOS DE GEOLOCALIZAÇÃO
// OS DADOS DE GEOLOCALIZAÇÃO SÃO EXIBIDOS NA PAGINA WEB EM UM GOOGLE MAPS

String makeGETlocation(){
	
	Serial.println("\nRequisitando Geolocalização via IP\n");
  
	if(!client.connect(IpApiHost, 80)){

		Serial.println("connection ao ip-api.com falhou");
		return "connection failed";
		
	}
  
	// REALIZA HTTP GET REQUEST
	client.println("GET /json/?fields=8405 HTTP/1.1");
	client.print("Host: ");
	client.println(IpApiHost);
	client.println("Connection: close");
	client.println();

	// RECEBE O HEADER DE RESPOSTA, 
	// CADA LINHA DO HEADER TERMINA COM "\R\N", A ULTIMA LINHA É FAZIA, OU SEJA, "\R\N" APENAS
	while (client.connected()) {
		
		// LÊ UMA LINHA ATÉ O "\N"
		String data = client.readStringUntil('\n'); 
		Serial.println(data);
		
		// A ULTIMA LINHA DO HEADER É VAZIA, OU SEJA APENAS "\R\N", SE LERMOS ATÉ "\N", TEMOS APENAS "\R"
		if (data == "\r") {     
			break;
		}
		
	}
	
	// RECEBE OS DADOS DE GEOLOCALIZAÇÃO EM FORMATO JSON E GUARDA NA VARIÁVEL DATA
	String data = client.readStringUntil('\n');
	Serial.println("Dados de geolocalização recebidos\n");
	Serial.println(data);  
	return data; 
}

// FUNÇÃO QUE ENVIA AO SERVIDOR A LOCALIZAÇÃO DO NODEMCU
// FUNÇÃO REALIZA UM POST REQUEST AO SERVIDOR NO LINK /LOCATION
// O SERVIDOR POR SUA VEZ EXIBE A LOCALIZAÇÃO DO NODEMCU NO GOOGLE MAPS

void makePOSTlocation(){
	
	// GUARDA O JSON DE GEOLOCALIZAÇÃO NA VARIÁVEL LOCATION
	String location = makeGETlocation(); 
	
	Serial.println("\nEnviando geolocalização ao servidor\n");
	
	// AQUI CONECTAMOS AO SERVIDOR
	if(!client.connect(rpiHost, 3000)){
		
		Serial.print("Could not connect to host: \n");
		Serial.print(rpiHost);
		
	} else {
		
		// REALIZA HTTP POST REQUEST    
		client.println("POST /location HTTP/1.1");
		client.println("Host: 192.168.0.24");
		client.println("Content-Type: application/json");
		client.print("Content-Length: ");
		client.println(location.length());
		client.println();
		// ENVIAMOS O JSON AO SERVIDOR
		client.println(location);    
		Serial.println("\nLocalização enviada com sucesso!");
		
	}
	
}

//INICIALIZAÇÃO DA APLICAÇÃO
void setup(){
	
	Serial.begin(115200);
	
	//INICIALIZANDO DADOS DO WIFI
	initWiFi();
	
	//REALIZANDO REQUISIÇÃO
	makePOSTlocation();
	
}

//FUNÇÃO LOOP
void loop() {

}