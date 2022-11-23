
#include <ArduinoOTA.h>
#include <FS.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncWebServer.h>
#include "ESP8266httpUpdate.h"
#include <config/mainStruct.hpp>
#include <service/update.hpp>
// SKETCH BEGIN

AsyncWebServer server(80);
AsyncEventSource events("/events");
baseFirmwareStruct gBaseFirmwareStruct;
IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void setup()
{
  Serial.begin(115200);
  LittleFS.begin();
  Serial.setDebugOutput(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(WiFi.macAddress(), "Digitels123456");

  events.onConnect([](AsyncEventSourceClient *client)
                   { client->send("hello!", NULL, millis(), 1000); });
  server.addHandler(&events);
#ifdef ESP32
  server.addHandler(new SPIFFSEditor(SPIFFS, http_username, http_password));
#elif defined(ESP8266)
  // server.addHandler(new FSEditor_ESP8266_LITTLEFS(http_username, http_password));
#endif
  server.on("/", [](AsyncWebServerRequest *request)
            { 
              AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index.html", "", false);
              request->send(response); });
  server.on("/style.css", [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/style.css", "text/css"); });
  server.on("/script.js", [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/script.js", "text/js"); });
  server.on("/submit", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String message;

              gBaseFirmwareStruct.fsUrl = request->getParam("UrlFS")->value().c_str();
              gBaseFirmwareStruct.firmwareUrl = request->getParam("UrlFirm")->value().c_str();
              gBaseFirmwareStruct.ssid = request->getParam("WiFiSSID")->value().c_str();
              gBaseFirmwareStruct.pass = request->getParam("WiFiPass")->value().c_str();
              Serial.println(String(gBaseFirmwareStruct.firmwareUrl));
              Serial.println(String(gBaseFirmwareStruct.fsUrl));
              Serial.println(String(gBaseFirmwareStruct.ssid));
              Serial.println(String(gBaseFirmwareStruct.pass));
              
              message = " Credentials received by ESP board!!! ";
              request->send(200, "text/plain", message);
              
              if ((strcmp(gBaseFirmwareStruct.fsUrl, "#") != 0) &&\
                  (strcmp(gBaseFirmwareStruct.firmwareUrl, "#") != 0) &&\
                  (strcmp(gBaseFirmwareStruct.ssid, "#") != 0) &&\
                  (strcmp(gBaseFirmwareStruct.pass, "#") != 0))
              {
                gBaseFirmwareStruct.state = true;
              } });
  server.begin();
}

void loop()
{
  if (gBaseFirmwareStruct.state)
  {
    WiFi.softAPdisconnect(true);

    delay(3000);
    firmwareUpdate(gBaseFirmwareStruct.ssid,
                   gBaseFirmwareStruct.pass,
                   gBaseFirmwareStruct.firmwareUrl,
                   gBaseFirmwareStruct.fsUrl);
  }
}
