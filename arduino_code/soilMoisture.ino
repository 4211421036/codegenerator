/*
  t6iot.ino - 
  Created by mathieu@internetcollaboratif.info <Mathieu Lory>.
  Sample file to connect t6 api
  
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/

#include <t6iot.h>
#include "settings.h"

String responseA; // for authentication
String responseD; // for datapoints
String responseDT; // for datatypes
String responseU; // for units
String responseS; // for status
String responseIndex; // for index
t6iot t6Client;

struct sAverage {
  int32_t blockSum;
  uint16_t numSamples;
};

struct sAverage sampleAve;
int16_t sensorTValue = 0;

void setup() {
  Serial.begin(115200);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  
  t6Client.begin(httpHost, httpPort, userAgent, timeout);

  // This Authentication method should not be used for an Object
  t6Client.authenticate(t6Username, t6Password, &responseA);
  
  // This Authentication method should be used to get a JWT from a Key/Secret
  //t6Client.authenticateKS(t6Key, t6Secret, &responseA);
    handleAuthenticateResponse();
}


/*
* set Arduino to sleep mode
*/
void pleaseGoToBed() {
  Serial.println();
  Serial.println();
  Serial.println("Sleeping in few milliseconds...");
  delay(500);
  ESP.deepSleep(SLEEP_DELAY_IN_SECONDS * 1000000, WAKE_RF_DEFAULT);
  delay(1500);
}
  
/*
* Use JWT token from Authenticate
*/
void handleAuthenticateResponse() {
  const int A_BUFFER_SIZE = JSON_OBJECT_SIZE(2);
  DynamicJsonBuffer A_jsonRequestBuffer(A_BUFFER_SIZE);
  JsonObject& authenticate = A_jsonRequestBuffer.parseObject(responseA);
  if (!authenticate.success()) {
    Serial.println("Failure on parsing json.");
    Serial.println(responseA);
  } else {
    const char* Aerror = authenticate["error"];
    const char* Atoken = authenticate["token"];
    const char* Astatus = authenticate["status"];
    const char* Arefresh_token = authenticate["refresh_token"];
    const char* ArefreshTokenExp = authenticate["refreshTokenExp"];
    if ( Aerror ) {
      Serial.println("Failure on:");
      Serial.println(responseA);
    }
    Serial.println();
    Serial.print("\tToken: ");
    Serial.println( Atoken );
    Serial.print("\tStatus: ");
    Serial.println( Astatus );
    Serial.print("\tRefresh Token: ");
    Serial.println( Arefresh_token );
    Serial.print("\tRefresh Token Exp: ");
    Serial.println( ArefreshTokenExp );
    Serial.println();
  }
} // handleAuthenticateResponse
  
/*
* Retrieve and use t6 api Status
*/
void handlStatusResponse() {
  const int S_BUFFER_SIZE = JSON_OBJECT_SIZE(2);
  DynamicJsonBuffer S_jsonRequestBuffer(S_BUFFER_SIZE);
  JsonObject& status = S_jsonRequestBuffer.parseObject(responseS);
  if (!status.success()) {
    Serial.println("Failure on parsing json.");
    Serial.println(responseS);
  } else {
    const char* Serror = status["error"];
    const char* Sstatus = status["status"];
    const char* Sversion = status["version"];
    if ( Serror ) {
      Serial.println("Failure on:");
      Serial.println(responseS);
    }
    Serial.println();
    Serial.print("\tStatus: ");
    Serial.println( Sstatus );
    Serial.print("\tVersion: ");
    Serial.println( Sversion );
    Serial.println();
  }
} // handlStatusResponse

/*
* Retrieve and use t6 Datatypes
*/
void handleDatatypesResponse() {
  const int DT_BUFFER_SIZE = JSON_OBJECT_SIZE(2);
  DynamicJsonBuffer DT_jsonRequestBuffer(DT_BUFFER_SIZE);
  JsonObject& datatypes = DT_jsonRequestBuffer.parseObject(responseDT);
  if (!datatypes.success()) {
    Serial.println("Failure on parsing json.");
    Serial.println(responseDT);
  } else {
    const char* DTerror = datatypes["error"];
    const char* DTstatus = datatypes["status"];
    const char* DTversion = datatypes["version"];
    if ( DTerror ) {
      Serial.println("Failure on:");
      Serial.println(responseDT);
    }
    Serial.println();
    Serial.println();
  }
} // handleDatatypesResponse

/*
* Retrieve and use t6 units
*/
void handleUnitsResponse() {
  Serial.println( "handleUnitsResponse" );
} // handleUnitsResponse

/*
* Add data point to timeserie
*/
void handleDatapointResponse() {
  const int D_BUFFER_SIZE = JSON_OBJECT_SIZE(2);
  DynamicJsonBuffer D_jsonRequestBuffer(D_BUFFER_SIZE);
  JsonObject& datapoint = D_jsonRequestBuffer.parseObject(responseD);
  if (!datapoint.success()) {
    Serial.println("Failure on parsing json.");
    Serial.println(responseD);
  } else {
    const char* Derror = datapoint["error"];
    if ( Derror ) {
      Serial.println("Failure on:");
      Serial.println(responseD);
    }
    Serial.println();
    Serial.println();
  }
} // handleDatapointResponse


/*******************************************************
 addSampleToAverage
 *******************************************************/
int16_t addSampleToAverage(struct sAverage *ave, int16_t newSample) {
  ave->blockSum += newSample;
  ave->numSamples++;
}

/*******************************************************
  getAverage
 *******************************************************/
int16_t getAverage(struct sAverage *ave) {
  int16_t average = ave->blockSum / ave->numSamples;
  // get ready for the next block
  ave->blockSum = 0;
  ave->numSamples = 0;
  return average;
}

/*******************************************************
 readSample
 *******************************************************/
void readSample() {
  int count=0;
  do {
    int moisture = analogRead(VAL_PROBE);
    
    Serial.print(moisture);
    Serial.print(" -> ");
    moisture = constrain(moisture, 0, 1024);
    Serial.print(moisture);
    Serial.print(" (");
    Serial.print(count);
    Serial.println(")");
    
    count++;
    addSampleToAverage(&sampleAve, moisture);
    
    delay(100);
  } while (count <= 10);
  Serial.println("------------------------------");
}

/*
* Loop
*/
void loop() {
  readSample();
  const int BUFFER_SIZE = JSON_OBJECT_SIZE(6);
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  JsonObject& payload = jsonBuffer.createObject();
  payload["value"] = getAverage(&sampleAve);
  payload["flow_id"] = t6FlowId;
  payload["save"] = "true";
  payload["publish"] = "true";
  payload.prettyPrintTo(Serial);
  
  t6Client.createDatapoint(t6FlowId, payload, false, &responseD);
    handleDatapointResponse();
  pleaseGoToBed();
} // Loop
