/*
  First atempt at serial comunications
*/
const int txClockPin = 3;
const int txPin = 4;
const int parityPin = 5;
const int resetPin = 6;
const int clockSpeed = 75;

char message[16] = "Hi"; // Message
bool parityCheck = true; // Parity check
int numOfBytes = 0;      // Number of bytes sent over serial

void setup()
{
  Serial.begin(9600);
  pinMode(txPin, OUTPUT);
  pinMode(txClockPin, OUTPUT);
  pinMode(parityPin, OUTPUT);
  pinMode(resetPin, INPUT);
  delay(1500);
  digitalWrite(txPin, LOW);
  digitalWrite(txClockPin, LOW);
  Serial.println("Start");
  Serial.println(message);
}

void loop()
{

  if (digitalRead(resetPin)) // Dugme je pritisnuto
  {
    while (digitalRead(resetPin)) // Dugme je pušteno
    {
    }
    digitalWrite(parityPin, LOW); // Restovati LED za indikaciju parnosti
    parityCheck = true;           // Postaviti parnost na parno (true) prije slanja poruke
    sendMessage(message);         // Posšalji poruku

    parityCheck ? sendParityBit(true) : sendParityBit(false); // Pošalji bit za parnost

    parityCheck ? digitalWrite(parityPin, HIGH) : digitalWrite(parityPin, LOW); // Upali ili ugasi LED za parnost
  }

  if (Serial.available()) // Dostupna nova poruka na serijskom portu
  {
    readSerial(message); // Pročitaj novu poruku
  };
}

//funkcija za slanje poruke
void sendMessage(char *TXstring)
{

  for (int byteIndex = 0; byteIndex <= strlen(TXstring); byteIndex++) // For koji prolazi kroz svaki karakter u poruci
  {

    char currByte = TXstring[byteIndex];

    for (int bitIndex = 0; bitIndex < 8; bitIndex++)
    { //For koji prolazi kroz svaki bit u trenutnom bajtu

      bool currBit = (0x80 >> bitIndex) & currByte; //Ako je na bitIndex mjestu u trenutnom bitu 1 postaviti currBit na true

      if (currBit)
      { //Ako je trenutni bit 1 poslati 1
        digitalWrite(txPin, HIGH);
        pulseClock();
        Serial.println('1');
        parityCheck = !parityCheck;
      }
      else
      { //Ako je trenutni bit 0 poslati 0
        digitalWrite(txPin, LOW);
        pulseClock();
        Serial.println('0');
      }
    }
  }
}

//Pulsiranje clocka
void pulseClock()
{
  delay(clockSpeed / 2);
  digitalWrite(txClockPin, HIGH);
  delay(clockSpeed / 2);
  digitalWrite(txClockPin, LOW);
}

void sendParityBit(bool send)
{
  if (send)
  {
    digitalWrite(txPin, HIGH);
    pulseClock();
    digitalWrite(txPin, LOW);
  }
}

void readSerial(char *charArr)
{ //Read a message over serial

  char tmpChar;

  tmpChar = Serial.read();
  Serial.println(tmpChar);

  if (tmpChar == 0x0A)
  { //If recieved byte is new line, end of message
    charArr[numOfBytes] = 0;
    numOfBytes = 0;
    Serial.println("Ready to send new message");
  }
  else
  {                                //If not new line go one
    charArr[numOfBytes] = tmpChar; //Add current byte to message
    numOfBytes++;
  };
}