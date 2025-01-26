/* 
 * Copyright 2024 Nathanne Isip
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ARDUINO
#   include <stdio.h>
#else
#   include <SD.h>
#endif

#include <mdif.h>

#ifdef ARDUINO

void setup() {
    Serial.begin(115200);

    if(!SD.begin(5)) {
        Serial.println("SD card initialization failed!");
        while(true);
    }

    mdif_t image;
    mdif_error_t result = mdif_read("/image.mdif", &image);
    if(result != MDIF_ERROR_NONE) {
        Serial.print("Error: ");
        Serial.println(mdif_error_message(result));
        while(true);
    }

    Serial.print("Image width:\t");
    Serial.println(image.width);
    Serial.print("Image height:\t");
    Serial.println(image.height);
}

void loop() {
    delay(1000);
}

#else

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("Usage: %s <filename>\r\n", argv[0]);
        return -1;
    }

    mdif_t image;
    mdif_error_t result = mdif_read(argv[1], &image);
    if(result != MDIF_ERROR_NONE) {
        printf("Error: %s\r\n", mdif_error_message(result));
        return -1;
    }

    printf("Image width:\t%d\r\n", image.width);
    printf("Image height:\t%d\r\n", image.height);

    return 0;
}

#endif