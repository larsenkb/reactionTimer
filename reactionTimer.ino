//
// Uses 8 digit, 7-segment display using MAX7219

// using bluepill
// Board: Generic STM32F103C series
// Optimize: Smallest(default)
// Upload Method: STM32duino bootloader
// Variant: STM32F103CB (20k RAM, 128k Flash)
// CPU Speed(MHz): 72 MHz (Normal)
// Port: /dev/ttyACM0 (Maple Mini)

// 8 digit 7-segment display hook-up
#define DIN   PB12
#define CSN   PB13
#define CLK   PB14   // shifts data in on rising edge, shifts data out on falling edge

#define BUTTON PB3	// use internal pull-up


// Display Adapter Commands
// 8 digit, 7-segment display using MAX7219
#define NO_OP_ADDR            0x00
#define DIGIT_0_ADDR          0x01
#define DIGIT_1_ADDR          0x02
#define DIGIT_2_ADDR          0x03
#define DIGIT_3_ADDR          0x04
#define DIGIT_4_ADDR          0x05
#define DIGIT_5_ADDR          0x06
#define DIGIT_6_ADDR          0x07
#define DIGIT_7_ADDR          0x08
#define MODE_ADDR             0x09
#define INTENSITY_ADDR        0x0A
#define SCAN_LIMIT_ADDR       0x0B
#define SHUTDOWN_ADDR         0x0C
#define DISPLAY_TEST_ADDR     0x0F


void initPins()
{
  digitalWrite(CSN, HIGH);
  pinMode(DIN, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(CSN, OUTPUT);

  pinMode(BUTTON, INPUT_PULLUP);

}

void initDisplay()
{
  output(DISPLAY_TEST_ADDR, 0x00);
  output(SHUTDOWN_ADDR, 0x01);
  output(SCAN_LIMIT_ADDR, 0x07);
  output(MODE_ADDR, 0xff);
  output(INTENSITY_ADDR, 0x02);
}

void output(byte addr, byte data)
{
  digitalWrite(CSN, LOW);
  shiftOut(DIN, CLK, MSBFIRST, addr);
  shiftOut(DIN, CLK, MSBFIRST, data);
  digitalWrite(CSN, HIGH);
}

void disp_clear(void)
{
  byte addr = DIGIT_0_ADDR;
  
  do {
    output(addr++, 0x0f);
  } while (addr <= DIGIT_7_ADDR);
}

void disp_num(unsigned int nbr)
{
  byte addr = DIGIT_0_ADDR;
  
#if 1
  output(addr++, nbr % 10);
  do {
    nbr /= 10;
    output(addr++, (nbr == 0) ? 0x0f : nbr % 10);
  } while (addr <= DIGIT_7_ADDR);
#else
  output(addr++, nbr % 10);
  nbr /= 10;
  while (nbr > 0) {
    output(addr++, nbr % 10);
    nbr /= 10;
  };
  output(SCAN_LIMIT_ADDR, addr-2);
  output(INTENSITY_ADDR, (addr-2)<<1);
#endif
}

int seed;

void setup()
{
  Serial.begin(9600);
  initPins();
  initDisplay();
  disp_clear();

  seed = analogRead(PA0);
  seed ^= analogRead(PA1);
  seed ^= analogRead(PA2);
  seed ^= analogRead(PA3);
  randomSeed(seed);
}

int nbr = 99999900;
unsigned long rnd;
unsigned long startTime, stopTime;

#if 1
void loop()
{
  // delay a random time betwee  3 and 15 seconds
  rnd = random(3000,15000);
  delay(rnd);

  // set left digit to '0' to indicate start of reaction timing
  output(DIGIT_7_ADDR, 0);
  startTime = millis();

  // wait for button press or timeout (5 seconds)
  while(digitalRead(BUTTON) == HIGH) {
    if ((millis() - startTime) > 5000)
      break;
  }
  stopTime = millis();

  
  if ((stopTime - startTime) < 5000) { // button pressed
    disp_num(stopTime - startTime);
    delay(5000);
    disp_clear();
  } else {          // timed-out
    disp_clear();
    while(digitalRead(BUTTON) == LOW);
    while(digitalRead(BUTTON) == HIGH);
    output(DIGIT_3_ADDR, 0);
    while(digitalRead(BUTTON) == LOW);
    disp_clear();
//    for(;;);        // can only get out
  }
}
#else
void loop()
{
  rnd = random(3000,15000);
  delay(rnd);
  disp_num(random(3000,15000));
//  nbr = (nbr < 99999999) ? nbr+1 : 0;
//  Serial.print(random(3000,10000)); Serial.println("");
  delay(1000);
  disp_clear();
 
}
#endif
