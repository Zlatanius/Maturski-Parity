/*

*/
#include <LiquidCrystal_I2C.h>

const int RXledPin = 2;
volatile const int RXClockPin = 3;
const int parityPin = 4;
const int resetPin = 5;
const int RXDataPin = 6;

char message[16];                     // Message to send
int bitCounter = 0;                   // Counts the number of bits
bool parityCheck = true;              // Parity indicator
bool recievedParity = false;          // Indicates if parity bit has been recieved
volatile byte currByte = 0;           // Stores the curren byte
volatile bool currBit = false;        // Stores the current bit
volatile bool clockIndicator = false; // Indicator for if the clock has pulsed
volatile bool endOfMessage = false;   // End of message indicator

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{

  Serial.begin(9600);
  pinMode(RXDataPin, INPUT);
  pinMode(RXClockPin, INPUT);
  pinMode(resetPin, INPUT);
  pinMode(RXledPin, OUTPUT);
  pinMode(parityPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(RXClockPin), onClockPulse, RISING);

  strcpy(message, "");

  lcd.init();
  lcd.backlight();

  lcdPrint("Ready", 0, 0);
  delay(1000);
  lcdPrint("     ", 0, 0);
  Serial.println("Start recieving\n");
};

void loop()
{
  if (clockIndicator)
  { // Ako je detektovan clock puls
    updateLcd();
    clockIndicator = false;
  };

  if (endOfMessage && recievedParity)
  {
    Serial.println(message);
    Serial.println("End of message");
    lcdPrint("        ", 0, 1);                                                 //Clear display
    parityCheck ? digitalWrite(parityPin, HIGH) : digitalWrite(parityPin, LOW); //update praity LED
    parityCheck ? lcdPrint("Parity: even", 0, 1) : lcdPrint("Parity: odd", 0, 1);

    if (parityCheck)
    { //If parity is even report error
      Serial.println("Parity Error");
      delay(1000);
      lcdPrint("        ", 0, 1);
      lcdPrint("Error       ", 0, 1);
      endOfMessage = false;
    }
    resetVariables();
  };

  if (digitalRead(resetPin))
  { //Preapeare for a new message
    Serial.println("reset");
    while (digitalRead(resetPin))
    {
    };
    resetLcd();
  }
};

void onClockPulse()
{
  currBit = digitalRead(RXDataPin);
  //recievedParity = false;

  if (currBit)
  { //Recieved a 1

    currByte |= (0x80 >> bitCounter); //If a 1 is reacived set the current byte to 1 in the aproptiete place
    parityCheck = !parityCheck;
  };
  if (bitCounter == 8)
  { //Full byte recieved
    if (currByte == 0)
    {
      endOfMessage = true;
      recievedParity = true;
    }
    else
    {
      endOfMessage = false;
    };
    strncat(message, &currByte, 1); //Add the recieved byte to the overall message
    bitCounter = 0;
    currByte = 0; //Set the byte back to 0
  };

  bitCounter++;
  clockIndicator = true;
};

void updateLcd()
{
  lcdPrint(message, 0, 0); //Print the message so far
  for (int i = 0; i < 8; i++)
  { //Print the so far recieved bits
    if (i < bitCounter)
    {
      currByte &(0x80 >> i) ? lcdPrint("1", i, 1) : lcdPrint("0", i, 1);
    }
    else
    {
      lcdPrint(" ", i, 1);
    };
  };
};

void lcdPrint(char *printChar, int column, int row)
{ //Function to print on the LCD
  lcd.setCursor(column, row);
  lcd.print(printChar);
};

void resetLcd()
{ //Reset the LCD
  lcdPrint("                ", 0, 0);
  lcdPrint("                ", 0, 1);
  lcd.setCursor(0, 0);
}

void resetVariables()
{
  strcpy(message, "");
  bitCounter = 0;
  parityCheck = true;
  recievedParity = false;
  currByte = 0;
  currBit = false;
  clockIndicator = false;
  endOfMessage = false;
}