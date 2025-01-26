/****************************************************************************************************************************
  ConfigOnStartup.ino
  For RP2040W boards

  WiFiManager_RP2040W is a library for RP2040W

  Modified from
  1) Tzapu        https://github.com/tzapu/WiFiManager
  2) Ken Taylor   https://github.com/kentaylor
  3) Khoi Hoang   https://github.com/khoih-prog/ESP_WiFiManager

  Built by Khoi Hoang https://github.com/khoih-prog/WiFiManager_RP2040W
  Licensed under MIT license
 *****************************************************************************************************************************/
/****************************************************************************************************************************
   This example will open a configuration portal for 60 seconds when first powered up if the boards has stored WiFi Credentials.
   Otherwise, it'll stay indefinitely in ConfigPortal until getting WiFi Credentials and connecting to WiFi

   WIO_ConfigOnStartup is a a bettter example for most situations but this has the advantage
   that no pins or buttons are required on the WIO_Terminal device at the cost of delaying
   the user sketch for the configurable period (CONFIG_PORTAL_TIMEOUT_SEC) that the configuration portal is open.

   Also in this example a password is required to connect to the configuration portal
   network. This is inconvenient but means that only those who know the password or those
   already connected to the target WiFi network can access the configuration portal and
   the WiFi network credentials will be sent from the browser over an encrypted connection and
   can not be read by observers.
 *****************************************************************************************************************************/

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _WIFIMGR_LOGLEVEL_    4

#include <WiFi.h>

#include <WiFiMulti_Generic.h>
WiFiMulti_Generic wifiMulti;

#include <LittleFS.h>

#define  CONFIG_FILENAME        ("/wifi_cred.dat")

// SSID and PW for Config Portal
// SSID and PW for Config Portal
String ssid = "RP2040W_ConfigPortal";
const char* password = "RP2040W_Pass";

#define CONFIG_PORTAL_TIMEOUT_SEC       120

// SSID and PW for your Router
String Router_SSID;
String Router_Pass;

#define LED_ON      HIGH
#define LED_OFF     LOW

#define MIN_AP_PASSWORD_SIZE    8

#define SSID_MAX_LEN            32
//From v1.0.10, WPA2 passwords can be up to 63 characters long.
#define PASS_MAX_LEN            64

typedef struct
{
  char wifi_ssid[SSID_MAX_LEN];
  char wifi_pw  [PASS_MAX_LEN];
}  WiFi_Credentials;

typedef struct
{
  String wifi_ssid;
  String wifi_pw;
}  WiFi_Credentials_String;

#define NUM_WIFI_CREDENTIALS      2

typedef struct
{
  WiFi_Credentials  WiFi_Creds [NUM_WIFI_CREDENTIALS];
} WM_Config;

WM_Config         WM_config;

//////

// Indicates whether ESP has WiFi credentials saved from previous session, or double reset detected
bool initialConfig = false;
//////

// Use false if you don't like to display Available Pages in Information Page of Config Portal
// Comment out or use true to display Available Pages in Information Page of Config Portal
// Must be placed before #include <WiFiManager_RP2040W.h>
#define USE_AVAILABLE_PAGES     false

// New in v1.0.11
#define USING_CORS_FEATURE          true

///////////////////////////////////////////////////

// Use DHCP
#warning Using DHCP IP
IPAddress stationIP   = IPAddress(0, 0, 0, 0);
IPAddress gatewayIP   = IPAddress(192, 168, 2, 1);
IPAddress netMask     = IPAddress(255, 255, 255, 0);

#define USE_CONFIGURABLE_DNS      false   //true

IPAddress dns1IP      = gatewayIP;

///////////////////////////////////////////////////

#define USE_CUSTOM_AP_IP          false

IPAddress APStaticIP  = IPAddress(192, 168, 42, 1);
IPAddress APStaticGW  = IPAddress(192, 168, 42, 1);
IPAddress APStaticSN  = IPAddress(255, 255, 255, 0);

#include <WiFiManager_RP2040W.h>              //https://github.com/khoih-prog/WiFiManager_RP2040W

// Function Prototypes
uint8_t connectMultiWiFi();

void heartBeatPrint()
{
  static int num = 1;

  if (WiFi.status() == WL_CONNECTED)
    Serial.print("H");        // H means connected to WiFi
  else
    Serial.print("F");        // F means not connected to WiFi

  if (num == 80)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(" ");
  }
}

void check_WiFi()
{
  if ( (WiFi.status() != WL_CONNECTED) )
  {
    Serial.println("\nWiFi lost. Call connectMultiWiFi in loop");
    connectMultiWiFi();
  }
}  

void check_status()
{
  static ulong checkstatus_timeout  = 0;
  static ulong checkwifi_timeout    = 0;

  static ulong current_millis;

#define WIFICHECK_INTERVAL    1000L
#define HEARTBEAT_INTERVAL    10000L

  current_millis = millis();
  
  // Check WiFi every WIFICHECK_INTERVAL (1) seconds.
  if ((current_millis > checkwifi_timeout) || (checkwifi_timeout == 0))
  {
    check_WiFi();
    checkwifi_timeout = current_millis + WIFICHECK_INTERVAL;
  }

  // Print hearbeat every HEARTBEAT_INTERVAL (10) seconds.
  if ((current_millis > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    heartBeatPrint();
    checkstatus_timeout = current_millis + HEARTBEAT_INTERVAL;
  }
}

bool loadConfigData()
{
  File file = LittleFS.open(CONFIG_FILENAME, "r");
  LOGERROR(("LoadWiFiCfgFile "));

  memset((void*) &WM_config,       0, sizeof(WM_config));

  // New in v1.4.0
  //memset((void*) &WM_STA_IPconfig, 0, sizeof(WM_STA_IPconfig));
  //////

  if (file)
  {
    file.readBytes((char *) &WM_config,   sizeof(WM_config));
    //file.readBytes((char *) &WM_STA_IPconfig, sizeof(WM_STA_IPconfig));

    file.close();
    LOGERROR(("OK"));
   
    return true;
  }
  else
  {
    LOGERROR(("failed"));

    return false;
  }
}

void saveConfigData()
{
  File file = LittleFS.open(CONFIG_FILENAME, "w");
  LOGERROR(("SaveWiFiCfgFile "));

  if (file)
  {   
    file.write((uint8_t*) &WM_config, sizeof(WM_config));
    //file.write((uint8_t*) &WM_STA_IPconfig, sizeof(WM_STA_IPconfig));

    file.close();
    LOGERROR(("OK"));
  }
  else
  {
    LOGERROR(("failed"));
  }
}


uint8_t connectMultiWiFi()
{
  // This better be 0 to shorten the connect time
  #define WIFI_MULTI_1ST_CONNECT_WAITING_MS       0
  #define WIFI_MULTI_CONNECT_WAITING_MS           100L
  
  uint8_t status;

  LOGERROR(F("ConnectMultiWiFi with :"));
  
  if ( (Router_SSID != "") && (Router_Pass != "") )
  {
    LOGERROR3(F("* LittleFS-stored Router_SSID = "), Router_SSID, F(", Router_Pass = "), Router_Pass );
  }

  for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
  {
    // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
    if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE) )
    {
      LOGERROR3(F("* Additional SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
    }
  }
  
  LOGERROR(F("Connecting MultiWifi..."));

  WiFi.end();

  WiFi.mode(WIFI_STA);

  int i = 0;
  status = wifiMulti.run();
  delay(WIFI_MULTI_1ST_CONNECT_WAITING_MS);

  while ( ( i++ < 10 ) && ( status != WL_CONNECTED ) )
  {
    status = wifiMulti.run();

    if ( status == WL_CONNECTED )
      break;
    else
      delay(WIFI_MULTI_CONNECT_WAITING_MS);
  }

  if ( status == WL_CONNECTED )
  {
    LOGERROR1(F("WiFi connected after time: "), i);
    LOGERROR3(F("SSID:"), WiFi.SSID(), F(",RSSI="), WiFi.RSSI());
    LOGERROR1(F("IP address:"), WiFi.localIP() );
  }
  else
    LOGERROR(F("WiFi not connected"));

  return status;
}

void processDataFromCP(WiFiManager_RP2040W &RP2040W_WiFiManager)
{
  // Stored  for later usage, from v1.1.0, but clear first
  memset(&WM_config, 0, sizeof(WM_config));

  for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
  {
    String tempSSID = RP2040W_WiFiManager.getSSID(i);
    String tempPW   = RP2040W_WiFiManager.getPW(i);

    if (strlen(tempSSID.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1)
      strcpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str());
    else
      strncpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1);

    if (strlen(tempPW.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1)
      strcpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str());
    else
      strncpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1);  
  }
}

void setup()
{
  // put your setup code here, to run once:
  // initialize the LED digital pin as an output.
  pinMode(PIN_LED, OUTPUT);
  
  Serial.begin(115200);
  while (!Serial && millis() < 5000);
  
  Serial.print("\nStarting ConfigOnStartup on "); Serial.println(BOARD_NAME);
  Serial.println(WIFI_MANAGER_RP2040W_VERSION);

  digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

  unsigned long startedAt = millis();

  bool beginOK = LittleFS.begin();

  if (!beginOK)
  {
    Serial.println("\nLittleFS error");
  }

  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager_RP2040W RP2040W_WiFiManager;

  RP2040W_WiFiManager.setDebugOutput(true);

  // Use only to erase stored WiFi Credentials
  //RP2040W_WiFiManager.resetSettings();

  RP2040W_WiFiManager.setMinimumSignalQuality(-1);

#if USING_CORS_FEATURE
  RP2040W_WiFiManager.setCORSHeader("Your Access-Control-Allow-Origin");
#endif

  // If no stored WiFi data, load from saved EEPROM 
  if ( (Router_SSID == "") || (Router_Pass == "") )
  {
    // Load stored data and verify CheckSum
    if (loadConfigData())
    {
      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
      {
        // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
        if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE) )
        {
          LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
          wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);

          Router_SSID = String(WM_config.WiFi_Creds[i].wifi_ssid);
          Router_Pass = String(WM_config.WiFi_Creds[i].wifi_pw);
        }
      }
    }
  }
  
  //Remove this line if you do not want to see WiFi password printed
  LOGERROR3("Stored: SSID =", Router_SSID, ", Pass =", Router_Pass);

  //Check if there is stored WiFi router/password credentials.
  //If not found, device will remain in configuration mode until switched off via webserver.
  LOGERROR("Opening configuration portal.");

  // Don't permit NULL password
  if ( (Router_SSID != "") && (Router_Pass != "") )
  {
    LOGERROR3(F("* Add SSID = "), Router_SSID, F(", PW = "), Router_Pass);
    wifiMulti.addAP(Router_SSID.c_str(), Router_Pass.c_str());
    
    RP2040W_WiFiManager.setConfigPortalTimeout(CONFIG_PORTAL_TIMEOUT_SEC); //If no access point name has been previously entered disable timeout.
    LOGERROR1("Got stored Credentials. Timeout for Config Portal =", CONFIG_PORTAL_TIMEOUT_SEC);
  }
  else
  {
    LOGERROR("Open Config Portal without Timeout: No stored Credentials.");
    digitalWrite(PIN_LED, LED_ON); // Turn led on as we are in configuration mode.

    initialConfig = true;
  }

  LOGERROR("Starting configuration portal.");
  digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

  // Starts an access point
  if (!RP2040W_WiFiManager.startConfigPortal((const char *) ssid.c_str(), password))
  {
    LOGERROR("Not connected to WiFi but continuing anyway.");
  }
  else
  {
    LOGERROR("WiFi connected...yeey :)");
  }

  // Only clear then save data if CP entered and with new valid Credentials
  // No CP => stored getSSID() = ""
  if ( String(RP2040W_WiFiManager.getSSID(0)) != "" && String(RP2040W_WiFiManager.getSSID(1)) != "" )
  {
    // Stored  for later usage, but clear first
    memset(&WM_config, 0, sizeof(WM_config));
    
    for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
    {
      String tempSSID = RP2040W_WiFiManager.getSSID(i);
      String tempPW   = RP2040W_WiFiManager.getPW(i);
  
      if (strlen(tempSSID.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1)
        strcpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str());
      else
        strncpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1);
  
      if (strlen(tempPW.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1)
        strcpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str());
      else
        strncpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1);  
  
      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE) )
      {
        LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
        wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
      }
    }
  
    saveConfigData();

    initialConfig = true;
  }

  digitalWrite(PIN_LED, LED_OFF); // Turn led off as we are not in configuration mode.

  startedAt = millis();

  if (!initialConfig)
  {
    // Load stored data, the addAP ready for MultiWiFi reconnection
    loadConfigData();

    for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
    {
      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE) )
      {
        LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
        wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
      }
    }

    if ( WiFi.status() != WL_CONNECTED ) 
    {
      LOGERROR("ConnectMultiWiFi in setup");
     
      connectMultiWiFi();
    }
  }

  Serial.print("After waiting ");
  Serial.print((float) (millis() - startedAt) / 1000L);
  Serial.print(" secs more in setup(), connection result is ");

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("connected");
  }
  else
    Serial.println(RP2040W_WiFiManager.getStatus(WiFi.status()));

  Serial.print("Local IP = "); Serial.println(WiFi.localIP());  
}

void loop()
{
  // put your main code here, to run repeatedly
  check_status();
}
