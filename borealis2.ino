#include <stdint.h>
#include <stdlib.h>
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
int wait = 1000 / fps;
int beat = 0;

uint32_t lastBeat = millis();

int32_t hue = 0;
int32_t saturation = 255;
int32_t value = 255;

int hueMode = 0; // 0: mono, 2: rainbow, 3: spectrum, 4: RGB
int saturationMode = 0; // 0: mono
int valueMode = 0; // 0: mono, 1: chase, 2: strobe, 3: kitt

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

int calcHue(int i, float frameRandom[]) {
  switch(hueMode) {
    case 0:
      // mono
      return beat * hueSpeed + hue;
      
      break;
    case 1:
      // rainbow
      return i + beat * hueSpeed + hue;
      
      break;
    case 2:
      // spectrum
      return (i * 256) / 6 + beat * hueSpeed * 10 + hue;
      
      break;
    case 3:
      // RGB
      return (i * 256) / 3 + beat * hueSpeed * 10 + hue;
      
      break;
    case 4:
      //  disco
      return round(beat * 0.1) * round(((1 + hueSpeed) * .5) * 300) + hue;
      
      break;
  }
}

int calcSaturation(int i, float frameRandom[]) {
  switch(saturationMode) {
    case 0:
      // mono
      return saturation;
      
      break;
  }
}

int calcValue(int i, float frameRandom[]) {
  double localValue;
  
  switch(valueMode) {
    case 0:
      // mono
      localValue = 255;
      break;
    case 1:
      // chase
      localValue = 127 + 127 * sin(i + beat * valueSpeed);
      break;
    case 2:
      // strobe
      localValue = round(beat * (1 + valueSpeed) / 2) % 3 ? 0 : 255;
      break;
    case 3:
      // kitt
      localValue = (1.0 - abs(((1.0 + valueSpeed) / 2.0) * NUM_LEDS - i) / 30.0, 0) * 255.0;
      break;
    case 4:
      // lightning
      float wave = sin(.03 * beat);
      localValue = (wave > 0 ? wave : 0) * frameRandom[0] * ((30 - abs(frameRandom[1] * NUM_LEDS - i)) / 30) * 255;
      
      break;
  }
  
  return localValue >= 0 ? localValue : 0;
}

void heart() {
  float frameRandom[] = {((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX))};
  
  for(int i = 0; i < NUM_LEDS; i++) {
    CHSV hsv = CHSV(calcHue(i, frameRandom), calcSaturation(i, frameRandom), calcValue(i, frameRandom));
    CRGB rgb;
    
    hsv2rgb_spectrum(hsv, rgb);

    leds[i] = rgb;
  }

  FastLED.show();

  beat++;
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

  uint32_t now = millis();

  if(now - lastBeat > wait) {
    lastBeat = now;
    
    heart();
  }
}
