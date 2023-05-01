/*
This program controls the music system and reset button 
*/

#include <avr/io.h>
#include <Volume.h>
#include <avr/interrupt.h>
#include "pitches.h"

unsigned long timeBuzzer0;
unsigned long timeBuzzer1;
volatile int value = 0;

#define joyX A0
#define joyY A1
#define musicPin 5
#define buzzerPin 13
#define resetPin 3
#define END -1
#define REST 0

// Selection of songs
int melodies[2][220] = {

  // Indian folk song
  {
    NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_A4,
    NOTE_G4, NOTE_C5, NOTE_AS4, NOTE_A4,                   
    NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_FS4, NOTE_DS4, NOTE_D4,
    NOTE_C4, NOTE_D4,0,                                 

    NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_A4,
    NOTE_G4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_A4,      //29               //8
    NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_FS4, NOTE_DS4, NOTE_D4,
    NOTE_C4, NOTE_D4,0,                                       

    NOTE_D4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_DS5, NOTE_D5,
    NOTE_C5, NOTE_AS4, NOTE_A4, NOTE_C5,
    NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_FS4, NOTE_D5, NOTE_C5,
    NOTE_AS4, NOTE_A4, NOTE_C5, NOTE_AS4,             //58

    NOTE_D4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_DS5, NOTE_D5,
    NOTE_C5, NOTE_D5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_A4, NOTE_C5, NOTE_G4,
    NOTE_A4, 0, NOTE_AS4, NOTE_A4, 0, NOTE_G4,
    NOTE_G4, NOTE_A4, NOTE_G4, NOTE_FS4, 0,

    NOTE_C4, NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_DS4,
    NOTE_C4, NOTE_D4, 0,
    NOTE_C4, NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_DS4,
    NOTE_C4, NOTE_D4, END
  },

  // Pirates
  {
    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, REST,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, REST,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, REST,
    NOTE_A4, NOTE_G4, NOTE_A4, REST,
    
    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, REST,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, REST,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, REST,
    NOTE_A4, NOTE_G4, NOTE_A4, REST,
    
    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, REST,
    NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, REST,
    NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, REST,
    NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, REST,
    
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, REST,
    NOTE_D5, NOTE_E5, NOTE_A4, REST,
    NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, REST,
    NOTE_C5, NOTE_A4, NOTE_B4, REST,
    
    NOTE_A4, NOTE_A4, END
  }
};

// Note durations for respective songs
int durations[2][220] = 
  {
  
    // Indian Folk Song
    {
      8,4,8,4,
      4,4,4,12,
      4,4,4,4,4,4,
      4,16,4,

      8,4,8,4,
      4,2,1,1,2,1,1,12,
      4,4,4,4,4,4,
      4,16,4,

      4,4,4,4,4,4,
      4,4,4,12,
      4,4,4,4,4,4,
      4,4,4,12,

      4,4,4,4,4,4,
      2,1,1,2,1,1,4,8,4,
      2,6,4,2,6,4,
      2,1,1,16,4,

      4,8,4,4,4,
      4,16,4,
      4,8,4,4,4,
      4,20,
    },

    // Pirates
    {

      8, 8, 4, 8, 8,
      8, 8, 4, 8, 8,
      8, 8, 4, 8, 8,
      8, 8, 4, 8,
      
      8, 8, 4, 8, 8,
      8, 8, 4, 8, 8,
      8, 8, 4, 8, 8,
      8, 8, 4, 8,
      
      8, 8, 4, 8, 8,
      8, 8, 4, 8, 8,
      8, 8, 4, 8, 8,
      8, 8, 8, 4, 8,
      
      8, 8, 4, 8, 8,
      4, 8, 4, 8,
      8, 8, 4, 8, 8,
      8, 8, 4, 4,
      
      4, 8
    }
  };

int speeds[3] = {
  50, 30
};

Volume vol;
int volume = 30;
int speed = 10;
int trackIndex = 0;


void setup() {
  vol.begin();
  usart_init();

  pinMode(musicPin, OUTPUT);
  pinMode(resetPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(resetPin), resetHitISR, RISING);
  
}

void loop() {  
  

   for (int i = 0; melodies[trackIndex][i] != -1; i++) {

    if(millis() - timeBuzzer1 >= 250){
      noTone(musicPin); 
    }

    int noteDuration = speeds[trackIndex] * durations[trackIndex][i];

    if (melodies[trackIndex][i] == 0)
    {
      vol.delay(noteDuration * .95);
    }
    else
    {
      vol.tone(melodies[trackIndex][i], volume);
      // Serial.println("Tone");
      vol.delay(noteDuration * .95);
      // vol.noTone();
      // Serial.println("No Tone");
    }
    

    // check joystick position
    int xValue = analogRead(joyX);
    int yValue = analogRead(joyY);

    // change volume if joystick has been shifted left or right
    if (xValue == 1023 && volume < 250) {
      volume += 10;
    }
    else if (xValue == 0 && volume > 0) {
      volume -= 10;
    }

    // change track if joystick has been shifted up or down     
    if (yValue == 1023) {
      trackIndex = (trackIndex + 1) % 2;
      i = 0;
    }
  }

  delay(3000);
}

void usart_init() {
  // DDRB = 0xFF; //Port B is output
  UCSR0B = (1<<RXEN0) | (1<<TXEN0);
  UCSR0B |= (1 << RXCIE0); //enable interrupt on RXC0 flag
  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
  //baud rate = 9600
  UBRR0L = 103;
}

ISR(USART_RX_vect) {
  while (! (UCSR0A & (1<<RXC0)));

    value = UDR0; //integer received from transmitter

    // turn led on/off based on integer received
    if (value == 1) 
    {
      tone(buzzerPin, 500); 
      timeBuzzer1 = millis();
    }
    else if (value == 0) 
    {
      tone(buzzerPin, 1000);
      timeBuzzer1 = millis();
    }
}


void resetHitISR() {
  // send a 1 to cause a reset
  UDR0 = 1;
}



