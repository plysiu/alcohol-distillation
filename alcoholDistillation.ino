#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Button.h>
/*  
 * LCD LCM1602
 * VCC -> 5V
 * GND -> GND
 * SCL -> A5
 * SDA -> A4
 */
const int LCD_ROWS = 2;
const int LCD_COLS = 16;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
/*
 * Temperature sensor DS18B20
 * ONE_WIRE_BUS -> 6
 */
const int ONE_WIRE_BUS = 6;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); 
/*
 * 4 x Channel Relay SRD-05VDC-SL-C
 * IN0 -> 2
 * IN1 -> 3
 * IN2 -> 4
 * IN3 -> 5
 */
const int IN0 = 2;
const int IN1 = 3;
const int IN2 = 4;
const int IN3 = 5; 
const int NUMBER_OF_RELAYS = 4;

const boolean RELAY_ON = LOW;
const boolean RELAY_OFF = HIGH;

int channelRelayPorts[NUMBER_OF_RELAYS] = { IN0, IN1, IN2, IN3 };
boolean channelRelayStates[NUMBER_OF_RELAYS] = { RELAY_OFF, RELAY_OFF, RELAY_OFF, RELAY_OFF };
int relaysOnCounter = 0;

boolean getRelayState(const int index){
  return channelRelayStates[index];
}

void setRelayState(const int index, const int state){
  channelRelayStates[index] = state;
}

void setRelay(const int index, const boolean state){
  if(getRelayState(index) != state){
    digitalWrite(channelRelayPorts[index], state);
    if(state == RELAY_ON){
      relaysOnCounter++;
    }
    setRelayState(index, state);
  }
}

void setupRelay(const int index){
  pinMode(channelRelayPorts[index], OUTPUT);
  digitalWrite(channelRelayPorts[index], channelRelayStates[index]);

}

void setupRelays(){
  for(int i = 0; i < NUMBER_OF_RELAYS; i++){
    setupRelay(i);
  }
}

void turnOffAllRelays(){
  for (int i = 0; i < NUMBER_OF_RELAYS; i++){
    setRelay(i, RELAY_OFF);
  }
}

void turnOnAllRelays(){
  for (int i = 0; i < NUMBER_OF_RELAYS; i++){
    setRelay(i, RELAY_ON);
  }
}

int getNumberOfOnRelays(){
  int sum = 0; 
  for (int i = 0; i < NUMBER_OF_RELAYS; i++){
    if(getRelayState(i) == RELAY_ON){
      sum++;
    }
  }
  return sum;
}

void turnOnOneRelay(){
  if(getNumberOfOnRelays() != 1 ){
    turnOffAllRelays();
    setRelay(relaysOnCounter % NUMBER_OF_RELAYS, RELAY_ON);
  }
}

/*
 * Tact Switch -> 7
 * Tact Switch -> 8
 */
const int STAGE_UP_BUTTON_PIN = 7;
const int STAGE_DOWN_BUTTON_PIN = 8;

Button stageUp = Button(STAGE_UP_BUTTON_PIN, PULLUP);
Button stageDown = Button(STAGE_DOWN_BUTTON_PIN, PULLUP);

int stage = 0;
void checkButtonStage(){
  if(stageUp.uniquePress()){
    stage++;
  }
  if(stageDown.uniquePress()){
    stage--;
 }
}

void lcdInformation(int s, float t){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Stage:");
  lcd.print(s);
  lcd.setCursor(0, 1);
  lcd.print("Temp:");
  lcd.print(t);
    lcd.setCursor(12, 1);
    for(int i = 0; i < NUMBER_OF_RELAYS; i++){
          lcd.print(!getRelayState(i));
    }


}

void stage_0(){  
  turnOffAllRelays();
  lcd.setCursor(0, 0);
  lcd.print("Push the button");
  lcd.setCursor(0,1);
  lcd.print("to start. s:");
  lcd.print(stage);
}
const int GRADIENT = 1;
const float METHANOL_BOILING_POINT = 64.6;
void stage_1(){
  float temperature = sensors.getTempCByIndex(0);

  if(temperature < METHANOL_BOILING_POINT){
    turnOnAllRelays();
  }
  else{
    stage++;
  }
  lcdInformation(stage, temperature);
}

void stage_2(){
  float temperature = sensors.getTempCByIndex(0);

  if(temperature <  METHANOL_BOILING_POINT - GRADIENT){
    turnOnOneRelay();
  }
  else{
    if(temperature > METHANOL_BOILING_POINT + GRADIENT){
      turnOffAllRelays();
    }
  }
  lcdInformation(stage, temperature);
}

const float ETHANOL_BOILING_POINT = 78.38;
void stage_3(){
  float temperature = sensors.getTempCByIndex(0);

  if(temperature < ETHANOL_BOILING_POINT){
    turnOnAllRelays();
  }
  else{
    stage++;
  }
  lcdInformation(stage, temperature);
}

void stage_4(){
  float temperature = sensors.getTempCByIndex(0);

  if(temperature <  ETHANOL_BOILING_POINT - GRADIENT){
    turnOnOneRelay();
  }
  else{
    if(temperature > ETHANOL_BOILING_POINT + GRADIENT){
      turnOffAllRelays();
    }
  }
  lcdInformation(stage, temperature);
}

const float THIRD_BOILING_POINT = 83;
void stage_5(){
  float temperature = sensors.getTempCByIndex(0);

  if(temperature < THIRD_BOILING_POINT){
    turnOnAllRelays();
  }
  else{
    stage++;
  }
  lcdInformation(stage, temperature);
}

void stage_6(){
  float temperature = sensors.getTempCByIndex(0);

  if(temperature <  THIRD_BOILING_POINT - GRADIENT){
    turnOnOneRelay();
  }
  else{
    if(temperature > THIRD_BOILING_POINT + GRADIENT){
      turnOffAllRelays();
    }
  }
  lcdInformation(stage, temperature);
}

void setup(){
  lcd.begin(LCD_COLS, LCD_ROWS);
  sensors.begin();
  setupRelays();
  turnOffAllRelays();
//  Serial.begin(9600);
}

void loop(){
  checkButtonStage();
  sensors.requestTemperatures();
  switch(stage){
  case 0:
    stage_0();
    break;
  case 1:
    stage_1();
    break;
  case 2:
    stage_2();
    break;
  case 3:
    stage_3();
    break;
  case 4:
    stage_4();
    break;
  case 5:
      stage_5();
    break;
  case 6:
    stage_6();
    break;
  default :
    turnOffAllRelays();
    break;
  }
}


















