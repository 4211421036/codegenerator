/*
  This file is part of the Arduino_GroveI2C_Ultrasonic library.
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

#include "Arduino_GroveI2C_Ultrasonic.h"

Arduino_GroveI2C_Ultrasonic sonar;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  sonar.begin();
}

void loop() {
  sonar.update();
  Serial.print("Distance [mm]: ");
  Serial.print(sonar.getDistance());
  Serial.print("\t");
  Serial.print("travel time [us]: ");
  Serial.println(sonar.getTravelTime());
}