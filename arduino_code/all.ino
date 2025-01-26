/*
 * THiNX Example with all features
 *
 * - Set your own WiFi credentials for development purposes.
 * - When THiNX finalizes checkin, update device status over MQTT.
 * - Use the `Push Configuration` function in website's action menu to trigger pushConfigCallback() [limit 512 bytes so far]
 */

#include <Arduino.h>

#define ARDUINO_IDE

#include <THiNXLib.h>

char *apikey = "";
char *owner_id = "";
char *ssid = "THiNX-IoT";
char *pass = "<enter-your-ssid-password>";

THiNX thx;

//
// Example of using injected Environment variables to change WiFi login credentials.
//

void pushConfigCallback (String config) {

  // Set MQTT status (unretained)
  thx.publishStatusUnretained("{ \"status\" : \"push configuration received\"}");

  // Convert incoming JSON string to Object
  DynamicJsonBuffer jsonBuffer(512);
  JsonObject& root = jsonBuffer.parseObject(config.c_str());
  JsonObject& configuration = root["configuration"];

  if ( !configuration.success() ) {
    Serial.println(F("Failed parsing configuration."));
  } else {

    // Parse and apply your Environment vars
    const char *ssid = configuration["THINX_ENV_SSID"];
    const char *pass = configuration["THINX_ENV_PASS"];

    // password may be empty string
    if ((strlen(ssid) > 2) && (strlen(pass) > 0)) {
      WiFi.disconnect();
      WiFi.begin(ssid, pass);
      ssid = ""; pass = "";
      unsigned long timeout = millis() + 20000;
      Serial.println("Attempting WiFi migration...");
      while (WiFi.status() != WL_CONNECTED) {
        if (millis() > timeout) break;
      }
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi migration failed."); // TODO: Notify using publish() to device status channel
      } else {
        Serial.println("WiFi migration successful."); // TODO: Notify using publish() to device status channel
      }
    }
  }
}

void setup() {

  Serial.begin(115200);

#ifdef __DEBUG__
  while (!Serial); // wait for debug console connection
#endif

  // If you need to inject WiFi credentials once...
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  //
  // Static pre-configuration
  //

  THiNX::accessPointName = "THiNX-AP";
  THiNX::accessPointPassword = "PASSWORD";

  //
  // Initialization
  //

  //THiNX::accessPointName = "THiNX-AP";
  //THiNX::accessPointPassword = "<enter-ap-mode-password>";
  THiNX::forceHTTP = true; disable HTTPS to speed-up checkin in development

  thx = THiNX(apikey, owner_id);

  //
  // App and version identification
  //

  // Override versioning with your own app before checkin
  thx.thinx_firmware_version = "ESP8266-THiNX-App-1.0.0";
  thx.thinx_firmware_version_short = "1.0.0";

  //
  // Callbacks
  //

  // Called after library gets connected and registered.
  thx.setFinalizeCallback([]{
    Serial.println("*INO: Finalize callback called.");
  thx.publishStatusUnretained("{ \"status\" : \"Hello, world!\" }"); // set MQTT status
  });

  // Called when new configuration is pushed OTA
  thx.setPushConfigCallback(pushConfigCallback);

  // Callbacks can be defined inline
  thx.setMQTTCallback([](String message) {
    Serial.println(message);
  });

}

/* Loop must call the thx.loop() in order to pickup MQTT messages and advance the state machine. */
void loop()
{
  thx.loop();
}
