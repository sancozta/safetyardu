//#include <ESP8266WiFi.h>
//#include <ESP8266HTTPClient.h>

// DEFINIÇÕES DA REDE WIFI
//const char* SSID     = "Home Net";
//const char* PASSWORD  = "Brasil7x1";

// VARIAVEIS PARA SENSOR DE MOVIMENTAÇÃO
#define PINMOV 7
#define PINLED 3

// VERIFICAÇÃO DE MOVIMENTO
//boolean GETMOV = 0;

// ENDERECO API
//String DNSAPIHOST = "https://safetycc.herokuapp.com/logs";

// VARIAVEL HTTP PARA CLIENT
//HTTPClient http;

//INICIALIZAÇÃO DA APLICAÇÃO
void setup(){
  
  Serial.begin(9600);
  
  //DETERMINANDO ENTRADA DO PINO DO SENSOR DE MOVIMENTO
  pinMode(PINMOV, INPUT);

  //LIGANDO LED QUANDO TIVER MOVIMENTO
  pinMode(PINLED, OUTPUT);
  
  //INICIALIZANDO DADOS DO WIFI
  //initWiFi();
  
}

//FUNÇÃO LOOP
void loop() {

  //VERIFICANDO DETECÇÃO DE MOVIMENTO
  boolean GETMOV = digitalRead(PINMOV); 
  delay(1000);
  if(!GETMOV){
    
    Serial.println("## DETECTADO ##");
    digitalWrite(PINLED, HIGH);
    //REALIZANDO REQUISIÇÃO
    //registerLogs();

  } else {
    
    Serial.println("## -------- ##");
    digitalWrite(PINLED, LOW);
    
  }

}

// FUNCAO PARA INICIAR TENTATIVAS DE CONEXAO
void initWiFi(){
  /*
  Serial.println("## INICIANDO CONFIGURAÇÃO WIFI ##");
  
  delay(1000);
  
  Serial.println("CONECTANDO-SE NA REDE "+String(SSID));
  Serial.println("AGUARDE..");

  reconnectWiFi();
  */
}

// FUNÇÃO QUE CONECTA O NODEMCU NA REDE WIFI
void reconnectWiFi(){
   /*
  //VERIFICA SE ESTA CONECTADO
  if(WiFi.status() == WL_CONNECTED){
    return;
  }
  
  // SSID E PASSWORD DEVEM SER INDICADOS NAS VARIÁVEIS
  WiFi.begin(SSID, PASSWORD);

  //PROGRESSO DE TEMPO ATE QUE A CONEXAO SEJA ESTABELECIDA
  while(WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  
  //RETORNANDO DADOS DA CONEXAO
  Serial.println("CONECTADO COM SUCESSO NA REDE "+String(WiFi.SSID()));
  Serial.println("IP OBTIDO "+String(WiFi.localIP()));
  Serial.println("ENDERECO MAC "+String(WiFi.macAddress()));
  */
}

// FUNÇÃO REALIZA UM POST REQUEST A API EXTERNA
void registerLogs(){
  
  //Serial.println("Enviando Registros De Movimentação !");
  /*
  http.begin(DNSAPIHOST);
  
  http.addHeader("Authorization", "Basic YWRtaW46c2VjcmV0");
  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String body = "users_id=1&arduino=DFRE235677DF23";

  int code = http.POST(body);
  
  if (code != HTTP_CODE_OK) {
    Serial.println("REQUEST RRROR : " + code);
  }

  String response =  http.getString();
  
  http.end();

  Serial.println(response);
  */
  //Serial.println("Requisição Realizada Com Sucesso !");
  
}
