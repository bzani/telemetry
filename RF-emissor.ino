/*
 * TRANSCEPTOR-EMISSOR - ARDUINO MEGA
 * Este processo se encarrega de transmitir as mensagens
 * ao Arduino Uno (receptor), além de mostrar no display
 * LCD informações correntes dos sensores e realizar as
 * medições necessárias dos sensores
 */

#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <LiquidCrystal.h>

// -- TRANSCEPTOR --
// pinos CE e CSN
RF24 radio(49,53);

const uint64_t PIPE_COMANDO = 0xE8E8F0F0E1LL;
const uint64_t PIPE_RESPOSTA = 0xE8E8F0F0E2LL;

const int CMD_INTERROGA=11;
const int CMD_PING=21;
const int CMD_TEMP=31;

int msg[5];

const byte pinoChave=2;

// -- DISPLAY --
// pinos da interface
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);
int pos = 0;

// -- RPM --
int rpm[8];
int rpmCalc=0;
int vel=0;

// -- ETC --
int tempoEnvioCte=0;

// -- FUNCOES AUXILIARES --
void enviaMsg() {
  radio.stopListening();
  radio.write(msg, sizeof(msg));
  radio.startListening();
}
 
void piscaLed(int l) {
  digitalWrite(l, HIGH);
  delay(200);
  digitalWrite(l, LOW);
}  

void interroga(int comando, int v, int r) {
  msg[0]=comando; 
  if (comando == CMD_TEMP) {
    msg[1]=v;
    msg[2]=r;
  }
  piscaLed(3);
  enviaMsg();
} 

void atualizaDisplay(int vel, int rpm) {
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Veloci.| ");
  lcd.print(vel);
  lcd.setCursor(0, 1);
  lcd.print("RPM    | ");
  lcd.print(rpm);
}

// -- SETUP --
void setup() {
  Serial.begin(9600);

  // transceptor
  pinMode(pinoChave,INPUT_PULLUP);
  radio.begin();
  radio.openWritingPipe(PIPE_COMANDO);
  radio.openReadingPipe(1,PIPE_RESPOSTA);
  radio.startListening();

  // push-buttons
  pinMode(3, OUTPUT);

  // display lcd
  lcd.begin(16, 2);
  lcd.print("Iniciando...    ");
    lcd.setCursor(0, 1);
  lcd.print("  Display...    ");
}

// -- LOOP --
void loop() {

  // envio temporizado de rpm
  if (tempoEnvioCte > 3) {
    for (int i=0; i<8; i++) {
      rpm[i] = analogRead(i);
      rpmCalc = rpmCalc + rpm[i];
    }
    rpmCalc = rpmCalc / 16 * 10;
    vel = rpmCalc/80;
    interroga(CMD_TEMP, vel, rpmCalc);
    tempoEnvioCte = 0;
  }
  else
    tempoEnvioCte = tempoEnvioCte + 1;

  // display
  atualizaDisplay(vel,rpmCalc);
 
  // delay
  delay(100);
}
