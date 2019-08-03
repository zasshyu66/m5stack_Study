#include <M5Stack.h>

#define BUFFER_SIZE 256

#define LCDHIGH 240
#define LCDWIDTH 320

#define BUTTONA 39
#define BUTTONB 38
#define BUTTONC 37

#define BUTTON_ON 0
#define BUTTON_OFF 1

void setup() {

  // initialize the M5Stack object
  M5.begin();

  //serial Setup
  //serial = GPIO 1,3 USB to UART
  //serial1 = GPIO 28,29 connect to SD card reader
  //serial2 = GPIO 16,17 Extern I/O pin

  //serial default setting,8 data bit,No parity,1 stop bit.
  Serial.begin(115200);

  //GPIO setting
  //Button A = GPIO 39
  //Button B = GPIO 38uuuuuuuhijbkjggvvfyvhgvhbjhkgjhjh
  //Button C = GPIO 37
  pinMode(BUTTONA,INPUT);
  pinMode(BUTTONB,INPUT);
  pinMode(BUTTONC,INPUT);

  // Lcd display setup
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0,LCDWIDTH / 3);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("Start M5stack!!");
  delay(5000);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0,0);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
}

void errorEnd(){
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0,0);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(5);
  M5.Lcd.printf("Buffer over flow!");
  for(;;);
}

void loop() {
  char message_a[] = "Button A push!\n";
  char message_b[] = "Button B push!\n";
  char message_c[] = "Button C push!\n";
  unsigned char readBuffer[BUFFER_SIZE];
  int serialLength;

  serialLength = Serial.available();
  if(serialLength > BUFFER_SIZE -1)errorEnd();
  if(serialLength > 0){
    Serial.readBytes(readBuffer,serialLength);
    readBuffer[serialLength] = '\0';
    M5.Lcd.printf("%s",readBuffer);
    Serial.printf("%s",readBuffer);
  }

  if(digitalRead(BUTTONA)!=BUTTON_OFF)Serial.printf(message_a);
  if(digitalRead(BUTTONB)!=BUTTON_OFF)Serial.printf(message_b);
  if(digitalRead(BUTTONC)!=BUTTON_OFF)Serial.printf(message_c);

}