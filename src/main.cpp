#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <RTClib.h>

RTC_DS3231 rtc;

char timeString[10];

#define MAX_DEVICES 4  // 4 módulos de 8x8
#define CS_PIN 10

MD_Parola myDisplay = MD_Parola(MD_MAX72XX::FC16_HW, CS_PIN, MAX_DEVICES);

void setup() {
  myDisplay.begin();
  myDisplay.setIntensity(5);  // Brilho 0-15
  myDisplay.displayClear();
  if (!rtc.begin()){
    myDisplay.print("ERRO DE RTC");
    delay(5000);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

}
  

void loop(){
  DateTime now = rtc.now();
  sprintf(timeString, "%02d:%02d", now.hour(), now.minute());

  if (myDisplay.displayAnimate()) {
    myDisplay.displayText(timeString, PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    myDisplay.displayReset();
  }
}