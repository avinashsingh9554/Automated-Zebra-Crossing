#include <IRremote.h>
#include <LiquidCrystal.h>

#define RS_PIN 12
#define E_PIN 11
#define D4_PIN 4
#define D5_PIN 7
#define D6_PIN A5
#define D7_PIN 10
#define IR_RECEIVE_PIN 8
#define LED_PIN_R 6
#define LED_PIN_G 5
#define ECHO_PIN 2
#define TRIGGER_PIN 3

int cursorLine = 0;

unsigned long lastTimeUltrasonicTrigger = millis();
unsigned long ultrasonicTriggerDelay = 60;

volatile unsigned long pulseInTimeBegin;
volatile unsigned long pulseInTimeEnd;
volatile bool newDistanceAvailable = false;
double previousDistance = 400.0;
LiquidCrystal lcd(RS_PIN, E_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

void triggerUltrasonicSensor()
{
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
}

double getUltrasonicDistance()
{
  double durationMicros = pulseInTimeEnd - pulseInTimeBegin;
  double distance = durationMicros / 58.0; // cm (148.0: inches)
  if (distance > 400.0) {
    return previousDistance;
  }
  distance = previousDistance * 0.7 + distance * 0.3;
  previousDistance = distance;
  return distance;
}
void echoPinInterrupt()
{
  if (digitalRead(ECHO_PIN) == HIGH) { // start measuring
    pulseInTimeBegin = micros();
  }
  else { // stop measuring
    pulseInTimeEnd = micros();
    newDistanceAvailable = true;
  }
}
void printDistanceOnLCD(double distance)
{
  lcd.setCursor(0, 1);
  lcd.print("Distance: ");
  lcd.print(distance);
  lcd.print("  ");
}

void setup() {
  Serial.begin(19200);
  pinMode(LED_PIN_R,OUTPUT);
  pinMode(LED_PIN_G,OUTPUT);
  lcd.begin(16,2);
  lcd.print("WELCOME");
  IrReceiver.begin(IR_RECEIVE_PIN);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Rate: ");
  lcd.print(ultrasonicTriggerDelay);
  lcd.print(" ms.");

  attachInterrupt(digitalPinToInterrupt(ECHO_PIN),
                  echoPinInterrupt,
                  CHANGE);
}
void loop() {
  digitalWrite(LED_PIN_R,HIGH);
  if (IrReceiver.decode()) {
    IrReceiver.resume();
    Serial.println("HII");
    Serial.println(IrReceiver.decodedIRData.command);
  }
  int code = IrReceiver.decodedIRData.command;
  
  digitalWrite(LED_PIN_R,HIGH);
  unsigned long timeNow = millis();
  
  if (timeNow - lastTimeUltrasonicTrigger > ultrasonicTriggerDelay) {
    lastTimeUltrasonicTrigger += ultrasonicTriggerDelay;
    triggerUltrasonicSensor();
  }

  if (newDistanceAvailable) {
    newDistanceAvailable = false;
    double distance = getUltrasonicDistance();
    printDistanceOnLCD(distance);
  }
  double distance = getUltrasonicDistance();
    if(distance <= 30)
  {
    digitalWrite(LED_PIN_R,LOW);
    digitalWrite(LED_PIN_G,HIGH);
    delay(1000);
  }
  else
  {
    digitalWrite(LED_PIN_G,LOW);
    delayMicroseconds(1000);
    digitalWrite(LED_PIN_R,HIGH);
  }
}
