/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.
  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port
  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/

#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

//LCD Display define
#define LCDHIGH 240
#define LCDWIDTH 320
#define TEXTSIZE 2

//TFT_eSPI liblary
TFT_eSPI tft = TFT_eSPI(LCDWIDTH,LCDHIGH);

const char *Response_H = "GET /H";
const char *Response_L = "GET /L";

// Set these to your desired credentials.
const char *ssid = "HTTPTest";
const char *password = "11111111";
const IPAddress accsessIP = IPAddress(192,168,10,254);
const IPAddress subnetIP = IPAddress(255,255,255,0);

WiFiServer server(80);

void setup() {

  // initialize the M5Stack object
  M5.begin();

  //LCD Initialize
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(TEXTSIZE);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(0,0);
  tft.print("ssid : ");
  tft.println(ssid);
  tft.print("password : ");
  tft.println(password);

  //serial start
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  
  WiFi.softAP(ssid, password);
  delay(100);
  WiFi.softAPConfig(accsessIP,accsessIP,subnetIP);
  IPAddress myIP = WiFi.softAPIP();
  tft.print("address: ");
  tft.println(myIP);
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  //delay(10);
  server.begin();

  Serial.println("Server started");
}

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith(Response_H)) {
          //LED display "GET /H" 
          tft.setCursor(0, LCDWIDTH / 2);
          tft.println(Response_H);
        }
        if (currentLine.endsWith(Response_L)) {
          //LED display "GET /L" 
          tft.setCursor(0, LCDWIDTH / 2);
          tft.println(Response_L);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
