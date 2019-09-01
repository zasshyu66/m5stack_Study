#include "globalvariable.h"

//Use core 0 thread
TaskHandle_t LCDdiplay;
void thread0(void *pvParameters);

//Use core 1 thread
TaskHandle_t buttonCheck;
void thread1(void *pvParameters);

//----------------------------------------------------------------
//
//	Arduino startup function.
//  IO setup and
//
//----------------------------------------------------------------
void setup()
{

  // initialize the M5Stack object
  M5.begin();

  //GPIO setting
  //Button A = GPIO 39
  //Button B = GPIO 38
  //Button C = GPIO 37
  pinMode(BUTTON_A_PIN, INPUT);
  pinMode(BUTTON_B_PIN, INPUT);
  pinMode(BUTTON_C_PIN, INPUT);

  //Encoder INPUT
  //AXIS_A = GPIO 21
  //AXIS_A = GPIO 22
  pinMode(AXIS_A_PIN, INPUT_PULLUP);
  pinMode(AXIS_B_PIN, INPUT_PULLUP);

  //Core 0 thread
  xTaskCreatePinnedToCore(
      thread0,
      "LCDdiplay",
      8192,
      NULL,
      3,
      &LCDdiplay,
      0);

  //Core 1 thread
  xTaskCreatePinnedToCore(
      thread1,
      "buttonCheck",
      8192,
      NULL,
      2,
      &buttonCheck,
      1);
}

//Core1 thread
void loop()
{
}