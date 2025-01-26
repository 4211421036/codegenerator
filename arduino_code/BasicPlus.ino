/*
DyanHTML Basic Plus OTA test.

A lot of my projects use a MQTT connected over wifi.
So here is a pretty basic example.

On first run, the example looks for a couple magic numbers in the EEPROM. If those don't exist, it assumes first run and goes straight to configuration mode. If you make a mistake and it can't connect, turn off esp, hold gpio4 low, then turn on. It will then boot into config mode.

While in Config mode, the built_in led will fade on and off.

In run mode, it will post an incremental number to the mqtt topic.

All code was tested on ESP 12-F modules as well as a nodemcu.

Example:
Added bool USBpower = false; > Pulls from a HTML Checkbox.
I have a sensor that can run on battery(CR123a) or USB Power by a Micro USB.
My battery powered devices go into deep hybernation.
The USB powered mode is for use with power hungry sensors where battery use is not feasible.

Added OTA programming.
Check my tasks.json file on how to use that within Visual Studio Code.
    F1 > RUN TASK > OTA UPload *Make sure you have the correct IP address and admin password, etc.

*/

#include <ESP_EEPROM.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

#include <dynaHTML.h>

#define SSID_MAX_LEN 32
#define PASS_MAX_LEN 64
#define ADA_URL_LEN 20
#define ADA_FEED_LEN 40
#define ADA_ID_LEN 30
#define ADA_KEY_LEN 33

const char *ssidAP = "ESP_DynaHTML";
const char *passwordAP = "12345678";
#define OTA_PASSDW "admin"
bool USBpower = false;

struct configData
{
    char wifi_ssid[SSID_MAX_LEN];
    char wifi_pw[PASS_MAX_LEN];
    char mqtt_server[ADA_URL_LEN];
    char sensorstatus[ADA_FEED_LEN];
    char sensorname[ADA_KEY_LEN];
    char mqtt_id[ADA_ID_LEN];
    char mqtt_key[ADA_KEY_LEN];
    char usb_power[2]; // bool is a byte, is a char[1] +null termination
};
configData MyconfigData = {};
struct apData
{
    uint32_t crc32;   // 4 bytes
    uint8_t channel;  // 1 byte,   5 in total
    uint8_t bssid[6]; // 6 bytes, 11 in total
};
apData MyAPdata = {};
bool apValid = false;

struct cStart
{
    uint8_t One;
    uint8_t Two;
};
cStart ColdStart = {};
bool hasConfig = false;

#define eepromstart 0
#define EEPROM_SIZE (sizeof(configData) + sizeof(apData) + sizeof(cStart))
#define eepromapstart sizeof(configData)
#define eepromcoldstart (sizeof(configData) + sizeof(apData))

ADC_MODE(ADC_VCC); // vcc read-mode
typedef enum
{
    CONFIG = 0,
    RUN = 1,
} ConfigMode;
uint8_t configmode = RUN;

int GPIO4 = 4;
int brightness = 0; // how bright the LED is
int fadeAmount = 5; // how many points to fade the LED by
unsigned long lastMsg = 0;
unsigned long mslightdim = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

AsyncWebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
dynaHTML dHTML;

//                     HTML ID, LABEL,  Your struct char data, TEXT/CHECK, GROUP
MenuItem mallItem[] = {{"wiid", "SSID", MyconfigData.wifi_ssid, e_INPUT, 0},
                       {"wipw", "Password", MyconfigData.wifi_pw, e_INPUT, 0},
                       {"msrv", "MQTT Server", MyconfigData.mqtt_server, e_INPUT, 1},
                       {"ss", "Sensor Publish", MyconfigData.sensorstatus, e_INPUT, 2},
                       {"sn", "Sensor Name", MyconfigData.sensorname, e_INPUT, 2},
                       {"mqi", "MQTT ID", MyconfigData.mqtt_id, e_INPUT, 1},
                       {"mqp", "MQTT PW", MyconfigData.mqtt_key, e_INPUT, 1},
                       {"powr", "USB Power", MyconfigData.usb_power, e_CHECK, 2}};
uint16_t NUM_MENU_ITEMS = sizeof(mallItem) / sizeof(MenuItem);
// Need to save the eeprom data and restart the esp
void dynaCallback()
{
    Serial.println("YOUR CALLBACK WORKED!!!!!!!!!!");
    Serial.println(MyconfigData.wifi_ssid);
    Serial.println(MyconfigData.wifi_pw);
    Serial.println(MyconfigData.mqtt_server);
    Serial.println(MyconfigData.sensorname);
    Serial.println(MyconfigData.sensorstatus);
    Serial.println(MyconfigData.mqtt_id);
    Serial.println(MyconfigData.mqtt_key);
    Serial.println(MyconfigData.usb_power);
    saveconfigtoEE(MyconfigData);
    // if we are updating data, force a refresh of the SSID
    MyAPdata.crc32 = 8675309; // If your CRC based on your bssid and channel happen to be Jennys number, CALL HER!
    saveAPEE(MyAPdata);
    delay(1000);
    ESP.restart();
}

void OTAinit()
{
    // Port defaults to 8266
    ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("DOOR-FRONT-ESP");
    ArduinoOTA.setHostname(MyconfigData.sensorname);
    // if (DEBUG == false) {
    //  Comment to: No authentication by default
    ArduinoOTA.setPassword(OTA_PASSDW);
    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
    // }

    ArduinoOTA.onStart([]()
                       {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type); });
    ArduinoOTA.onEnd([]()
                     { Serial.println("\nEnd"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          { Serial.printf("Progress: %u%%\r\n", (progress / (total / 100))); });
    ArduinoOTA.onError([](ota_error_t error)
                       {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
    ArduinoOTA.begin();
    Serial.println("Ready");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

uint32_t calculateCRC32(const uint8_t *data, size_t length)
{
    uint32_t crc = 0xffffffff;
    while (length--)
    {
        uint8_t c = *data++;
        for (uint32_t i = 0x80; i > 0; i >>= 1)
        {
            bool bit = crc & 0x80000000;
            if (c & i)
            {
                bit = !bit;
            }

            crc <<= 1;
            if (bit)
            {
                crc ^= 0x04c11db7;
            }
        }
    }

    return crc;
}

configData getConfigData()
{
    configData customData;
    EEPROM.get(eepromstart, customData);
    return customData;
}

void saveconfigtoEE(configData MyconfigData)
{
    EEPROM.put(eepromstart, MyconfigData);
    JumpStart();
    boolean ok2 = EEPROM.commit();
    Serial.println(ok2);
}

apData getAPData()
{
    apData customAPData;
    EEPROM.get(eepromapstart, customAPData);
    uint32_t crc = calculateCRC32(((uint8_t *)&customAPData) + 4, sizeof(customAPData) - 4);
    if (crc == customAPData.crc32)
    {
        apValid = true;
    }
    return customAPData;
}

void saveAPEE(apData MyAPData)
{
    EEPROM.put(eepromapstart, MyAPData);
    boolean ok2 = EEPROM.commit();
}

void chkColdStart()
{
    EEPROM.get(eepromcoldstart, ColdStart);
    if (ColdStart.One == 0x45 && ColdStart.Two == 0x72)
    {
        hasConfig = true;
    }
}

void JumpStart()
{
    ColdStart.One = 0x45;
    ColdStart.Two = 0x72;
    EEPROM.put(eepromcoldstart, ColdStart);
}

void setup_wifi()
{
    // We start by connecting to a WiFi network
    WiFi.persistent(true);
    WiFi.mode(WIFI_STA);
    WiFi.forceSleepWake();
    // delay(200);
    // WiFi.config(staticIP, subnet, gateway, dns);
    // WiFi.begin(MyconfigData.wifi_ssid, MyconfigData.wifi_pw);
    if (apValid)
    {
        Serial.println("Using BSSID data");
        // The BSSID data was good, make a quick connection
        WiFi.begin(MyconfigData.wifi_ssid, MyconfigData.wifi_pw, MyAPdata.channel, MyAPdata.bssid, true);
    }
    else
    {
        Serial.println("Not using BSSID data");
        // The BSSID data was not valid, so make a regular connection
        WiFi.begin(MyconfigData.wifi_ssid, MyconfigData.wifi_pw);
    }
    int retries = 0;
    int wifiStatus = WiFi.status();

    while (wifiStatus != WL_CONNECTED)
    {
        retries++;
        if (retries == 100)
        {
            // Quick connect is not working, reset WiFi and try regular connection
            WiFi.disconnect();
            delay(10);
            WiFi.forceSleepBegin();
            delay(10);
            WiFi.forceSleepWake();
            delay(10);
            WiFi.begin(MyconfigData.wifi_ssid, MyconfigData.wifi_pw);
        }
        // Give it 20 Seconds of retrying, otherwise reset the BSSID and try again.
        if (retries == 400)
        {
            WiFi.disconnect(true);
            delay(1);
            WiFi.mode(WIFI_OFF);
            // invalidate the CRC, force a refresh of the WIFI AP without a BSSID/CHANNEL
            MyAPdata.crc32 = 8675309;
            saveAPEE(MyAPdata);
            delay(10);
            ESP.restart();
            return;
        }
        delay(50);
        if (retries % 10 == 1)
            Serial.print(".-.");
        wifiStatus = WiFi.status();
    }

    MyAPdata.channel = WiFi.channel();
    memcpy(MyAPdata.bssid, WiFi.BSSID(), 6); // Copy 6 bytes of BSSID (AP's MAC address)

    MyAPdata.crc32 = calculateCRC32(((uint8_t *)&MyAPdata) + 4, sizeof(MyAPdata) - 4);
    saveAPEE(MyAPdata);

    randomSeed(micros());

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP().toString());
}

void reconnect()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.println("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "DB-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str(), MyconfigData.mqtt_id, MyconfigData.mqtt_key))
        {
            Serial.println("connected");
            Serial.println(MyconfigData.sensorstatus);
        }
        else
        {
            Serial.println("failed, rc=");
            Serial.println(client.state());
            Serial.println(" try again in 1 seconds");
            // Wait 1 seconds before retrying
            delay(1000); // oh man, we should not be blocking.
        }
    }
}

void setup()
{

    pinMode(LED_BUILTIN, OUTPUT); // Initialize the BUILTIN_LED pin as an output
    pinMode(GPIO4, INPUT_PULLUP); // If this pin is LOW, then REPROGRAM by setting up the Access Point.
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);
    delay(10);
    MyconfigData = getConfigData();
    delay(10);
    MyAPdata = getAPData();
    delay(10);
    chkColdStart();
    Serial.println("Config/ap data");
    Serial.println(MyconfigData.wifi_ssid);
    Serial.println(MyAPdata.crc32);

    if (hasConfig == false or digitalRead(GPIO4) == LOW)
    {
        configmode = CONFIG;

        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP(ssidAP, passwordAP);
        IPAddress IP = WiFi.softAPIP();

        dHTML.setCallback(dynaCallback);
        uint16_t tot = dHTML.setMenuItems(mallItem, NUM_MENU_ITEMS);

        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                  { dHTML.handleRequest(request); });

        server.begin();
    }
    if (hasConfig == true and digitalRead(GPIO4) == HIGH)
    {
        configmode = RUN;
        setup_wifi();
        client.setServer(MyconfigData.mqtt_server, 1883);
        // client.setCallback(callback);

        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        OTAinit();
        USBpower = (strcmp(MyconfigData.usb_power, "1") == 0);
        Serial.println(WiFi.localIP().toString());
    }
}

void loop()
{
    if (configmode == RUN)
    {
        if (!client.connected())
        {
            reconnect();
        }
        client.loop();

        ArduinoOTA.handle();
    }
    unsigned long now = millis();

    if (configmode == CONFIG)
    {

        if (now - mslightdim > 30)
        {
            mslightdim = now;
            analogWrite(LED_BUILTIN, brightness);

            // change the brightness for next time through the loop:
            brightness = brightness + fadeAmount;

            // reverse the direction of the fading at the ends of the fade:
            if (brightness <= 0 || brightness >= 255)
            {
                fadeAmount = -fadeAmount;
            }
        }
    }
    if (configmode == RUN)
    {
        if (now - lastMsg > 10000)
        {
            lastMsg = now;
            ++value;
            snprintf(msg, MSG_BUFFER_SIZE, "msg #%ld", value);

            client.publish(MyconfigData.sensorstatus, msg);
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

            if (USBpower == true)
            {
                Serial.println("USB POWER ENABLED");
            }
        }
    }
}