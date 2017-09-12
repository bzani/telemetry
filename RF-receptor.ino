/*
 * TRANSCEPTOR-RECEPTOR - ARDUINO UNO
 * Este processo irá apenas escutar a transmissão
 * do Arduino Mega (emissor) e mostrar a mensagem
 * na Serial
 */

#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>

// pinos CE e CSN
RF24 radio(9,10);

// enderecos de comunicacao dos modulos RF
const uint64_t PIPE_COMANDO = 0xE8E8F0F0E1LL;
const uint64_t PIPE_RESPOSTA = 0xE8E8F0F0E2LL;


const int CMD_INTERROGA=11;
const int CMD_PING=21;
const int CMD_TEMP=31;

int msg[5];

boolean temMensagem;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(PIPE_COMANDO);
  radio.openReadingPipe(1,PIPE_RESPOSTA);
  radio.startListening();
  pinMode(5, OUTPUT);
}

void piscaLed(int l) {
  digitalWrite(l, HIGH);
  delay(200);
  digitalWrite(l, LOW);
}  

void responde() {
  if (radio.available()) {
    boolean concluido=false;
    while (!concluido) {
      concluido=radio.read(msg, sizeof(msg));
    }  
    switch (msg[0]) {
      case CMD_TEMP:
        Serial.print("RPM: ");
        Serial.print(msg[2]);
        Serial.print(" | Velocidade: ");
        Serial.println(msg[1]);
        piscaLed(5);
        break;
      default:
        Serial.println("Recebi comando desconhecido ou improprio:");
        Serial.println(msg[0]);
    }
  }  
} 
 
void loop() {
  responde();
  delay(200);
}
