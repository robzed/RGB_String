void setup() {
    pinMode(9, OUTPUT);    // sets the digital pin 13 as output
    pinMode(10, OUTPUT);    // sets the digital pin 13 as output
    pinMode(11, OUTPUT);    // sets the digital pin 13 as output
}


void set_colour(uint8_t red, uint8_t green, uint8_t blue)
{
//  if(red != 0)
//  {
    analogWrite(11, red);
//  }
//  else
//  {
//    digitalWrite(11, HIGH);
//  }

//  if(green != 0)
//  {
    analogWrite(10, green);
//  }
//  else
//  {
//    digitalWrite(10, HIGH);
//  }

//  if(blue != 0)
//  {
    analogWrite(9, blue);
//  }
//  else
//  {
//    digitalWrite(9, HIGH);
//  }
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

#define RED 255, 0, 0
#define YELLOW 255, 255, 0
#define GREEN 0, 255, 0
#define CYAN 0, 255, 255
#define BLUE 0, 0, 255
#define MAGENTA 255, 0, 255

#define WHITE 255, 255, 255

#define COLOUR_CHANGE_TIME 2000
//#define MID_WHITE 1
void loop() {
  fade(COLOUR_CHANGE_TIME, RED, YELLOW);
#ifdef MID_WHITE
  fade(1000, YELLOW, WHITE);
  fade(1000, WHITE, YELLOW);
#endif
  fade(COLOUR_CHANGE_TIME, YELLOW, GREEN);
#ifdef MID_WHITE
  fade(1000, GREEN, WHITE);
  fade(1000, WHITE, GREEN);
#endif
  fade(COLOUR_CHANGE_TIME, GREEN, CYAN);
#ifdef MID_WHITE
  fade(1000, CYAN, WHITE);
  fade(1000, WHITE, CYAN);
#endif
  fade(COLOUR_CHANGE_TIME, CYAN, BLUE);
#ifdef MID_WHITE
  fade(1000, BLUE, WHITE);
  fade(1000, WHITE, BLUE);
#endif
  fade(COLOUR_CHANGE_TIME, BLUE, MAGENTA);
#ifdef MID_WHITE
  fade(1000, MAGENTA, WHITE);
  fade(1000, WHITE, MAGENTA);
#endif
  fade(COLOUR_CHANGE_TIME, MAGENTA, RED);
#ifdef MID_WHITE
  fade(1000, RED, WHITE);
  fade(1000, WHITE, RED);
#endif

//  fade(1000, 255, 0, 0, 255,255, 0);
//  fade(1000, 0, 0, 0, 0, 255, 0);
  //set_colour(255, 0, 0);
//    digitalWrite(9, HIGH);
//    digitalWrite(10, HIGH);
//   digitalWrite(11, HIGH);
//set_colour(0,0,0);
//  delay(1000);
//set_colour(0,0,128);
//  delay(1000);
//    digitalWrite(9, LOW);
//    digitalWrite(10, LOW);
//    digitalWrite(11, LOW);
  //set_colour(0, 0, 0);
//  delay(3000);
}
