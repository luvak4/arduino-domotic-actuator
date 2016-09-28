//================================================================
// Halt, reboot, check status of a pfSense server through Arduino 
// and radio modules. 
// Project is divided in three parts: "keyboard", "servente" and "display".
//
// author: luvak4@gmail.com
//================================================================
//
/////////////////////////////////////
//// this is the "ACTUATOR" code ////
/////////////////////////////////////
#include <VirtualWire.h>
// max lenght of my message
const int MSG_LEN = 13;
// position of character to change
const int POSIZIONE_CARATT = 11;
// STEP
int internalStep=0;
// LEDs
const int pinLED =2;
// sensore analogico
const int sensorPin = A0;
// timing loop
int dutyCycle = 0;
unsigned long int Pa;
unsigned long int Pb;
// radio modules
const int transmit_pin = 12; 
const int receive_pin = 11; 
uint8_t buf[VW_MAX_MESSAGE_LEN];
uint8_t buflen = VW_MAX_MESSAGE_LEN;
// msg "received command"
///////////////////////////////////123456789012 
 char msgTxComandoRicevuto[13]  = "statACTU0007";
// prefix to transmit
/////////////////////////123456789012
char msgTxAnalogico[13]="anagACTU0000";
char msgTxDigitale[13] ="digiACTU0000";
//
String stringaRX;
int secondi=0;
//
//================================
// setup
//================================
void setup() {
  // LEDs
  pinMode(pinLED, OUTPUT);
  // radio tx rx
  vw_set_tx_pin(transmit_pin);
  vw_set_rx_pin(receive_pin);  
  vw_setup(2000);      
  vw_rx_start(); 
  //  Serial.begin(9600);
  // SPEGNE PIN 13 ACCESO DI DEFAULT
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
}
//================================
// loop
//================================
void loop() {
  //--------------------------------
  // time subdivision
  //--------------------------------
  unsigned long int Qa;
  unsigned long int Qb;
  int DIFFa;
  int DIFFb;
  int Xa;
  int Xb;
  //
  dutyCycle += 1;
  if (dutyCycle > 9){
    dutyCycle = 0;
  }
  if (dutyCycle > 0){
    Qa=millis();
    if (Qa >= Pa){
      DIFFa=Qa-Pa;
      Xa = DIFFa - 25;
      if (Xa >= 0){
  Pa = Qa;
  //--------------------------------
  // every 0.025 Sec
  //--------------------------------
      }
    } else {
      Pa = Qa - Xa;
    }
  } else {  
    Qb=millis();
    if (Qb >= Pb){
      DIFFb=Qb-Pb;
      Xb = DIFFb - 1000;
      if (Xb >= 0){
  Pb = Qb - Xb;
  //--------------------------------
  // every second
  //--------------------------------
  //
  secondi+=1;
  if (secondi > 120){
    secondi=0;
    txAnalogico();
  }
  //--------------------------------
  // BEGIN message received
  //--------------------------------
  if (vw_get_message(buf, &buflen)){
    vw_rx_stop(); 
    //
    stringaRX="";
    //
    // retriving which pushbutton
    // was pressed (ir-keyboard)
    for (int i = 1; i <= POSIZIONE_CARATT; i++){
      stringaRX += char(buf[i-1]);
    }
    //Serial.println(stringaRX);
    ////////////////123456789012
    if (stringaRX=="pulsACTU000"){
      switch (buf[POSIZIONE_CARATT]){
      case '1':
        // ACCENDI RELE
        txRicevutoComando();
        digitalWrite(pinLED,HIGH);
        break;
      case '2':
        // SPEGNI RELE
        txRicevutoComando();
        digitalWrite(pinLED,LOW);
        break;
      case '3':
        // LEGGI STATO RELE
        txDigitale();
        break;        
      case '4':
        // LEGGI ANALOGICO 0
        txAnalogico();
        break;
      }
    }
    delay(100);
    vw_rx_start(); 
  }
  //--------------------------------
  // END message received
  //--------------------------------
  //
      }
    } else {
      Pb = Qb;
    }      
  }
}

//================================
// send "command receive" to "display"
//================================
void txRicevutoComando(){
  //vw_rx_stop(); // disable rx section
  vw_send((uint8_t *)msgTxComandoRicevuto,MSG_LEN);
  vw_wait_tx(); // Wait until the whole message is gone
  //vw_rx_start(); // enable rx section
}

void txAnalogico(){
  char c[3];
  int sensorValue = analogRead(sensorPin);
  itoa(sensorValue, c, 10);
  //vw_rx_stop(); // disable rx section
  msgTxAnalogico[9]=c[0];
  msgTxAnalogico[10]=c[1];
  msgTxAnalogico[11]=c[2];
  vw_send((uint8_t *)msgTxAnalogico,MSG_LEN);
  vw_wait_tx(); // Wait until the whole message is gone
  //vw_rx_start(); // enable rx section
  //Serial.println(msgTxAnalogico);
  msgTxAnalogico[9]='0';
  msgTxAnalogico[10]='0';
  msgTxAnalogico[11]='0';
}



void txDigitale(){
  if (digitalRead(pinLED)){
    msgTxDigitale[11]='1';
  } else {
    msgTxDigitale[11]='0';
  }
  //vw_rx_stop(); // disable rx section
  vw_send((uint8_t *)msgTxDigitale,MSG_LEN);
  vw_wait_tx(); // Wait until the whole message is gone
  //vw_rx_start(); // enable rx section
  //Serial.println(msgTxDigitale);
  msgTxDigitale[11]='0';
}
