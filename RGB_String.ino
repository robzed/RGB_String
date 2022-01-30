
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

enum {
  MODE_default = 0,
  MODE_sleeping,
  MODE_dawn,
  MODE_day,
  MODE_midday,
  MODE_late_afternoon,
  MODE_evening,
  MODE_late_evening,
};

int time_mode() {
  DateTime now = rtc.now();
  int simple_time = now.hour() * 100 + now.minute();

  int mode = MODE_default;
  if(now.dayOfTheWeek() == 6 || now.dayOfTheWeek() == 0)
  {
    if(simple_time > 2200) {
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

void fade(int time_ms, uint8_t r1,uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2)
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
  while(t_current > 0)
  {
    uint8_t r = limit(r_current);
    uint8_t g = limit(g_current);
    uint8_t b = limit(b_current);
    set_colour(r, g, b);

    delay(STEP_TIME);
    t_current -= STEP_TIME;
    r_current += dr;
    g_current += dg;
    b_current += db;
  }
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
  fade(COLOUR_CHANGE_TIME, RED, YELLOW);
  fade(COLOUR_CHANGE_TIME, YELLOW, GREEN);
  fade(COLOUR_CHANGE_TIME, GREEN, CYAN);
  fade(COLOUR_CHANGE_TIME, CYAN, BLUE);
  fade(COLOUR_CHANGE_TIME, BLUE, MAGENTA);
  fade(COLOUR_CHANGE_TIME, MAGENTA, RED);
}

void white_rainbow()
{
  fade(COLOUR_CHANGE_TIME, RED, YELLOW);
  fade(1000, YELLOW, WHITE);
  fade(1000, WHITE, YELLOW);
  fade(COLOUR_CHANGE_TIME, YELLOW, GREEN);
  fade(1000, GREEN, WHITE);
  fade(1000, WHITE, GREEN);
  fade(COLOUR_CHANGE_TIME, GREEN, CYAN);
  fade(1000, CYAN, WHITE);
  fade(1000, WHITE, CYAN);
  fade(COLOUR_CHANGE_TIME, CYAN, BLUE);
  fade(1000, BLUE, WHITE);
  fade(1000, WHITE, BLUE);
  fade(COLOUR_CHANGE_TIME, BLUE, MAGENTA);
  fade(1000, MAGENTA, WHITE);
  fade(1000, WHITE, MAGENTA);
  fade(COLOUR_CHANGE_TIME, MAGENTA, RED);
  fade(1000, RED, WHITE);
  fade(1000, WHITE, RED);
}

void all_off()
{  
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  delay(5000);
}

void dawn_starts()
{
  fade(60000, BLACK, RED);
  fade(60000, RED, YELLOW);
  fade(60000, YELLOW, CYAN);
  fade(60000, CYAN, WHITE);
  fade(1000, WHITE, BLACK);
}

void willow()
{
  fade(8000, GREEN, YELLOW);
  fade(8000, YELLOW, GREEN);
  //fade(5000, CYAN, GREEN);
}

void darkness_falls()
{
  fade(10000, BLACK, RED);
  fade(10000, RED, BLUE);
  fade(10000, BLUE, RED);
  fade(10000, RED, BLACK);
}


int mode = 0;
void loop() {

  mode = time_mode();
  show_time();
  
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
