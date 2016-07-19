/**********************************************************
                Fast PWM Test
Demostrates the generation of high speed PWM
using timers 1 and 4
There are two pieces of code:
One for pins 9, 10 and 11 using TIMER 1
with frequencies up to 62kHz
Other for pins 6 and 13 using TIMER 4
with frequencies up to 187kHz
History:
  12/12/2014 - Version 1.0
  22/12/2014 - Adding a missing OCR4C value

  http://r6500.blogspot.com/2014/12/fast-pwm-on-arduino-leonardo.html
  
***********************************************************/

/**********************************************************
   Fast PWM on pins 9,10,11 (TIMER 1)
   
   Do not use analogWrite to pins 9, 10 or 11 if using 
   this functions as they use the same timer.
   
   Those functions will probably conflict with the 
   servo library.
   
   Uses 5 PWM frequencies between 61Hz and 62.5kHz
   
**********************************************************/

// Frequency modes for TIMER1
#define PWM62k   1   //62500 Hz
#define PWM8k    2   // 7812 Hz
#define PWM1k    3   //  976 Hz
#define PWM244   4   //  244 Hz
#define PWM61    5   //   61 Hz

// Direct PWM change variables
#define PWM9   OCR1A
#define PWM10  OCR1B
#define PWM11  OCR1C

// Configure the PWM clock
// The argument is one of the 5 previously defined modes
void pwm91011configure(int mode)
{
// TCCR1A configuration
//  00 : Channel A disabled D9
//  00 : Channel B disabled D10
//  00 : Channel C disabled D11
//  01 : Fast PWM 8 bit
TCCR1A=1;

// TCCR1B configuration
// Clock mode and Fast PWM 8 bit
TCCR1B=mode|0x08;  

// TCCR1C configuration
TCCR1C=0;
}

// Set PWM to D9
// Argument is PWM between 0 and 255
void pwmSet9(int value)
{
OCR1A=value;   // Set PWM value
DDRB|=1<<5;    // Set Output Mode B5
TCCR1A|=0x80;  // Activate channel
}

// Set PWM to D10
// Argument is PWM between 0 and 255
void pwmSet10(int value)
{
OCR1B=value;   // Set PWM value
DDRB|=1<<6;    // Set Output Mode B6
TCCR1A|=0x20;  // Set PWM value
}

// Set PWM to D11
// Argument is PWM between 0 and 255
void pwmSet11(int value)
{
OCR1C=value;   // Set PWM value
DDRB|=1<<7;    // Set Output Mode B7
TCCR1A|=0x08;  // Set PWM value
}


/**********************************************************
   Fast PWM on pins 6, 13 (High Speed TIMER 4)
   
   Do not use analogWrite to pins 6 or 13 if using 
   this functions as they use the same timer.
   
   Those functions will conflict with the 
   MSTIMER2 library.
   Uses 7 PWM frequencies between 2930Hz and 187.5kHz
   
   Timer 4 uses a PLL as input so that its clock frequency
   can be up to 96MHz on standard Arduino Leonardo.
   We limit imput frequency to 48MHz to generate 187.5kHz PWM
   If needed, we can double that up to 375kHz
**********************************************************/

// Frequency modes for TIMER4
#define PWM187k 1   // 187500 Hz
#define PWM94k  2   //  93750 Hz
#define PWM47k  3   //  46875 Hz
#define PWM23k  4   //  23437 Hz
#define PWM12k  5   //  11719 Hz
#define PWM6k   6   //   5859 Hz
#define PWM3k   7   //   2930 Hz

// Direct PWM change variables
#define PWM6        OCR4D
#define PWM13       OCR4A

// Terminal count
#define PWM6_13_MAX OCR4C

// Configure the PWM clock
// The argument is one of the 7 previously defined modes
void pwm613configure(int mode)
{
// TCCR4A configuration
TCCR4A=0;

// TCCR4B configuration
TCCR4B=mode;

// TCCR4C configuration
TCCR4C=0;

// TCCR4D configuration
TCCR4D=0;

// TCCR4D configuration
TCCR4D=0;

// PLL Configuration
// Use 96MHz / 2 = 48MHz
// PLLFRQ=(PLLFRQ&0xCF)|0x30;
PLLFRQ=(PLLFRQ&0xCF)|0x10; // Will double all frequencies

// Terminal count for Timer 4 PWM
OCR4C=255;
}

// Set PWM to D6 (Timer4 D)
// Argument is PWM between 0 and 255
void pwmSet6(int value)
{
OCR4D=value;   // Set PWM value
DDRD|=1<<7;    // Set Output Mode D7
TCCR4C|=0x09;  // Activate channel D
}

// Set PWM to D13 (Timer4 A)
// Argument is PWM between 0 and 255
void pwmSet13(int value)
{
OCR4A=value;   // Set PWM value
DDRC|=1<<7;    // Set Output Mode C7
TCCR4A=0x82;  // Activate channel A
}

/*************** ADDITIONAL DEFINITIONS ******************/

// Macro to converts from duty (0..100) to PWM (0..255)
#define DUTY2PWM(x)  ((255*(x))/100)

/**********************************************************/

////////////////////////////////////////
//////////        WHOA        //////////
//////////        WHOA        //////////
//////////        WHOA        //////////
//////////        WHOA        //////////
//////////        WHOA        //////////
////////////////////////////////////////

// Pins 

int hvDigitalIn = 19; // pf6 -> A1   D19
int hvClock = 18;     // pf7 -> A0   D18

int clearPin = 6;     // pd7 -> A7   D6#
int sense1 = A6;      // pd4 -> A6   D4
int sense2 = A11;     // pd6 -> A11  D12
int sense3 = 8;       // pb4 -> A8   D8
int sense4 = 9;       // pb5 -> A9   D9

int elEnable = 20;    // pf5 -> A2   D20

// State 

#define pwmDepth 4

#define channelCount 4
#define switchedPass 5
int iter = 0;
int switched[] = {0, 0, 0, 0};
bool glow[] = {true, true, true, true};
bool noGlow[] = {false, false, false, false};
int pwmState[] = {pwmDepth, pwmDepth, pwmDepth, pwmDepth};
int senseChannel[] = {sense1, sense2, sense3, sense4};

#define prevValSize 50
int prevVal[prevValSize][channelCount];

#define sampleCount 3
int sampleAll[channelCount][sampleCount];
int sampleAllVal[channelCount];

int samples[sampleCount];
int sampleVal = 0;



int del = 0;
int blah = 0;
int prev = 0;

int tempGlow[channelCount];

// Helpers 

void switchOutputs(bool* glow) { 
  for (int i = 0; i < channelCount; i++) {
    tempGlow[i] = glow[i];
  }
  // Make pins on the layout correspond to the pins on the switch 
  tempGlow[0] = glow[1];
  tempGlow[1] = glow[0];

  for (int i = 0; i < 8; i++) {
//    Serial.print(tempGlow[i / 2]);
    digitalWrite(hvDigitalIn, tempGlow[i / 2]);
    digitalWrite(hvClock, LOW); 
    delayMicroseconds(8);
    digitalWrite(hvClock, HIGH);
  }
}

void sort(int *a, int len)
{
   for (int i = 1; i < len; ++i)
   {
     int j = a[i];
     int k;
     for (k = i - 1; (k >= 0) && (j < a[k]); k--)
     {
       a[k + 1] = a[k];
     }
     a[k + 1] = j;
   }
}

unsigned long t1;
unsigned long t2;

int sense(int channel) {
     for (int i = 0; i < 1; i++) {
      // TODO, decouple this - can reduce flashing.
      digitalWrite(elEnable, LOW);
      delayMicroseconds(300);
      switchOutputs(noGlow);
      delayMicroseconds(30);
      digitalWrite(clearPin, HIGH);
      delayMicroseconds(50);
      digitalWrite(elEnable, HIGH);
      digitalWrite(clearPin, LOW);
    //  delayMicroseconds(20000);
      delay(4);
  
      samples[i] = analogRead(senseChannel[channel]);
      
      Serial.print(samples[i]);
      Serial.print(", ");

   }

   sort(samples, sampleCount);
   // Average the median three samples.
   sampleVal = samples[0];
//   for(int i = -1; i < 2; i++) {
//     sampleVal += samples[sampleCount / 2 + i];
//   }

   digitalWrite(elEnable, HIGH);

   if (switched[channel] > switchedPass) {
      switched[channel] = 0;
      return 0;
   }
   else if (switched[channel] > 0) { 
      switched[channel]++;
      return 0;
   }
   return sampleVal;
}

// This is currently unused (and unfinished) 
// It will blink more, but can be more convenient for experiementing.
//int* senseAll() {
//     for (int i = 0; i < sampleCount; i++) {
//      // TODO, decouple this - can reduce flashing.
//      switchOutputs(noGlow);
//      delayMicroseconds(10);
//      digitalWrite(clearPin, LOW);
//      delayMicroseconds(200);
//      
//      digitalWrite(clearPin, HIGH);
//      delayMicroseconds(500);
//
//      for (int channel = 0; channel < channelCount; channel++) {
//        if (switched[channel] > switchedPass) {
//          samples[channel][i] = 0;
//          return 0;
//        }
//        else if (switched[channel] > 0) { 
//          if (i == 0) { 
//            switched[channel]++;
//          }
//          samples[channel][i] = 0;
//        }
//        else {
//          samples[channel][i] = analogRead(senseChannel[channel]);
//        }
//      }
//
//   }
//   switchOutputs(glow);
//
//   for (int channel = 0; channel < channelCount; j++) {
//     sort(samples[channel], sampleCount);
//     // Average the median three samples.
//     for(int i = -1; i < 2; i++) {
//       sampleAllVal[channel] += samples[channel][sampleCount / 2 + i];
//     }
//   }
//
//   return sampleAllVal;
//}

// This is pretty hack-y.  Done because mirco will half the pwm frequency sometimes.
void ensureCorrectFrequency() {
  switchOutputs(noGlow);
  pwm613configure(PWM94k);
  pwmSet13(127); // set to 50% duty cycle for symmetry.
  int read94k = sense(1);
  pwm613configure(PWM187k);
  pwmSet13(127); // set to 50% duty cycle for symmetry.
  int read187k = sense(1);


// TODO: make this actually work...
//  if (read94k > read187k) { 
//    pwm613configure(PWM94k);
//    pwmSet13(127);
//  }
}

//////////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~///////////

void setup() {
  Serial.begin(9600);  

  // hvSwitch communication
  pinMode(hvDigitalIn, OUTPUT);
  pinMode(hvClock, OUTPUT);

  pinMode(clearPin, OUTPUT);
  digitalWrite(clearPin, LOW);

  pinMode(10, OUTPUT);

  pinMode(elEnable, OUTPUT);
  digitalWrite(elEnable, HIGH);

  pinMode(sense1, INPUT);
  pinMode(sense2, INPUT);
  pinMode(sense3, INPUT);
  pinMode(sense4, INPUT);
  
  analogReference(DEFAULT);

  // this should be called after the clear pin is set high
  ensureCorrectFrequency();
  
}

int val = 0;
bool shouldGlow = true;
// High level flow: 
// Sense -> Update -> Glow?
// Concerns: 
// How do you do PWM on top of sensing?  
// Where to do MIDI, wireless communication? 
// Balancing lag and wireless comm with other things.  
void loop() {
 //switchOutputs(glow);
//    switchOutputs(noGlow);
//     delay(1000);

// digitalWrite(clearPin, LOW);

    
    switchOutputs(noGlow);
    if (shouldGlow) {
       switchOutputs(glow);
    //   delay(1000);
    }
    delay(700);
    val = sense(2);
    Serial.println(val);
    Serial.println((val / 1024.0) * 3.3 / 1.0);
// //   switchOutputs(glow);
// //   delay(100);
// //   Serial.println((sense(1) / 1024.0) * 3.3 / 3.0);
// //    Serial.print(val / 3.0);
// //    Serial.print("  ---  ");
// // //   Serial.print(prevVal - val);
// //    Serial.print("  ---  ");
// //    Serial.println((val / 1024.0) * 3.3 / 3.0);
//  
     if (val != 0 && 
     //    prevVal[(iter - 2) % prevValSize][0] != 0 && 
     // bike jacket 35? 
         prevVal[(iter - 5) % prevValSize][0] - val > 200) { 
        Serial.println("BLAH BLAH BLAH BLAH");
            //    shouldGlow = !shouldGlow;
        for(int i = 0; i < 4; i++) { 
          glow[0] = true;
          glow[1] = false;
          switchOutputs(glow);
          delay(300);
          glow[0] = false;
          glow[1] = true;
          switchOutputs(noGlow);
          delay(300);
        }
        switched[0]++;
     }

     iter++;
     prevVal[iter % prevValSize][0] = val;
}
