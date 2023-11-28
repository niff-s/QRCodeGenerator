#include <RtcDS1302.h>
#include "qrcode.h"

#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 

#define TFT_CS        10
#define TFT_RST        8
#define TFT_DC         9

#define OLED_WIDTH 128
#define OLED_HEIGHT 160

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

ThreeWire myWire(4, 5, 2);  // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

char datestring[26];

void setup() {
  tft.initR(INITR_BLACKTAB);
  Rtc.Begin();

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);

  //time = millis() - time;

  //Serial.println(time, DEC);
  //delay(500);
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime();

  printDateTime(now);
  Serial.println();

  qrcodegen();

  delay(60000);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt) {
  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(datestring);
}

void qrcodegen() {
  //Create the QR code
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, 0, datestring);

  int scale = min (OLED_WIDTH / qrcode.size, OLED_HEIGHT / qrcode.size);

  int shiftX = (OLED_WIDTH - qrcode.size*scale)/2;
  int shiftY = (OLED_HEIGHT - qrcode.size*scale)/2;

  tft.fillScreen(ST77XX_BLACK);

  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule (&qrcode, x, y)) {
        tft.fillRect(shiftX + x*scale, shiftY + y*scale, scale, scale, ST77XX_WHITE);
      }      
    }
  }

}
