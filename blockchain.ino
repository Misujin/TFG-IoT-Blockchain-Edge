#include <WiFi.h>
#include <string>
#include <cstring>
#include "config.h"
#include "JRPCGeth.h"
#include "ethereum.h"

//Led debug y sensor de sonido
int Led = 21; //G21
int SensorDigital = 18; //G18
int SensorAnalog = 33; //G33

//Timers comprobacion MCU sigue funcionando y puede registrar datos en la blockchain
unsigned long check2HpreviousMillis = 0;
//const long check2Hinterval = 3600000; //1h
const long check2Hinterval = 14400000; //4h

//Timers registro de sonido ilegal
unsigned long checkIllegalNoisePreviousMillis = 0;
unsigned long startIllegalFirstReadMillis = 0;
const long checkIllegalNoiseInterval = 300000; //5m
const long checkIllegalDurationInterval = 5000; //5s
bool isFirstIllegalRead = true;
bool isStartCountingNoiseStep = false;
unsigned long noiseCounter = 0;
int maxAnalogNoise = 0;

//Numero de intentos para registrar una transaccion en la blockchain a traves de la pasarela JRPC GETH
const int MAX_ATTEMPTS_GETH = 10;

                     
void setup() {
    pinMode (Led, OUTPUT) ;   // define LED as output interface
    pinMode (SensorDigital, INPUT) ;  // define Sound sensor as input interface
    pinMode (SensorAnalog, INPUT) ;  // define Sound sensor as input interface

    //Inicializamos los timers para que se haga un registro inicial del legal y el ilegal este preparado desde el principio
    check2HpreviousMillis = check2Hinterval;
    checkIllegalNoisePreviousMillis = checkIllegalNoiseInterval;
  
    adc1_config_width(ADC_WIDTH_BIT_12);
    Serial.begin(9600);

    for(uint8_t t = 3; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }
    Serial.println("");

    WiFi.begin(ENV_SSID, ENV_WIFI_KEY);

    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print("[WI-FI] Connecting...\n");
        // wait 1 second for re-trying
        delay(1000);
    }

    Serial.println("[WI-FI] Connected\n");

    showEthereumAddressData();
}

void loop() 
{
  //Lecturas analogica y digital del sensor de sonido
  int digitalNoise = digitalRead(SensorDigital);
  int analogNoise = map(analogRead(SensorAnalog), 0, 4095, 100, 0);

  //Registro del ruido de comprobacion legal cada x horas para ver que el dispositivo funciona
  if (millis() - check2HpreviousMillis >= check2Hinterval) {
    check2HpreviousMillis = millis();
    registerNoiseInEthereum(analogNoise, true, MAX_ATTEMPTS_GETH);
    noiseCounter = 0;
    Serial.println("------------------------- REGISTRADO SONIDO LEGAL DE COMPROBACION ---------------------------------------");
  }

  //Si se registra un sonido que supere el umbral se registra y se esperan 5 minutos antes de seguir registrando sonidos ilegales
  //Procesamiento transaccion (X intentos, sino para)
  if(digitalNoise){
    digitalWrite(Led, HIGH);
    //Comprobamos si es la primera lectura de un sonido que supera el umbral de ruido y guardamos el instante en el que sucedio
    if(isFirstIllegalRead){
      startIllegalFirstReadMillis = millis();
      isStartCountingNoiseStep = true;
      isFirstIllegalRead = false;
      noiseCounter = 0;
      maxAnalogNoise = 0;
    }
    //Si se ha producido una primera lectura de ruido ilegal y se han obtenido el numero de ruidos durante dos segundos continuamos...
    //la comprobacion de ruido prolongado durante 5s se hace para evitar registrar ruidos puntuales como que se caiga una caja por accidente
    //y para solventar el problema de sensibilidad del sensor de sonido (esto se podria solucionar con un modulo mas caro y preciso)
    if(isStartCountingNoiseStep && (millis() - startIllegalFirstReadMillis <= checkIllegalDurationInterval)){
      noiseCounter++;
      if(analogNoise > maxAnalogNoise){
        maxAnalogNoise = analogNoise;
      }
    }
  }
  else{
    digitalWrite(Led, LOW); 
  }

  //Una vez terminada la lectura de ruido durante 5s, comprobamos si supera un valor estimado que hace referencia a ruido constante en el sensor
  //aproximadamente, si el sensor recibiese ruido que supera el umbra durante todos los 5s, el valor del contador seria de ~340000
  if(isStartCountingNoiseStep && (millis() - startIllegalFirstReadMillis > checkIllegalDurationInterval)){
    Serial.println("analogNoise: " + String(analogRead(SensorAnalog)) + " | analogNoiseMapped: " + String(analogNoise) + " | maxAnalogNoise: " + String(maxAnalogNoise) + " | noiseCounter: " + String(noiseCounter));
    if(noiseCounter >= 50000 && (millis() - checkIllegalNoisePreviousMillis >= checkIllegalNoiseInterval)){
      //Si se detecta un ruido prolongado durante 5s y hace +5 minutos que no se registra ninguna transaccion en la blockchain, procesamos una nueva transaccion
      checkIllegalNoisePreviousMillis = millis();
      registerNoiseInEthereum(maxAnalogNoise, false, MAX_ATTEMPTS_GETH);
      Serial.println("------------------------- REGISTRADO SONIDO ILEGAL: " + String(maxAnalogNoise) + " ---------------------------------------");
      Serial.println();
    }
    //Reset variables controladoras del proceso
    isStartCountingNoiseStep = false;
    isFirstIllegalRead = true;
    maxAnalogNoise = 0;
    noiseCounter = 0;
  }
}
