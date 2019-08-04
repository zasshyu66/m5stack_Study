#include <M5Stack.h>

#define LCDHIGH 240
#define LCDWIDTH 320

#define BUTTONA 39
#define BUTTONB 38
#define BUTTONC 37

#define BUTTON_ON 0
#define BUTTON_OFF 1

//timer interrupt variable.
volatile unsigned long usecCount = 0;
hw_timer_t *interrupptTimer = NULL;
portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;

//min,sec,msec,usec display.
int display[4] = {0};

//timer start/stop check variable
int countStart = 0;
int startCheck = 0;

void IRAM_ATTR usecTimer()
{
  portENTER_CRITICAL_ISR(&mutex);
  usecCount += 5;
  portEXIT_CRITICAL_ISR(&mutex);
}

void setup()
{

  // initialize the M5Stack object
  M5.begin();

  //GPIO setting
  //Button A = GPIO 39
  //Button B = GPIO 38
  //Button C = GPIO 37
  pinMode(BUTTONA, INPUT);
  pinMode(BUTTONB, INPUT);
  pinMode(BUTTONC, INPUT);

  // Lcd display setup
  M5.Lcd.fillScreen(BLACK);
  //M5.Lcd.setCursor(0, LCDWIDTH / 4);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(3);
  //M5.Lcd.printf(" m: s: ms: us\n");
  //M5.Lcd.printf("00:00:000:000\n");

  //interrupt timer setting
  //timerBegin is count per 100 microsec.
  interrupptTimer = timerBegin(0, 80, true);
  //interrupt method setting
  timerAttachInterrupt(interrupptTimer, &usecTimer, true);
  //interrupt timing setting.
  timerAlarmWrite(interrupptTimer, 5, true);
  timerAlarmDisable(interrupptTimer);
  //timerAlarmEnable(interupptTimer);
}

void loop()
{
  // put your main code here, to run repeatedly:

  //Start Button Check
  if (digitalRead(BUTTONA) != BUTTON_OFF && startCheck == 0)
  {
    delay(1);
    if (digitalRead(BUTTONA) != BUTTON_OFF)
      countStart = 1;
    for (;;)
      if (digitalRead(BUTTONA) == BUTTON_OFF)
        break;
    delay(1);
  }

  //Stop Button Check
  if (digitalRead(BUTTONA) != BUTTON_OFF && startCheck == 1)
  {
    delay(1);
    if (digitalRead(BUTTONA) != BUTTON_OFF)
      countStart = 0;
    for (;;)
      if (digitalRead(BUTTONA) == BUTTON_OFF)
        break;
    delay(1);
  }

  //Time Count  Start
  if (countStart == 1 && startCheck == 0)
  {
    timerAlarmEnable(interrupptTimer);
    startCheck = 1;
  }

  //Time Count  Stop
  if (countStart == 0 && startCheck == 1)
  {
    timerAlarmDisable(interrupptTimer);
    startCheck = 0;
  }

  //Count Reset Check
  if (digitalRead(BUTTONB) != BUTTON_OFF && startCheck == 0)
  {
    delay(1);
    if (digitalRead(BUTTONB) != BUTTON_OFF)
      usecCount = 0;
    for (;;)
      if (digitalRead(BUTTONB) == BUTTON_OFF)
        break;
    delay(1);
  }

  //time calculation
  display[3] = (int)(usecCount % 1000);
  display[2] = (int)((usecCount % 1000000) / 1000);
  display[1] = (int)((usecCount / 1000000) % 60);
  display[0] = (int)((usecCount / 60000000) % 3600);

  //count display
  //portENTER_CRITICAL(&mutex);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, LCDWIDTH / 3);
  M5.Lcd.printf(" m: s: ms: us\n");
  M5.Lcd.printf("%02d:",display[0]);
  M5.Lcd.printf("%02d:",display[1]);
  M5.Lcd.printf("%03d:",display[2]);
  M5.Lcd.printf("%03d\n",display[3]);
  M5.Lcd.printf("%lu", usecCount);
  //portEXIT_CRITICAL(&mutex);
}