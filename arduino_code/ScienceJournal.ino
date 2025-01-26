/*
  This file is part of the Arduino_ScienceKitCarrier library.
  Copyright (c) 2023 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "ble_config.h"
#include "Arduino_ScienceKitCarrier.h"

String name;
unsigned long lastNotify = 0;
ScienceKitCarrier science_kit;
rtos::Thread thread_update_sensors;

bool ble_is_connected = false;


void setup(){
  science_kit.begin(NO_AUXILIARY_THREADS); // Doesn't start the BME688 and external temperature threads for the moment

  if (!BLE.begin()){
    while(1);
  }

  String address = BLE.address();

  address.toUpperCase();

  name = "ScienceKit R3 - ";
  name += address[address.length() - 5];
  name += address[address.length() - 4];
  name += address[address.length() - 2];
  name += address[address.length() - 1];

  BLE.setLocalName(name.c_str());
  BLE.setDeviceName(name.c_str());


  BLE.setAdvertisedService(service);
  /* ________________________________________________________________VERSION  */
  service.addCharacteristic(versionCharacteristic);
  /* ________________________________________________________________CURRENT  */
  service.addCharacteristic(currentCharacteristic);
  /* ________________________________________________________________VOLTAGE  */
  service.addCharacteristic(voltageCharacteristic);
  /* ______________________________________________________________RESISTANCE */
  service.addCharacteristic(resistanceCharacteristic);
  /* _________________________________________________________________LIGHT   */
  service.addCharacteristic(lightCharacteristic);
  /* _______________________________________________________________PROXIMITY */
  service.addCharacteristic(proximityCharacteristic);
  /* ____________________________________________________________ACCELERATION */
  service.addCharacteristic(accelerationCharacteristic);
  /* _______________________________________________________________GYROSCOPE */
  service.addCharacteristic(gyroscopeCharacteristic);
  /* ____________________________________________________________MAGNETOMETER */
  service.addCharacteristic(magnetometerCharacteristic);
  /* _____________________________________________________________TEMPERATURE */
  service.addCharacteristic(temperatureCharacteristic);
  /* ________________________________________________________________PRESSURE */
  service.addCharacteristic(pressureCharacteristic);
  /* ________________________________________________________________HUMIDITY */
  service.addCharacteristic(humidityCharacteristic);
  /* _____________________________________________________________AIR_QUALITY */
  service.addCharacteristic(airQualityCharacteristic);
  /* _________________________________________________________SOUND_INTENSITY */
  service.addCharacteristic(sndIntensityCharacteristic);
  /* _____________________________________________________________SOUND_PITCH */
  service.addCharacteristic(sndPitchCharacteristic);
  /* _________________________________________________________________INPUT_A */
  service.addCharacteristic(inputACharacteristic);
  /* _________________________________________________________________INPUT_B */
  service.addCharacteristic(inputBCharacteristic);
  /* ____________________________________________________EXTERNAL_TEMPERATURE */
  service.addCharacteristic(extTempCharacteristic);
  /* ____________________________________________________FUNCTION_GENERATOR_1 */
  service.addCharacteristic(funcGenOneCharacteristic);
  /* ____________________________________________________FUNCTION_GENERATOR_2 */
  service.addCharacteristic(funcGenTwoCharacteristic);
  /* ________________________________________________________________DISTANCE */
  service.addCharacteristic(distanceCharacteristic);
  /* ____________________________________________________________________PING */
  service.addCharacteristic(pingCharacteristic);
  

  /* ________________________________________________________________VERSION  */
  versionCharacteristic.setValue(VERSION);

  BLE.addService(service);
  BLE.advertise();

  science_kit.startAuxiliaryThreads(); // start the BME688 and External Temperature Probe threads

  thread_update_sensors.start(update); // this thread updates sensors
}


void update(void){
  while(1){
    science_kit.update(ROUND_ROBIN_ENABLED);
    rtos::ThisThread::sleep_for(25);
  }
}

void loop(){
  BLEDevice central = BLE.central();
  if (central) {
    ble_is_connected = true;
    lastNotify=millis();
    while (central.connected()) {
      if (millis()-lastNotify>10){
        updateSubscribedCharacteristics();
        lastNotify=millis();
      }
    }
  }
  else {
    delay(100);
    ble_is_connected = false;
  }
}

void updateSubscribedCharacteristics(){
  /* ________________________________________________________________CURRENT  */
  if(currentCharacteristic.subscribed()){
    currentCharacteristic.writeValue(science_kit.getCurrent());
  }


  /* ________________________________________________________________VOLTAGE  */
  if(voltageCharacteristic.subscribed()){
    voltageCharacteristic.writeValue(science_kit.getVoltage());
  }


  /* ______________________________________________________________RESISTANCE */
  if(resistanceCharacteristic.subscribed()){
    resistanceCharacteristic.writeValue(science_kit.getResistance());  
  }


  /* _________________________________________________________________LIGHT   */
  if (lightCharacteristic.subscribed()){
    long light[4];
    light[0] = science_kit.getRed();
    light[1] = science_kit.getGreen();
    light[2] = science_kit.getBlue();
    light[3] = science_kit.getClear();
    lightCharacteristic.writeValue((byte*)light, sizeof(light));
  }


  /* _______________________________________________________________PROXIMITY */
  if (proximityCharacteristic.subscribed()){                                     
    proximityCharacteristic.writeValue(science_kit.getProximity());
  }


  /* ____________________________________________________________ACCELERATION */
  if (accelerationCharacteristic.subscribed()){
    float acceleration[3];
    acceleration[0] = science_kit.getAccelerationX();
    acceleration[1] = science_kit.getAccelerationY();
    acceleration[2] = science_kit.getAccelerationZ();
    accelerationCharacteristic.writeValue((byte*)acceleration, sizeof(acceleration));
  }

  /* _______________________________________________________________GYROSCOPE */
  if (gyroscopeCharacteristic.subscribed()){
    float gyroscope[3];
    gyroscope[0] = science_kit.getAngularVelocityX();
    gyroscope[1] = science_kit.getAngularVelocityY();
    gyroscope[2] = science_kit.getAngularVelocityZ();
    gyroscopeCharacteristic.writeValue((byte*)gyroscope, sizeof(gyroscope));
  }

  /* ____________________________________________________________MAGNETOMETER */
  if (magnetometerCharacteristic.subscribed()){
    float magnetometer[3];
    magnetometer[0] = science_kit.getMagneticFieldX();
    magnetometer[1] = science_kit.getMagneticFieldY();
    magnetometer[2] = science_kit.getMagneticFieldZ();
    magnetometerCharacteristic.writeValue((byte*)magnetometer, sizeof(magnetometer));
  }
  
  /* 
   * BME688 
   */

  /* _____________________________________________________________TEMPERATURE */
  if(temperatureCharacteristic.subscribed()){
    temperatureCharacteristic.writeValue(science_kit.getTemperature());
  }
  
  /* ________________________________________________________________PRESSURE */
  if(pressureCharacteristic.subscribed()){
    pressureCharacteristic.writeValue(science_kit.getPressure());
  }
  
  /* ________________________________________________________________HUMIDITY */
  if(humidityCharacteristic.subscribed()){
    humidityCharacteristic.writeValue(science_kit.getHumidity());
  }

  /* _____________________________________________________________AIR_QUALITY */
  if(airQualityCharacteristic.subscribed()){
    airQualityCharacteristic.writeValue(science_kit.getAirQuality());
  }
  
  /*
   * MICROPHONE
   */

  /* _________________________________________________________SOUND_INTENSITY */
  /* NOTE: raw value - value not in Db */ 
  if(sndIntensityCharacteristic.subscribed()){
    sndIntensityCharacteristic.writeValue(science_kit.getMicrophoneRMS());
  }

  /* _____________________________________________________________SOUND_PITCH */
  if(sndPitchCharacteristic.subscribed()){
    sndPitchCharacteristic.writeValue(science_kit.getExternalTemperature());
  }

  /* _________________________________________________________________INPUT_A */
  if (inputACharacteristic.subscribed()){
    inputACharacteristic.writeValue(science_kit.getInputA());
  }

  /* _________________________________________________________________INPUT_B */
  if (inputBCharacteristic.subscribed()){
    inputBCharacteristic.writeValue(science_kit.getInputB());
  }

  /*_____________________________________________________EXTERNAL_TEMPERATURE */
  if(extTempCharacteristic.subscribed()){
    extTempCharacteristic.writeValue(science_kit.getExternalTemperature());
  }


  /* ____________________________________________________FUNCTION_GENERATOR_1 */
  if (funcGenOneCharacteristic.subscribed()){
    long f1[2];
    f1[0] = (science_kit.getFrequency1() * science_kit.getRange1());
    f1[1] = science_kit.getPhase1();
    funcGenOneCharacteristic.writeValue((byte*)f1, sizeof(f1));
  }

  /* ____________________________________________________FUNCTION_GENERATOR_2 */
  if (funcGenTwoCharacteristic.subscribed()){
    long f2[2];
    f2[0] = (science_kit.getFrequency2() * science_kit.getRange2());
    f2[1] = science_kit.getPhase2();
    funcGenTwoCharacteristic.writeValue((byte*)f2, sizeof(f2));
  }

  /* ________________________________________________________________DISTANCE */
  if (distanceCharacteristic.subscribed()){
    if (science_kit.getUltrasonicIsConnected()){
      /* NOTE: getDistance() calls getMeters() 
         Requested value is in meters */
      distanceCharacteristic.writeValue(science_kit.getDistance());
    }
    else{
      distanceCharacteristic.writeValue(-1.0);
    }
  }

  /* ____________________________________________________________________PING */
  if (pingCharacteristic.subscribed()){
     if (science_kit.getUltrasonicIsConnected()){
      /* NOTE: getTravelTime() returns micro seconds  */
      /* Converted to milliseconds (agreed with RF 20230719) */
      pingCharacteristic.writeValue(science_kit.getTravelTime() * 1000.0 );
    }
    else{
      pingCharacteristic.writeValue(-1.0);
    } 
  }  
}





/***
 *                       _       _                                    
 *         /\           | |     (_)                                   
 *        /  \   _ __ __| |_   _ _ _ __   ___                         
 *       / /\ \ | '__/ _` | | | | | '_ \ / _ \                        
 *      / ____ \| | | (_| | |_| | | | | | (_) |                       
 *     /_/____\_\_| _\__,_|\__,_|_|_| |_|\___/ ___ _     _____  ____  
 *      / ____|    (_)                     | |/ (_) |   |  __ \|___ \ 
 *     | (___   ___ _  ___ _ __   ___ ___  | ' / _| |_  | |__) | __) |
 *      \___ \ / __| |/ _ \ '_ \ / __/ _ \ |  < | | __| |  _  / |__ < 
 *      ____) | (__| |  __/ | | | (_|  __/ | . \| | |_  | | \ \ ___) |
 *     |_____/ \___|_|\___|_| |_|\___\___| |_|\_\_|\__| |_|  \_\____/ 
 *                                                                    
 *                                                                    
 */