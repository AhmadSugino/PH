#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

#define SensorPin 0          // pH sensor pin
#define ECSensorPin A1       // EC sensor pin
#define relayPin 8           // Relay pin
#define solenoidPinAsam 9    // Solenoid pin for acid
#define solenoidPinBasa 10   // Solenoid pin for base

#define sampling 5           // Number of samples for averaging

unsigned long int avgValuePH;  // Store the average value of the pH sensor feedback
unsigned long int avgValueEC;  // Store the average value of the EC sensor feedback
float b;
int buf[10], temp;

void setup()
{
  Serial.begin(9600);
  Serial.println("Ready");

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print(" pH & EC Sensor ");
  lcd.setCursor(0, 1);
  lcd.print(" EIF - SOHAIL ");
  delay(2000);
  lcd.clear();

  pinMode(relayPin, OUTPUT);
  pinMode(solenoidPinAsam, OUTPUT);
  pinMode(solenoidPinBasa, OUTPUT);
}

void loop()
{
  // pH sensor reading
  for (int i = 0; i < 10; i++)
  {
    buf[i] = analogRead(SensorPin);
    delay(10);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValuePH = 0;
  for (int i = 2; i < 8; i++)
    avgValuePH += buf[i];
  float phValue = (float)avgValuePH * 5.0 / 1024 / 6;
  phValue = 3.5 * phValue;

  // EC sensor reading
  int adcEC = 0;
  for (int i = 0; i < sampling; i++)
  {
    adcEC += analogRead(ECSensorPin);
  }
  adcEC = adcEC / sampling;
  float vEC = (adcEC * 5.0) / 1023;
  float EC = (4.982 * vEC) - 0.08465;

  // pH and EC values output
  Serial.print("pH: ");
  Serial.print(phValue, 2);
  Serial.print("\t EC: ");
  Serial.print(EC, 2);
  Serial.println(" ms/cm");

  lcd.setCursor(0, 0);
  lcd.print("pH: ");
  lcd.print(phValue, 2);

  lcd.setCursor(0, 1);
  lcd.print("EC: ");
  lcd.print(EC, 2);

  // Control relay based on EC value
  if (EC > 1.5)
  {
    digitalWrite(relayPin, HIGH);
  }
  else
  {
    digitalWrite(relayPin, LOW);
  }

  // Control solenoid based on pH value
  if (phValue > 7)
  {
    // Open solenoid for acid
    digitalWrite(solenoidPinAsam, HIGH);
    digitalWrite(solenoidPinBasa, LOW);
  }
  else if (phValue < 7)
  {
    // Open solenoid for base
    digitalWrite(solenoidPinAsam, LOW);
    digitalWrite(solenoidPinBasa, HIGH);
  }
  else
  {
    // Close both solenoids if pH is 7
    digitalWrite(solenoidPinAsam, LOW);
    digitalWrite(solenoidPinBasa, LOW);
  }

  delay(800);
}
