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
#   include <stdlib.h>
#else
#   include <SD.h>
#endif

#include <mdif.h>

void create_gradient_image(mdif_t* image, int width, int height) {
    mdif_init(image, width, height);
    if(!image->red ||
        !image->blue ||
        !image->green ||
        !image->alpha
    ) {
        #ifndef ARDUINO

        printf("Failed to allocate memory for image channels.\r\n");
        exit(-1);

        #else

        Serial.println("Failed to allocate memory for image channels.");
        while(true);

        #endif
    }

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            size_t index = y * width + x;

            image->red[index]   = (char) ((255 * (width - 1 - x)) / (width - 1));;
            image->green[index] = 0;
            image->blue[index]  = (char) ((255 * x) / (width - 1));
            image->alpha[index] = (char) 255;
        }
    }
}

#ifdef ARDUINO

void setup() {
    Serial.begin(115200);

    if(!SD.begin(5)) {
        Serial.println("SD card initialization failed!");
        while(true);
    }

    mdif_t image;
    create_gradient_image(&image, 256, 256);

    mdif_error_t result = mdif_write("/image.mdif", &image);
    if(result != MDIF_ERROR_NONE) {
        Serial.print("Error: ");
        Serial.println(mdif_error_message(result));
        while(true);
    }

    mdif_free(&image);
    Serial.println("Gradient MDIF file created successfully.");
}

void loop() {
    delay(1000);
}

#else

int main() {
    mdif_t image;
    create_gradient_image(&image, 256, 256);

    mdif_error_t result = mdif_write("image.mdif", &image);
    if(result != MDIF_ERROR_NONE) {
        printf("Error: %s\r\n", mdif_error_message(result));
        return -1;
    }

    mdif_free(&image);
    printf("Gradient MDIF file created successfully.\r\n");
    return 0;
}

#endif