// Import required libraries

#include "ESPAsyncWebServer.h"
#include <FS.h>
#include <ESP8266WiFi.h>
#include "led.h"
#include <string.h>
#include "credentials.h"
const char *PARAM_MESSAGE = "message";

enum ledStatus
{
  LED_STATUS_OFF_E,
  LED_STATUS_ON_E
};

// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String ledState;
ledMode ledLoopMode = STATIC_MODE_E;
ledStatus currentLedStatus = LED_STATUS_OFF_E;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String &var)
{
  Serial.println(var);
  if (var == "STATE")
  {
    if (digitalRead(ledPin))
    {
      ledState = "ON";
    }
    else
    {
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  return String();
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Initialize SPIFFS
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Init led strip
  initLed();

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Hello World!");
    response->addHeader("Server", "ESP Async Web Server");
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route for root / web page
  server.on("/dropdown.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/dropdown.html", String(), false, processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to load style.css file
  server.on("/navbar.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/navbar.css", "text/css");
  });

  // Route to load style.css file
  server.on("/jscolor.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/jscolor.js", "text/javascript");
  });

  // Route to load style.css file
  server.on("/menue.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/menue.js", "text/javascript");
  });

  // Route to load style.css file
  server.on("/globalScripts.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/globalScripts.js", "text/javascript");
  });

  // Route to load style.css file
  server.on("/style.css.map", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css.map", "text/css");
  });

  // Route for Contact
  server.on("/contact", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/contact.html", String(), false, processor);
  });
  // Route to load style.css file
  server.on("/contact.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/contact.css", "text/css");
  });

  // Route for RGB picker
  server.on("/rgbpicker", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/rgbpicker.html", String(), false, processor);
  });

  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/rgbpicker", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (currentLedStatus == LED_STATUS_ON_E)
    {
      AsyncWebParameter *p = request->getParam(0);
      ledLoopMode = STATIC_MODE_E;
      Serial.print(ledLoopMode);
      setStaticColor(p->value().toInt());
    }
    request->send(SPIFFS, "/rgbpicker.html", String(), false, processor);
  });

  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/ledstatus", HTTP_POST, [](AsyncWebServerRequest *request) {
    AsyncWebParameter *p = request->getParam(0);
    if (p->name() == "status")
    {
      AsyncWebServerResponse *response = request->beginResponse(200);
      response->addHeader("led-status", currentLedStatus ? "on" : "off");
      request->send(response);
    }
  });

  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    int paramsNr = request->params();

    Serial.println(paramsNr);

    for (int i = 0; i < paramsNr; i++)
    {
      AsyncWebParameter *p = request->getParam(i);
      Serial.print("Param name: ");
      Serial.println(p->name());
      Serial.print("Param value: ");
      Serial.println(p->value());
      Serial.println("------");

      /* switch on mode */
      if (p->name() == "status")
      {
        AsyncWebServerResponse *response = request->beginResponse(200);
        response->addHeader("led-status", currentLedStatus ? "on" : "off");
        request->send(response);
      }

      if (p->name() == "ledState")
      {
        if (p->value() == "on")
        {
          ledLoopMode = STATIC_MODE_E;
          currentLedStatus = LED_STATUS_ON_E;
          turnOnLed();
        }
        else if (p->value() == "off")
        {
          ledLoopMode = STATIC_MODE_E;
          currentLedStatus = LED_STATUS_OFF_E;
          turnOffLed();
        }
      }
      else if (p->name() == "brightness")
      {
        setBrightness(p->value().toInt());
      }
    }

    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(ledPin, HIGH);
    ledLoopMode = STATIC_MODE_E;
    turnOnLed();
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to set GPIO to HIGH
  server.on("/ledmode", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (currentLedStatus == LED_STATUS_ON_E)
    {
      AsyncWebParameter *p = request->getParam(0);
      if (p->value() == "rainbow")
        ledLoopMode = RAINBOW_MODE_E;
      if (p->value() == "theater")
        ledLoopMode = THEATHER_MODE_E;
      if (p->value() == "theaterrainbow")
        ledLoopMode = THEATHER_RAINBOW_MODE_E;
      if (p->value() == "colorwipe")
        ledLoopMode = COLOE_WIPE_MODE_E;

      request->send(SPIFFS, "/index.html", String(), false, processor);
    }
  });

  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(ledPin, LOW);
    turnOffLed();
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Start server
  server.begin();
}

void loop()
{
  Serial.println(ledLoopMode);
  switch (ledLoopMode)
  {
  case RAINBOW_MODE_E:
    rainbow(20);
    break;
  case THEATHER_MODE_E:
    theaterChase(0X00FF00, 50); // Green
    theaterChase(0xFF0000, 50); // Red
    theaterChase(0x0000FF, 50); // Blue
    break;
  case THEATHER_RAINBOW_MODE_E:
    theaterChaseRainbow(50);
    break;
  case COLOE_WIPE_MODE_E:
    colorWipe(0xFF0000, 50); // Red
    colorWipe(0X00FF00, 50); // Green
    colorWipe(0x0000FF, 50); // Blue
    break;
  default:
    break;
  }
}