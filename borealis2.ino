#include <stdint.h>
#include <math.h>
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "FS.h"
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

#define DATA_PIN 5
#define NUM_LEDS 150

int fps = 30;
int wait = fps / 1000;
int beat = 0;

int16_t hue = 0;
int16_t saturation = 255;
int16_t value = 255;

int hueMode = 0; // 0: mono, 2: rainbow, 3: spectrum, 4: RGB
int saturationMode = 0; // 0: mono
int valueMode = 0; // 0: mono, 1: chase

double hueSpeed = 0.0;
double saturationSpeed = 0.0;
double valueSpeed = 0.0;


CRGB leds[NUM_LEDS];
void hsv2rgb_spectrum(CRGB& leds);

const char *ssid = "niels-esp8266";
const char *password = "thereisnospoon";

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void handleNotFound() {
  server.send(404, "text/html", "<h1>404 - Not Found</h1>");
}

void socketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    StaticJsonBuffer<200> jsonBuffer;
    
    switch(type) {
        case WStype_DISCONNECTED: {
            Serial.printf("[%u] Disconnected!\n", num);
            
            break;
        }
        case WStype_CONNECTED: {
          {
            IPAddress ip = webSocket.remoteIP(num);
            
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
          }

          JsonObject& init = jsonBuffer.createObject();

          init["hue"] = hue * 360 / 255;
          init["hueMode"] = hueMode;
          init["hueSpeed"] =  hueSpeed;
          
          init["saturation"] = saturation / 255.0;
          init["saturationMode"] = saturationMode;
          init["saturationSpeed"] =  saturationSpeed;
          
          init["value"] = value / 255.0;
          init["valueMode"] = valueMode;
          init["valueSpeed"] =  valueSpeed;

          char buffer[200];

          init.printTo(buffer, sizeof(buffer));

          webSocket.broadcastTXT(buffer);
          
          break;
        }
        case WStype_TEXT: {
          JsonObject& root = jsonBuffer.parseObject(payload);

          if(!root.success()) {
            Serial.println("JSON parse failed");

            return;
          }

          if(root.containsKey("hueMode")) {
            hueMode = root["hueMode"];
            
            Serial.print("Updated hue mode to ");
            Serial.println(hueMode);
          }

          if(root.containsKey("hueSpeed")) {
            hueSpeed = root["hueSpeed"];
            
            Serial.print("Updated hue speed to ");
            Serial.println(hueSpeed);
          }

          if(root.containsKey("hue")) {
            hue = (uint32_t) root["hue"] * 255 / 360;
            
            Serial.print("Updated hue to ");
            Serial.println(hue);
          }

          if(root.containsKey("saturationMode")) {
            saturationMode = root["saturationMode"];
            
            Serial.print("Updated saturation mode to ");
            Serial.println(saturationMode);
          }

          if(root.containsKey("saturationSpeed")) {
            saturationSpeed = root["saturationSpeed"];
            
            Serial.print("Updated saturation speed to ");
            Serial.println(saturationSpeed);
          }

          if(root.containsKey("saturation")) {
            saturation = (double) root["saturation"] * 255;
            
            Serial.print("Updated saturation to ");
            Serial.println(saturation);
          }

          if(root.containsKey("valueMode")) {
            valueMode = (int) root["valueMode"];
            
            Serial.print("Updated value mode to ");
            Serial.println(valueMode);
          }

          if(root.containsKey("valueSpeed")) {
            valueSpeed = root["valueSpeed"];
            
            Serial.print("Updated value speed to ");
            Serial.println(valueSpeed);
          }

          if(root.containsKey("value")) {
            value = (double) root["value"] * 255;
            
            Serial.print("Updated value to ");
            Serial.println(value);
          }
          
          break;
        }
    }
}

int calcHue(int i) {
  switch(hueMode) {
    case 0:
      return beat * hueSpeed + hue;
      
      break;
    case 1:
      return i + beat * hueSpeed + hue;
      
      break;
    case 2:
      return (i * 256) / 6 + beat * hueSpeed + hue;
      
      break;
    case 3:
      return (i * 256) / 3 + beat * hueSpeed + hue;
      
      break;
  }
}

int calcSaturation(int i) {
  switch(saturationMode) {
    case 0:
      return saturation;
      break;
  }
}

int calcValue(int i) {
  uint8_t localValue = 1;
  
  switch(valueMode) {
    case 0:
      localValue =  value;
      break;
    case 1:
      localValue =  (127 + sin(i + beat * (valueSpeed / 4)) * 127) * value / 255;
      break;
    case 2:
      localValue =  (round(beat * (1 + valueSpeed) / 8) % 3 ? 0 : 255) * value /  255;
      break;
  }

  return localValue;
}

void heart(int wait) {
  for(int i = 0; i < NUM_LEDS; i++) {
    CHSV hsv = CHSV(calcHue(i), calcSaturation(i), calcValue(i)); // Hue 255 (red) appears to trigger a FastLED bug, converts to blue
    CRGB rgb;
    
    hsv2rgb_spectrum(hsv, rgb);

    leds[i] = rgb;
  }

  FastLED.show();

  beat++;

  delay(wait);
}

void setup() {
	Serial.begin(115200);
	Serial.println();

  // LEDs
  FastLED.addLeds<WS2811, DATA_PIN>(leds, NUM_LEDS);

  // File system
  bool fs = SPIFFS.begin();
  
  if(fs) { 
    Serial.println("File system mounted");
  }

  // Access Point
	Serial.print("Configuring access point...");
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);

  // HTTP server
  server.serveStatic("/", SPIFFS, "/index.html");
  server.serveStatic("/", SPIFFS, "/");
  
  server.onNotFound(handleNotFound);
	server.begin();
 
	Serial.println("HTTP server started");

  // Socket Server
  webSocket.onEvent(socketEvent);
  
  webSocket.begin();
  Serial.println("Socket server started");
}

void loop() {
	server.handleClient();
  webSocket.loop();
  
  heart(wait);
}
