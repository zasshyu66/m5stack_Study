#include <M5Stack.h>

#define LCDHIGH 240
#define LCDWIDTH 320

#define BUTTONA 39
#define BUTTONB 38
#define BUTTONC 37

#define BUTTON_ON 0
#define BUTTON_OFF 1

//TFT_eSPI liblary
TFT_eSPI tft = TFT_eSPI();

//thread config
TaskHandle_t LCDdiplay;
TaskHandle_t timeCalculation;

//timer interrupt variable.
volatile unsigned long usecCount = 0;
hw_timer_t *interrupptTimer = NULL;
portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;

//min,sec,msec,usec display.
int display[4] = {0};

//timer start/stop check variable
int startCheck = 0;

void thread1(void *pvParameters)
{
  //unsigned long preusecCount = 0;

  while (1)
  {
    //count display

    tft.setCursor(0, LCDWIDTH / 3);
    tft.printf(" m: s: ms: us\n");
    tft.printf("%02d:", display[0]);
    tft.printf("%02d:", display[1]);
    tft.printf("%03d:", display[2]);
    tft.printf("%03d\n", display[3]);
    tft.printf("%lu", usecCount);

    vTaskDelay(1);
  }
}

//Timer count
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

  //TFT_eSPI setup
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  //interrupt timer setting
  //timerBegin is count per 100 microsec.
  interrupptTimer = timerBegin(0, 80, true);

  //interrupt method setting
  timerAttachInterrupt(interrupptTimer, &usecTimer, true);

  //interrupt timing setting.
  timerAlarmWrite(interrupptTimer, 5, true);
  timerAlarmDisable(interrupptTimer);
  //timerAlarmEnable(interupptTimer);

  //Core 0 thread
  xTaskCreatePinnedToCore(
      thread1,
      "LCDdiplay",
      8192,
      NULL,
      3,
      &LCDdiplay,
      0);
}

//Core1 thread
void loop()
{
    //Start Button Check
    if (digitalRead(BUTTONA) != BUTTON_OFF && startCheck == 0)
    {
      if (digitalRead(BUTTONA) != BUTTON_OFF)
      {
        delayMicroseconds(100);
        for (;;)
        {
          if (digitalRead(BUTTONA) == BUTTON_OFF)
          break;
        }
        portENTER_CRITICAL(&mutex);
        timerAlarmEnable(interrupptTimer);
        startCheck = 1;
        portEXIT_CRITICAL(&mutex);
      }
    }

    //Stop Button Check
    if (digitalRead(BUTTONA) != BUTTON_OFF && startCheck == 1)
    {
      if (digitalRead(BUTTONA) != BUTTON_OFF)
      {
        delayMicroseconds(100);
        for (;;)
        {
          if (digitalRead(BUTTONA) == BUTTON_OFF)
          break;
        }
        portENTER_CRITICAL(&mutex);
        timerAlarmDisable(interrupptTimer);
        startCheck = 0;
        portEXIT_CRITICAL(&mutex);
      }
    }

    //Count Reset Check
    if (digitalRead(BUTTONB) != BUTTON_OFF && startCheck == 0)
    {
      if (digitalRead(BUTTONB) != BUTTON_OFF)
        usecCount = 0;
    }

    //time calculation
    display[3] = (int)(usecCount % 1000);
    display[2] = (int)((usecCount % 1000000) / 1000);
    display[1] = (int)((usecCount / 1000000) % 60);
    display[0] = (int)((usecCount / 60000000) % 3600);
}