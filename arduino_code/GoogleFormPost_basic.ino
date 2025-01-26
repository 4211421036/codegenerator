/* (c) 2023 John Mueller - MIT License -- see LICENSE file
 *
 * Sample code for GoogleFormPost library, which posts data to
 * a Google Form / Spreadsheet, without using APIs or authentication.
 * 
 * Requires WiFiManager ( https://github.com/tzapu/WiFiManager.git )
 * 
 */

#include <string>
#include <vector>

#include <WiFiManager.h>
#include "GoogleFormPost.h"

#define FORM_ROOT_URL "https://docs.google.com/forms/d/e/1FAIpQLSeVQSjI9kL_eireauQ-bEHTwOZl5foafiqWJD119pK0LVRJmA/"

bool postOnce;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    // WiFiManager
    // Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    // reset saved settings
    //wifiManager.resetSettings();
    
    // fetches ssid and pass from eeprom and tries to connect
    // if it does not connect it starts an access point with the specified name
    // and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect();
    
    // if you get here you have connected to the WiFi
    Serial.println("connected to wifi");

    postOnce = true;
}

void loop() {
    if (postOnce) {
        GoogleFormPost gf;
        gf.setDebugMode(true);
        gf.setFormUrl(String(FORM_ROOT_URL));
        // version without knowing the field IDs
        int i = gf.readFields();
        if (i>0) {
            gf.addData(String("Random:") + String(random(100)));
            gf.addData(String("Random:") + String(random(100)));
            gf.showDebug();
            gf.send();
        }
        // or if we did know them:
        gf.reset();
        gf.setFormUrl(String(FORM_ROOT_URL));
        gf.addData(String("Other random:") + String(random(100)), String("entry.312992878"));
        gf.showDebug();
        gf.send();
        postOnce = false;
    }
}
