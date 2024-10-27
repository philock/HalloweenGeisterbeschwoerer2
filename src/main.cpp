#include <Arduino.h>
#include <MD_YX5300.h>
#include <HardwareSerial.h>
#include <FastLED.h>

#define UART_MP3_PLAYER  2
#define PIN_RELAIS       4
#define PIN_PUPPET       27
#define PIN_LED_STRIPE   22
#define PIN_LIGHTBARRIER 21

#define LED_TYPE    WS2813
#define COLOR_ORDER GRB
#define NUM_LEDS    60
#define BRIGHTNESS  15
CRGB leds[NUM_LEDS];
const CRGB color = CRGB::White;

HardwareSerial  MP3Stream(UART_MP3_PLAYER);  // MP3 player serial stream for comms
MD_YX5300 mp3(MP3Stream);
const uint8_t PLAY_FOLDER = 1;   // tracks are all placed in this folder

// Used to process device responses either as a library callback function
// or called locally when not in callback mode.
void cbResponse(const MD_YX5300::cbData *status){
  Serial.print("Response: ");

  switch (status->code)
  {
  case MD_YX5300::STS_OK:         Serial.print(F("STS_OK"));         break;
  case MD_YX5300::STS_TIMEOUT:    Serial.print(F("STS_TIMEOUT"));    break;
  case MD_YX5300::STS_VERSION:    Serial.print(F("STS_VERSION"));    break;
  case MD_YX5300::STS_CHECKSUM:   Serial.print(F("STS_CHECKSUM"));    break;
  case MD_YX5300::STS_TF_INSERT:  Serial.print(F("STS_TF_INSERT"));  break;
  case MD_YX5300::STS_TF_REMOVE:  Serial.print(F("STS_TF_REMOVE"));  break;
  case MD_YX5300::STS_ERR_FILE:   Serial.print(F("STS_ERR_FILE"));   break;
  case MD_YX5300::STS_ACK_OK:     Serial.print(F("STS_ACK_OK"));     break;
  case MD_YX5300::STS_FILE_END:   Serial.print(F("STS_FILE_END"));   break;
  case MD_YX5300::STS_INIT:       Serial.print(F("STS_INIT"));       break;
  case MD_YX5300::STS_STATUS:     Serial.print(F("STS_STATUS"));     break;
  case MD_YX5300::STS_EQUALIZER:  Serial.print(F("STS_EQUALIZER"));  break;
  case MD_YX5300::STS_VOLUME:     Serial.print(F("STS_VOLUME"));     break;
  case MD_YX5300::STS_TOT_FILES:  Serial.print(F("STS_TOT_FILES"));  break;
  case MD_YX5300::STS_PLAYING:    Serial.print(F("STS_PLAYING"));    break;
  case MD_YX5300::STS_FLDR_FILES: Serial.print(F("STS_FLDR_FILES")); break;
  case MD_YX5300::STS_TOT_FLDR:   Serial.print(F("STS_TOT_FLDR"));   break;
  default: Serial.print(F("STS_??? 0x")); Serial.print(status->code, HEX); break;
  }

  Serial.print(F(", 0x"));
  Serial.print(status->data, HEX);
}

void leds_on(bool s){
  if(s){
    for(int i =0; i< NUM_LEDS; i++){
        leds[i] = CRGB::Red;
    }
  }
  else{
    for(int i =0; i< NUM_LEDS; i++){
        leds[i] = CRGB::Black;
    }
  }

  FastLED.show();  
}

void flicker_once(bool flick_on, int t_min, int t_max){
  int t = random(t_min, t_max);

  if(flick_on){
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
    delay(t);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
  else{
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(t);
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
  }
}

void flicker(bool stay_on, int n_flick){ 
  const int t_min_on = 20;
  const int t_max_on = 100;

  const int t_min_off = 50;
  const int t_max_off = 330;

  if(stay_on){
    for(int i = 0; i < n_flick; i++){
      int t_on = random(t_min_on, t_max_on);
      int t_off = random(t_min_off, t_max_off);

      fill_solid(leds, NUM_LEDS, color);
      FastLED.show();
      delay(t_on);

      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
      delay(t_off);
    }

    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
  }
  else{
    for(int i = 0; i < n_flick; i++){
      int t_on = random(t_min_on, t_max_on);
      int t_off = random(t_min_off, t_max_off);

      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
      delay(t_off);

      fill_solid(leds, NUM_LEDS, color);
      FastLED.show();
      delay(t_on);
    }

    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
}

void trigger_puppet(){
  digitalWrite(PIN_PUPPET, HIGH);
  delay(100);
  digitalWrite(PIN_PUPPET, LOW);
}

void guide_lights(){
  flicker(true, 6);

    // keep light on and flicker randomly
    for(int i = 0; i < 50; i++){
      if(random(1,101) <= 4) flicker_once(false, 30, 250);
      /* if(random(0,2))fadeToBlackBy(leds, NUM_LEDS, 2);
      else fadeLightBy(leds, NUM_LEDS, 2);
      FastLED.show(); */
      delay(100);
    }

    // lights off
    leds_on(false);
}

void strobe(int t){
  int period = random(30,100);
  for(int i = 0; i < t/(2*period); i++){
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
    delay(50);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(50);
  }
}

void setup(){
  Serial.begin(9600);
  Serial.println("start");

  // set the hardware pins
  pinMode(PIN_RELAIS, OUTPUT);
  pinMode(PIN_PUPPET, OUTPUT);
  pinMode(PIN_LIGHTBARRIER, INPUT_PULLUP);

  digitalWrite(PIN_RELAIS, HIGH);
  digitalWrite(PIN_PUPPET, LOW);

  FastLED.addLeds<LED_TYPE,PIN_LED_STRIPE,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 450);
  leds_on(false);
  /*fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
  delay(2000);
  fill_solid(leds, NUM_LEDS, CRGB::Wheat);
  FastLED.show();
  delay(2000);
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
  FastLED.show();
  delay(2000);
  fill_solid(leds, NUM_LEDS, CRGB::BlueViolet);
  FastLED.show();
  delay(2000);
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  FastLED.show();
  delay(2000);
  fill_solid(leds, NUM_LEDS, CRGB::Violet);
  FastLED.show();
  delay(2000);
  fill_solid(leds, NUM_LEDS, CRGB::DarkOrange);
  FastLED.show();
  delay(2000); */

  // initialize global libraries
  MP3Stream.begin(MD_YX5300::SERIAL_BPS);
  mp3.begin();
  mp3.setSynchronous(true);
  mp3.playFolderRepeat(PLAY_FOLDER);
  mp3.volume(mp3.volumeMax());
}

void loop(){
  // call in each loop iteration to poll mp3 status
  /* if(mp3.check()){        
    cbResponse(mp3.getStatus());
  } */

  // when the lightbarrier is triggered
  if(!digitalRead(PIN_LIGHTBARRIER)){

    trigger_puppet(); // start puppet

    delay(10000); // wait 500 ms

  }

  delay(50);
}
