
#include "RTClib.h" // https://github.com/adafruit/RTClib

RTC_PCF8563 rtc;
bool rtc_active = false;

void setup() {
    Serial.begin(115200);
    if (! rtc.begin()) {
      for(int i = 0; i < 10; i++)
      {
        delay(500);
        Serial.println("Couldn't find RTC");
        Serial.flush();
      }
    } else {
      rtc_active = true;
    }

    if (rtc.lostPower()) {
        Serial.println("RTC is NOT initialized, let's set the time!");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
        //
        // Note: allow 2 seconds after inserting battery or applying external power
        // without battery before calling adjust(). This gives the PCF8523's
        // crystal oscillator time to stabilize. If you call adjust() very quickly
        // after the RTC is powered, lostPower() may still return true.
      }

    // When time needs to be re-set on a previously configured device, the
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    
    // When the RTC was stopped and stays connected to the battery, it has
    // to be restarted by clearing the STOP bit. Let's do this to ensure
    // the RTC is running.
    rtc.start();

    pinMode(9, OUTPUT);    // sets the digital pin 13 as output
    pinMode(10, OUTPUT);    // sets the digital pin 13 as output
    pinMode(11, OUTPUT);    // sets the digital pin 13 as output
}


#define NO_MODE_OVERRIDE -1
int mode_override = NO_MODE_OVERRIDE;

enum {
  MODE_sleeping,
  MODE_dawn,
  MODE_day,
  MODE_midday,
  MODE_late_afternoon,
  MODE_evening,
  MODE_late_evening,
};
#define MODE_default MODE_sleeping

void show_time();

bool command(void) {
   bool button_pressed = false;
   while(Serial.available()) {
      char inChar = (char)Serial.read(); // get the new byte:
      switch(inChar) {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
          Serial.print("Mode Override ");
          Serial.print(inChar);
          Serial.println(" - esc to cancel");
          mode_override = inChar-'1';
          button_pressed = true;
          break;

        case '\x1B':
          Serial.println("Cancel");
          mode_override = NO_MODE_OVERRIDE;
          button_pressed = true;
          break;

        case '+':
        {
            show_time();
            DateTime now = rtc.now();
            TimeSpan incr(60);
            now = now + incr;
            rtc.adjust(now);
            show_time();
            break;
        }

        case '-':
        {
            show_time();
              DateTime now = rtc.now();
              TimeSpan decrement(-60);
              now = now + decrement;
              rtc.adjust(now);
              show_time();
            break;
        }

        default:
          button_pressed = true;
          break;
        }
   }
   return button_pressed;
}

bool delay_with_command(unsigned long delay_in_ms) {
   unsigned long finish_time = millis() + delay_in_ms;
   bool button_pressed = false;
   while(millis() < finish_time && not button_pressed)
   {
      button_pressed = command();
   }
   return button_pressed;
}





int time_mode() {
  DateTime now = rtc.now();
  int simple_time = now.hour() * 100 + now.minute();

  int mode = MODE_default;
  if(now.dayOfTheWeek() == 6 || now.dayOfTheWeek() == 0)
  {
    // 
    // Weekend settings
    // 
    if(simple_time > 2200) {
      mode = MODE_sleeping;
    } else if (simple_time > 2100) {
      mode =  MODE_late_evening;
    } else if (simple_time > 1800) {
      mode =  MODE_evening;
    } else if (simple_time > 1630) {
      mode =  MODE_late_afternoon;
    } else if (simple_time > 1330) {
      mode =  MODE_day;
    } else if (simple_time > 1200) {
      mode =  MODE_midday;
    } else if (simple_time > 1000) {
      mode =  MODE_day;
    } else if (simple_time > 800) {
      mode =  MODE_dawn;
    } else {
      mode =  MODE_sleeping;
    }    
  }
  else
  {
    //
    // Weekday settings
    //
    
    if(simple_time > 2245) {
      mode = MODE_sleeping;
    } else if (simple_time > 2100) {
      mode =  MODE_late_evening;
    } else if (simple_time > 1800) {
      mode =  MODE_evening;
    } else if (simple_time > 1630) {
      mode =  MODE_late_afternoon;
    } else if (simple_time > 1300) {
      mode =  MODE_day;
    } else if (simple_time > 1200) {
      mode =  MODE_midday;
    } else if (simple_time > 830) {
      mode =  MODE_day;
    } else if (simple_time > 700) {
      mode =  MODE_dawn;
    } else {
      mode =  MODE_sleeping;
    }
  }
  return mode;
}

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
void show_time()
{
    DateTime now = rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
     Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    /*
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");

    // calculate a date which is 7 days, 12 hours and 30 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));

    Serial.print(" now + 7d + 12h + 30m + 6s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();
    */
}

void set_colour(uint8_t red, uint8_t green, uint8_t blue)
{
    analogWrite(11, red);
    analogWrite(10, green);
    analogWrite(9, blue);
}

uint8_t limit(int in)
{
  uint8_t ret = in;
  if(in > 255)
  {
    ret = 255;
  }
  if(in < 0)
  {
    ret = 0;
  }
  return ret;
}

bool fade(int time_ms, uint8_t r1,uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2)
{
#define STEP_TIME 25
  float t = (float)time_ms / (float)STEP_TIME;
  float dr = ((int)r2-(int)r1) / t;
  float dg = ((int)g2-(int)g1) / t;
  float db = ((int)b2-(int)b1) / t;
  float r_current = r1;
  float g_current = g1;
  float b_current = b1;
  float t_current = time_ms;
  bool button_pressed = false;
  while(t_current > 0 && button_pressed == false)
  {
    uint8_t r = limit(r_current);
    uint8_t g = limit(g_current);
    uint8_t b = limit(b_current);
    set_colour(r, g, b);

    button_pressed = delay_with_command(STEP_TIME);
    t_current -= STEP_TIME;
    r_current += dr;
    g_current += dg;
    b_current += db;
  }

  return button_pressed;
}

#define BLACK 0, 0, 0
#define RED 255, 0, 0
#define YELLOW 255, 255, 0
#define GREEN 0, 255, 0
#define CYAN 0, 255, 255
#define BLUE 0, 0, 255
#define MAGENTA 255, 0, 255

#define WHITE 255, 255, 255

#define COLOUR_CHANGE_TIME 2000

void rainbow()
{
  bool b = false;
  b = fade(COLOUR_CHANGE_TIME, RED, YELLOW); if(b) { return; }
  b = fade(COLOUR_CHANGE_TIME, YELLOW, GREEN); if(b) { return; }
  b = fade(COLOUR_CHANGE_TIME, GREEN, CYAN); if(b) { return; }
  b = fade(COLOUR_CHANGE_TIME, CYAN, BLUE); if(b) { return; }
  b = fade(COLOUR_CHANGE_TIME, BLUE, MAGENTA); if(b) { return; }
  b = fade(COLOUR_CHANGE_TIME, MAGENTA, RED); if(b) { return; }
}

void white_rainbow()
{
  bool b = false;
  b = fade(COLOUR_CHANGE_TIME, RED, YELLOW); if(b) { return; }
  b = fade(1000, YELLOW, WHITE); if(b) { return; }
  b = fade(1000, WHITE, YELLOW); if(b) { return; }
  b = fade(COLOUR_CHANGE_TIME, YELLOW, GREEN); if(b) { return; }
  b = fade(1000, GREEN, WHITE); if(b) { return; }
  b = fade(1000, WHITE, GREEN); if(b) { return; }
  b = fade(COLOUR_CHANGE_TIME, GREEN, CYAN); if(b) { return; }
  b = fade(1000, CYAN, WHITE); if(b) { return; }
  b = fade(1000, WHITE, CYAN); if(b) { return; }
  b = fade(COLOUR_CHANGE_TIME, CYAN, BLUE); if(b) { return; }
  b = fade(1000, BLUE, WHITE); if(b) { return; }
  b = fade(1000, WHITE, BLUE); if(b) { return; }
  b = fade(COLOUR_CHANGE_TIME, BLUE, MAGENTA); if(b) { return; }
  b = fade(1000, MAGENTA, WHITE); if(b) { return; }
  b = fade(1000, WHITE, MAGENTA); if(b) { return; }
  b = fade(COLOUR_CHANGE_TIME, MAGENTA, RED); if(b) { return; }
  b = fade(1000, RED, WHITE); if(b) { return; }
  b = fade(1000, WHITE, RED); if(b) { return; }
}

void all_off()
{  
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  delay_with_command(5000);
}

void dawn_starts()
{
  bool b = false;
  b = fade(1000, BLACK, WHITE); if(b) { return; }
  b = fade(30000, WHITE, RED); if(b) { return; }
  b = fade(30000, RED, YELLOW); if(b) { return; }
  b = fade(30000, YELLOW, CYAN); if(b) { return; }
  b = fade(30000, CYAN, WHITE); if(b) { return; }
  b = fade(1000, WHITE, BLACK); if(b) { return; }
}

void willow()
{
  bool b = false;
  b = fade(8000, GREEN, YELLOW); if(b) { return; }
  b = fade(8000, YELLOW, GREEN); if(b) { return; }
  //b = fade(5000, CYAN, GREEN); if(b) { return; }
}

void darkness_falls()
{
  bool b = false;
  b = fade(10000, BLACK, RED); if(b) { return; }
  b = fade(10000, RED, BLUE); if(b) { return; }
  b = fade(10000, BLUE, RED); if(b) { return; }
  b = fade(10000, RED, BLACK); if(b) { return; }
}


void loop() {
  int mode = time_mode();
  show_time();
  if(mode_override != NO_MODE_OVERRIDE)
  {
    mode = mode_override;
  }
    
  switch(mode) {
    case MODE_sleeping:
      Serial.println("Sleeping");
      all_off();
      break;
    case MODE_dawn:
      Serial.println("Dawn");
      dawn_starts();
      break;
    case MODE_day:
      Serial.println("Day");
      all_off();
      break;
    case MODE_midday:
      Serial.println("Midday");
      white_rainbow();
      break;
    case MODE_late_afternoon:
      Serial.println("Late afternoon");
      willow();
      break;
    case MODE_evening:
      Serial.println("Evening");
      rainbow();
      break;
    case MODE_late_evening:
      Serial.println("Late Evening");
      darkness_falls();
      break;
    default:
      Serial.println("Default");
      all_off();
      mode = 0;
      break;
  }
  //mode ++;

}
