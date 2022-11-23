/**
  @brief Connect device to internet and server update then update fs and firmware OTA by downloading from server
  @param ssid SSID of WiFi this device want to connect
  @param pass WiFi's password this device want to connect
  @param firmwareUrl URL firmware update
  @param fsUrl URL fs update
*/
void firmwareUpdate(const char *ssid, const char *pass, const char *firmwareUrl, const char *fsUrl)
{
  BearSSL::CertStore certStore;
  HTTPClient http;

  // connect WiFi
  delay(200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass, 0, 0, true);
  // //Serial.printf("\n{data:firmwareUpdate,status:connectingToSsid,ssid:%s,pass:%s}", (String)jsonMainStruct.firmwareUpdateSsid, (String)jsonMainStruct.firmwareUpdatePass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.println("connecting WiFi");
  }
  Serial.println("\n{data:connectedToWiFi}");

  // set server connection configuration
  configTime(3 * 3600, 0, "pool.ntp.org");
  const uint8_t fingerprint[20] = {0x02, 0xa7, 0x05, 0x7c, 0xdf, 0x65, 0x3c, 0x5f, 0x92, 0x7c, 0x17, 0xc3, 0x4b, 0x75, 0x0e, 0x8b, 0x6d, 0x03, 0x11, 0xf2}; // init fingerprint
  std::unique_ptr<BearSSL::WiFiClientSecure> client_(new BearSSL::WiFiClientSecure);                                                                        // set bear ssl function

  client_->setFingerprint(fingerprint); // put fingerprint into bearssl
  client_->setInsecure();               // set bear ssl to insecure mode
  http.useHTTP10(true);                 // use http10
  http.setTimeout(30000);

  // firmware URL con not be empty
  if (strcmp(firmwareUrl, "") == 0)
    ESP.restart();

  // update fs if needed
  if (strcmp(fsUrl, "") != 0)
  {
    Serial.println("updating flash");
    t_httpUpdate_return _fs = ESPhttpUpdate.updateFS(*client_, fsUrl, "");
    Serial.println("update flash done");
  }

  // update firmware
  Serial.println("updating firmware");
  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
  HTTPUpdateResult ret = ESPhttpUpdate.update(firmwareUrl, "", fingerprint);
}