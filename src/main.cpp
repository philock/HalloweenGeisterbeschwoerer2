#include <Arduino.h>
#include <MD_YX5300.h>
#include <HardwareSerial.h>
#include <input.h>

#define UART_MP3_PLAYER  2
#define PIN_RELAIS       14
#define PIN_TRIGGER 21

HardwareSerial  MP3Stream(UART_MP3_PLAYER);  // MP3 player serial stream for comms
MD_YX5300 mp3(MP3Stream);
const uint8_t PLAY_FOLDER = 1;   // tracks are all placed in this folder

Input trigger(PIN_TRIGGER, false, true);

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

bool mp3IsPlaying(){
  /* mp3.queryStatus();
  mp3.check();
  const MD_YX5300::cbData *status = mp3.getStatus();

  uint16_t data = status->data;

  uint8_t playState = data & 0xFF;  // Extract the low byte
  return (playState == 0x01); */
  return false;
}

void startSound(){
  mp3.playTrack(1);
    
  // Trigger lightshow
  digitalWrite(PIN_RELAIS, LOW);
  delay(300);
  digitalWrite(PIN_RELAIS, HIGH);

  delay(222*1000); 
}

void stopSound(){
  //mp3.playTrack(2);
  //mp3.playStop();
    
  // Trigger lightshow
  /* digitalWrite(PIN_RELAIS, LOW);
  delay(300);
  digitalWrite(PIN_RELAIS, HIGH); */

  //delay(2000); 
}

void setup(){
  Serial.begin(9600);
  Serial.println("start");

  // set the hardware pins
  pinMode(PIN_RELAIS, OUTPUT);
  digitalWrite(PIN_RELAIS, HIGH);

  // initialize mp3 player
  MP3Stream.begin(MD_YX5300::SERIAL_BPS);
  mp3.begin();
  mp3.setSynchronous(true);
  //mp3.playFolderRepeat(PLAY_FOLDER);
  mp3.volume(mp3.volumeMax());

  //mp3.playStop();

  trigger.limitRate(5);
  trigger.setDebounceTime(200);
  trigger.setLongpressTime(1000);
  //trigger.setActivationHandler(startSound);
  trigger.setLongpressHandler(startSound);
}

void loop(){
  // call in each loop iteration to poll mp3 status
  if(mp3.check()){        
    cbResponse(mp3.getStatus());
  }

  trigger.poll();

  delay(10);
}
