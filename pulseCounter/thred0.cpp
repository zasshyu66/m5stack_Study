#include "globalvariable.h"

//timer interrupt variable.
hw_timer_t *interrupptTimer = NULL;
portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR usecTimer(void);

//TFT_eSPI liblary
TFT_eSPI tft = TFT_eSPI(LCDWIDTH,LCDHIGH);

//display data set
String stateDisplay = "STOP";
String modeDisplay = "multiplication by 4";
String AinLevel = "HIGH";
String BinLevel = "HIGH";

//Encoder count and input level buffer.
int encoderCount = 0;
int inValue_A[2] = {0};
int inValue_B[2] = {0};
char errorCheck = 0;

//mode select
extern char countMode;
extern char startCheck;
extern char countReset;

//prototype declaration.

void timerISRInitialize();
void LCDInitialize(void);
String startCheckfunction(void);
String countModeCheck(void);
String encoderInputCheck(uint8_t pins);
String errorDisplay(void);
void encoderCountPattern(void);
void pulseCountPattern(void);

//----------------------------------------------------------------
//
//  LCD display functon.
//
//----------------------------------------------------------------
void thread0(void *pvParameters)
{

  portTickType reflashWait;
  const portTickType reflashRate = REFLASHMSEC / portTICK_RATE_MS;

  reflashWait = xTaskGetTickCount();

  timerISRInitialize();
  LCDInitialize();

  for (;;)
  {
    vTaskDelayUntil(&reflashWait, reflashRate);

    //count display
    tft.setCursor(0, LCDWIDTH / 8);
    tft.println();
    tft.println(startCheckfunction());
    tft.println();
    tft.println(countModeCheck());
    tft.println();
    tft.printf("COUNT  :%8d\n\n",encoderCount);
    tft.println("A-IN   : " + encoderInputCheck(AXIS_A_PIN));
    tft.println("B-IN   : " + encoderInputCheck(AXIS_B_PIN));
    tft.println(errorDisplay());

    if (countReset != 0)
    {
      countReset = 0;
      encoderCount = 0;
    }
    
    
    vTaskDelay(1);
  }
}

//----------------------------------------------------------------
//
//	timer interrrupt initialize.
//  timer interrupt interval is 2 usec.
//
//----------------------------------------------------------------
void timerISRInitialize(void)
{
  //interrupt timer setting
  //timerBegin is count per 100 microsec.
  interrupptTimer = timerBegin(0, 80, true);

  //interrupt method setting
  timerAttachInterrupt(interrupptTimer, &usecTimer, true);

  //interrupt timing setting.
  timerAlarmWrite(interrupptTimer, SAMPLING, true);
  timerAlarmDisable(interrupptTimer);
  //timerAlarmEnable(interrupptTimer);
}

//----------------------------------------------------------------
//
//	LCD display startup.
//
//----------------------------------------------------------------
void LCDInitialize(void)
{
  //TFT_eSPI setup
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(TEXTSIZE);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

//----------------------------------------------------------------
//
//	Count start check function.
//
//----------------------------------------------------------------
String startCheckfunction(void)
{
  String returnString = "STATUS : ";

  if (startCheck != 0)
  {
    if (!timerAlarmEnabled(interrupptTimer))
    {
      inValue_A[1] = digitalRead(AXIS_A_PIN);
      inValue_B[1] = digitalRead(AXIS_B_PIN);
      delayMicroseconds(5);
      timerAlarmEnable(interrupptTimer);
    }

    returnString += "COUNT START";
  }
  else
  {
    if (timerAlarmEnabled(interrupptTimer))
    {
      timerAlarmDisable(interrupptTimer);
    }

    returnString += "COUNT STOP ";
  }

  return returnString;
}

//----------------------------------------------------------------
//
//	Count start check function.
//
//----------------------------------------------------------------
String errorDisplay(void)
{
  String returnString = "        ";

  if (errorCheck != 0)
  {
    returnString = "ERROR!!!";
    if(countReset != 0)
    {
      errorCheck = 0;
    }
  }

  return returnString;
}

//----------------------------------------------------------------
//
//	Count mode check function.
//
//----------------------------------------------------------------
String countModeCheck(void)
{
  String returnString = "";

  switch (countMode)
  {
  case MULTIPLICATION1:
  returnString += "multiplication by 1";
    break;
    case MULTIPLICATION2:
  returnString += "multiplication by 2";
    break;
      case MULTIPLICATION4:
  returnString += "multiplication by 4";
    break;
      case TWOPULSE:
  returnString += "A -> +pls, B-> -pls";
    break;
          case DIRPULSE:
  returnString += "A -> DIR, B-> pls  ";
    break;

  default:
    break;
  }

  return returnString;
}

//----------------------------------------------------------------
//
//	Encoder input level check.
//
//----------------------------------------------------------------
String encoderInputCheck(uint8_t pins)
{
  String returnString = "";

  if(digitalRead(pins) != LOW)
  {
    returnString = "HIGH";
  }
  else
  {
    returnString = "LOW";
  }

  return returnString;
}

//----------------------------------------------------------------
//
//	Encoder sampling function
//  sampling time is 2 usec.
//
//----------------------------------------------------------------
void IRAM_ATTR usecTimer(void)
{
  portENTER_CRITICAL_ISR(&mutex);
  inValue_A[0] = digitalRead(AXIS_A_PIN);
  inValue_B[0] = digitalRead(AXIS_B_PIN);

  if((countMode == MULTIPLICATION1) || (countMode == MULTIPLICATION2) || (countMode == MULTIPLICATION4))
  {
    encoderCountPattern();
  }
  else
  {
    pulseCountPattern();
  }

  inValue_A[1] = inValue_A[0];
  inValue_B[1] = inValue_B[0];

  portEXIT_CRITICAL_ISR(&mutex);
}

//----------------------------------------------------------------
//
//	Encoder count mode function.
//  Count mode -> Multiplication by 1/2/4.
//
//----------------------------------------------------------------
void encoderCountPattern(void)
{
  //pattern 1
  if(inValue_A[1] == LOW && inValue_B[1] == LOW)
  {
    if(inValue_A[0] == HIGH && inValue_B[0] == LOW)
    {
      encoderCount++;
    }
    else if (inValue_A[0] == LOW && inValue_B[0] == HIGH)
    {
      if(countMode == MULTIPLICATION4)
      {
        encoderCount--;
      }
    }
    else if(inValue_A[0] == HIGH && inValue_B[0] == HIGH)
    {
      errorCheck = 1;      
    }
  }
  
  //pattern 2
  if(inValue_A[1] == HIGH && inValue_B[1] == LOW)
  {
    if(inValue_A[0] == HIGH && inValue_B[0] == HIGH)
    {
      if(countMode == MULTIPLICATION4)
      {
        encoderCount++;
      }
    }
    else if (inValue_A[0] == LOW && inValue_B[0] == LOW)
    {
      encoderCount--;
    }
    else if(inValue_A[0] == LOW && inValue_B[0] == HIGH)
    {
      errorCheck = 1;      
    }
  }

  //pattern 3
  if(inValue_A[1] == HIGH && inValue_B[1] == HIGH)
  {
    if(inValue_A[0] == LOW && inValue_B[0] == HIGH)
    {
      if(countMode != MULTIPLICATION1)
      {
        encoderCount++;
      }
    }
    else if (inValue_A[0] == HIGH && inValue_B[0] == LOW)
    {
      if(countMode == MULTIPLICATION4)
      {
        encoderCount--;
      }
    }
    else if(inValue_A[0] == LOW && inValue_B[0] == LOW)
    {
      errorCheck = 1;      
    }
  }

  //pattern 4
  if(inValue_A[1] == LOW && inValue_B[1] == HIGH)
  {
    if(inValue_A[0] == LOW && inValue_B[0] == LOW)
    {
      if(countMode == MULTIPLICATION4)
      {
        encoderCount++;
      }
    }
    else if (inValue_A[0] == HIGH && inValue_B[0] == HIGH)
    {
      if(countMode != MULTIPLICATION1)
      {
        encoderCount--;
      }
    }
    else if(inValue_A[0] == HIGH && inValue_B[0] == LOW)
    {
      errorCheck = 1;      
    }
  }
}

//----------------------------------------------------------------
//
//	Pulse count mode function.
//  Count mode -> Two pulse.
//
//----------------------------------------------------------------
void pulseCountPattern(void)
{
  //Two pulse mode
  if(countMode == TWOPULSE)
  {
    if(inValue_A[1] == HIGH && inValue_B[1] == HIGH)
    {
      if(inValue_A[0] == LOW && inValue_B[0] == HIGH)
      {
        encoderCount++;
      }
      else if (inValue_A[0] == HIGH && inValue_B[0] == LOW)
      {
        encoderCount--;
      }
      else if(inValue_A[0] == LOW && inValue_B[0] == LOW)
      {
        errorCheck = 1;      
      }
    }
  }

  //DIR pulse mode
  if(countMode == DIRPULSE)
  {
    if(inValue_A[1] == HIGH && inValue_B[1] == HIGH)
    {
      if(inValue_A[0] == HIGH && inValue_B[0] == LOW)
      {
        encoderCount++;
      }
      else if(inValue_A[0] == LOW && inValue_B[0] == HIGH)
      {
        errorCheck = 1;      
      }
    }

    if(inValue_A[1] == LOW && inValue_B[1] == HIGH)
    {
      if(inValue_A[0] == LOW && inValue_B[0] == LOW)
      {
        encoderCount--;
      }
      else if(inValue_A[0] == HIGH && inValue_B[0] == LOW)
      {
        errorCheck = 1;      
      }
    }
  }
  
}