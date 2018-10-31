// VARIAVEIS PARA SENSOR DE MOVIMENTAÇÃO
#define PINMOV 7
#define PINLED 3

//INICIALIZAÇÃO DA APLICAÇÃO
void setup(){

    Serial.begin(9600);

    //DETERMINANDO ENTRADA DO PINO DO SENSOR DE MOVIMENTO
    pinMode(PINMOV, INPUT);

    //LIGANDO LED QUANDO TIVER MOVIMENTO
    pinMode(PINLED, OUTPUT);

}

//FUNÇÃO LOOP
void loop(){

    //VERIFICANDO DETECÇÃO DE MOVIMENTO
    boolean GETMOV = digitalRead(PINMOV);

    delay(1000);

    if (GETMOV){

        Serial.println("## DETECTION ##");
        digitalWrite(PINLED, HIGH);

    } else {

        Serial.println("## NOT DETECTION ##");
        digitalWrite(PINLED, LOW);

    }

}