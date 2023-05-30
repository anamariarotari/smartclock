#include "IRremote.h"
#include <LiquidCrystal.h>
#include "RTClib.h"

#define ALARM_SET_HOURS 1
#define ALARM_SET_MINUTES 2
#define ALARM_SET_SECONDS 3
#define ALARM_SET 4
#define ALARM_RINGING 5

int hours;
int minutes;
int seconds;

int res = 0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int receiver = 9;
const int buzzer = 10;
char cmd[24];
volatile bool digitCmd;
RTC_DS1307 rtc;
int setAlarm;
char mathEquation[20]; // Adjust the size as per your requirement


IRrecv irrecv(receiver);
decode_results results;

void translateIR() {
  switch(results.value) {
    case 0xFFA25D: {
      strcpy(cmd, "PREV");
      digitCmd = false;
      break;
    }
    case 0xFF906F: {
      strcpy(cmd, "EQ");
      digitCmd = false;
      break;
    }
    case 0xFF6897: {
      strcpy(cmd, "0");
      digitCmd = true;
      break;
    }
    case 0xFF30CF: {
      strcpy(cmd, "1");
      digitCmd = true;
      break;
    }
    case 0xFF18E7: {
      strcpy(cmd, "2");
      digitCmd = true;
      break;
    }
    case 0xFF7A85: {
      strcpy(cmd, "3");
      digitCmd = true;
      break;
    }
    case 0xFF10EF: {
      strcpy(cmd, "4");
      digitCmd = true;
      break;
    }
    case 0xFF38C7: {
      strcpy(cmd, "5");
      digitCmd = true;
      break;
    }
    case 0xFF5AA5: {
      strcpy(cmd, "6");
      digitCmd = true;
      break;
    }
    case 0xFF42BD: {
      strcpy(cmd, "7");
      digitCmd = true;
      break;
    }
    case 0xFF4AB5: {
      strcpy(cmd, "8");
      digitCmd = true;
      break;
    }
    case 0xFF52AD: {
      strcpy(cmd, "9");
      digitCmd = true;
      break;
    }
    case 0xFFFFFFFF: {
      strcpy(cmd, "repeat");
      digitCmd = false;
      break;  
    }
  
    default: {
      strcpy(cmd, "other");
      digitCmd = false;
    }
  }

  delay(500);
}

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial.println("IR Receiver Button Decode"); 
  irrecv.enableIRIn();

  if(rtc.begin()&&rtc.isrunning()){
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void setHour() {
  lcd.setCursor(0, 0);
  lcd.print("Set hours");
  lcd.setCursor(0, 1);

  if (irrecv.decode(&results)) {
    translateIR();
    if (digitCmd) {
      hours = hours * 10 + atoi(cmd);
    }
    irrecv.resume();
 
   if (hours < 24) {
    lcd.print(hours);
    if (strcmp(cmd, "EQ") == 0) {
      setAlarm = ALARM_SET_MINUTES;
    }
   } else {
    lcd.clear();
    hours = 0;
    setAlarm = ALARM_SET_HOURS;
   }
  }
}

void setMinute() {
  lcd.setCursor(0,0);
  lcd.print("Set Minutes");
  lcd.setCursor(0,1) ;

  if (irrecv.decode(&results)) {
    translateIR();
    if (digitCmd) {
      minutes = minutes * 10 + atoi(cmd);
    }
    irrecv.resume();

   lcd.print(hours);
   lcd.print(':');
   if (minutes < 60) {
    lcd.print(minutes);
    if (strcmp(cmd, "EQ") == 0) {
      setAlarm = ALARM_SET_SECONDS;
    }
   } else {
    lcd.clear();
    minutes = 0;
    setAlarm = ALARM_SET_MINUTES;
   }
  }
}

void setSecond() {
  lcd.setCursor(0, 0);
  lcd.print("Set seconds");
  lcd.setCursor(0, 1);

  
  if (irrecv.decode(&results)) {
    translateIR();
    if (digitCmd) {
      seconds = seconds * 10 + atoi(cmd);
    }
    irrecv.resume();

   lcd.print(hours);
   lcd.print(':');
   lcd.print(minutes);
   lcd.print(':');
   
   if (seconds < 60) {
    lcd.print(seconds);  
    if (strcmp(cmd, "EQ") == 0) {
      setAlarm = ALARM_SET;
    }
   } else {
    lcd.clear();
    seconds = 0;
    setAlarm = ALARM_SET_SECONDS;
   }
  }
}



void getAlarm() {
  int ques1 = rand()%100+1;
  int ques2 = rand()%100+1; 
  int ans = ques1 + ques2;
  if (irrecv.decode(&results)) {
    translateIR(); 
    if (strcmp(cmd, "EQ") == 0) {
      setAlarm = ALARM_SET_HOURS;
    }
    irrecv.resume();
  }

  if (digitalRead(buzzer) == HIGH) {
    setAlarm = ALARM_RINGING;
  }
  while (setAlarm == ALARM_SET_HOURS) {
    setHour();
  }

  while (setAlarm == ALARM_SET_MINUTES) {
    setMinute();
  }

  while (setAlarm == ALARM_SET_SECONDS) {
    setSecond();
  }

  while (setAlarm == ALARM_SET) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Alarm-");
    
    if (hours < 10)
      lcd.print("0");
    lcd.print(hours);
    
    lcd.print(':');
    
    if(minutes < 10)
      lcd.print("0");
    lcd.print(minutes);
    
    lcd.print(':');
    
    if(seconds < 10)
      lcd.print("0");
    lcd.print(seconds);

    if (strcmp(cmd, "EQ") == 0) {
      setAlarm = 0;
    }
  }

  DateTime now = rtc.now();

  if (now.hour() == hours && now.minute() == minutes && now.second() == seconds) {
    tone(buzzer, 100);
    hours = 0;
    minutes = 0;
    seconds = 0;
  }

  if (irrecv.decode(&results)) {
    if (strcmp(cmd, "EQ") == 0) {
      hours = 0;
      minutes = 0;
      seconds = 0;
      lcd.setCursor(0, 1);
      lcd.clear();
    }
  }

  while (setAlarm == ALARM_RINGING) {
    lcd.setCursor(0, 0);
    lcd.print(ques1);
    lcd.print('+');
    lcd.print(ques2);
    lcd.print("      ");
    
    if(irrecv.decode(&results)){
      translateIR();
      if (digitCmd) {
        res = res * 10 + atoi(cmd);
      } else if (strcmp(cmd, "PREV") == 0) {
        res = 0;
      }
        irrecv.resume();
        lcd.print(res);
    }
    
    if (res == ans) {
      noTone(buzzer);
      setAlarm = 0;
      lcd.setCursor(0, 1);
      lcd.clear();
      res = 0;
    }
  }
}

void loop() {
  DateTime now = rtc.now();
  lcd.setCursor(0, 0);

  if (now.hour() < 10)
    lcd.print("0");
  lcd.print(now.hour());

  lcd.print(":");
  
  if (now.minute() < 10)
    lcd.print("0");
  lcd.print(now.minute());

  lcd.print(":");
   
  if (now.second() < 10)
    lcd.print("0");
  lcd.print(now.second());

  getAlarm();
}
