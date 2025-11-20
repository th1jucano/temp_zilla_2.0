#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <RTClib.h>
#include <DHT.h>

RTC_DS3231 rtc;

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

char timeString[10];
char tempString[20];
char humString[20];
int displayMode = 0;
int passagem = 35;
unsigned long ultimaLeitura = 0;
unsigned long ultimaAnimacao = 0;

#define MAX_DEVICES 4  // 4 módulos de 8x8
#define CS_PIN 10



MD_Parola myDisplay = MD_Parola(MD_MAX72XX::FC16_HW, CS_PIN, MAX_DEVICES);

void setup() {
  Serial.begin(9600);
  myDisplay.begin();
  myDisplay.setIntensity(15);  // Brilho 0-15
  myDisplay.displayClear();
  if (!rtc.begin()){
    myDisplay.print("ERRO DE RTC");
    delay(5000);
  }
  // FORCE O AJUSTE - COMENTE ESSA LINHA DEPOIS!
  //rtc.adjust(DateTime(2025, 11, 18, 19, 30, 0));  // Coloque a hora EXATA de agora!
  dht.begin();

}
  

void loop(){

  // Watchdog: Se a animação travar por mais de 30 segundos, reseta
  if (millis() - ultimaAnimacao > 30000 && ultimaAnimacao > 0) {
    myDisplay.displayReset();
    displayMode = 0;
    ultimaAnimacao = millis();
  }

  DateTime now = rtc.now();
  static float temp = 25.0;  // variável estática mantém valor entre loops
  static float hum = 50.0;

  if (millis() - ultimaLeitura >=10000){
    float tempLida = dht.readTemperature();
    float humLida = dht.readHumidity();
    
    // Só atualiza se a leitura for válida
    if (!isnan(tempLida) && tempLida > -40 && tempLida < 80) {
      temp = tempLida * 0.83;
    }
    if (!isnan(humLida) && humLida > 0 && humLida <= 100) {
      hum = humLida * 1.17;
    }
    
    ultimaLeitura = millis();
    Serial.print("T:");
    Serial.print(temp);
    Serial.print(" H:");
    Serial.println(hum);
  }

  

  if (myDisplay.displayAnimate()){
    ultimaAnimacao = millis();  // Reseta watchdog
    
    if (displayMode == 0){
      // Etapa 1: Blinds aparecendo e hora fica visível
      sprintf(timeString, "%02d:%02d", now.hour(), now.minute());
      myDisplay.displayText(timeString, PA_CENTER, 100, 2000, PA_BLINDS, PA_PRINT);
      displayMode = 10;
    }else if(displayMode == 10){
      // Etapa 2: Blinds desaparecendo
      myDisplay.displayText(timeString, PA_CENTER, 100, 0, PA_PRINT, PA_BLINDS);
      displayMode = 1;
    }
    else if (displayMode == 1){
      if (isnan(temp)) {
        sprintf(tempString, "Temp: ERRO TEMP");
      } else {
        sprintf(tempString, "Temp: %dC", (int)temp);
      }
      myDisplay.displayText(tempString, PA_CENTER, passagem, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      displayMode = 2;

    }
    else {
      if (isnan(hum)) {
        sprintf(humString, "Umid: ERRO HUMID");
      } else {
        sprintf(humString, "Umid: %d%%", (int)hum);
      }
      myDisplay.displayText(humString, PA_CENTER, passagem, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      displayMode = 0;

    }
    
    myDisplay.displayReset();

  }
}