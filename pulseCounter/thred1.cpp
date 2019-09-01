#include "globalvariable.h"

//mode select
char countMode = 0;
char startCheck = 0;
char countReset = 0;

//prototype declaration.

void buttonChattaringCheck(char &pushCount, char &pushConfirm, char buttonName);

//----------------------------------------------------------------
//
//	Button Check function.
//
//----------------------------------------------------------------
void thread1(void *pvParameters)
{
  portTickType checkWait;
  const portTickType checkRate = CHECKMSEC / portTICK_RATE_MS;

  checkWait = xTaskGetTickCount();

  char lastPushBuffer[BUTTONALL] = {0};
  char confirmPush[BUTTONALL] = {0};

  for (;;)
  {
    vTaskDelayUntil(&checkWait, checkRate);

    if (digitalRead(BUTTON_A_PIN) != BUTTON_OFF)
    {
      buttonChattaringCheck(
          lastPushBuffer[BUTTON_A],
          confirmPush[BUTTON_A],
          BUTTON_A);
    }
    else
    {
      lastPushBuffer[BUTTON_A] = 0;
      confirmPush[BUTTON_A] = 0;
    }

    if (digitalRead(BUTTON_B_PIN) != BUTTON_OFF && startCheck == 0)
    {
      buttonChattaringCheck(
          lastPushBuffer[BUTTON_B],
          confirmPush[BUTTON_B],
          BUTTON_B);
    }
    else
    {
      lastPushBuffer[BUTTON_B] = 0;
      confirmPush[BUTTON_B] = 0;
    }

    if (digitalRead(BUTTON_C_PIN) != BUTTON_OFF)
    {
      buttonChattaringCheck(
          lastPushBuffer[BUTTON_C],
          confirmPush[BUTTON_C],
          BUTTON_C);
    }
    else
    {
      lastPushBuffer[BUTTON_C] = 0;
      confirmPush[BUTTON_C] = 0;
    }
    delay(1);
  }
}

//----------------------------------------------------------------
//
//	Button chattaring check.
//  Check time is 150 msec.
//
//----------------------------------------------------------------
void buttonChattaringCheck(char &pushCount, char &pushConfirm, char buttonName)
{
  pushCount++;
  if (pushConfirm == 0 && pushCount >= CHECKCOUNT)
  {
    pushConfirm = 1;
    pushCount = CHECKCOUNT;

    switch (buttonName)
    {
    case BUTTON_A:

      if (startCheck != 0)
      {
        startCheck = 0;
      }
      else
      {
        startCheck = 1;
      }

      break;

    case BUTTON_B:

      if (countMode > DIRPULSE)
      {
        countMode = 0;
      }
      else
      {
        countMode++;
      }

      break;

    case BUTTON_C:
      countReset = 1;
      break;

    default:
      break;
    }

  }
}