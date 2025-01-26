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
#   include <string.h>
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

        perror("Failed to allocate memory for image channels.");
        exit(EXIT_FAILURE);

        #else

        Serial.println("Failed to allocate memory for image channels.");
        while(true);

        #endif
    }

    for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++) {
            size_t index = y * width + x;

            image->red[index]   = (char) ((255 * (width - 1 - x)) / (width - 1));
            image->green[index] = 0;
            image->blue[index]  = (char) ((255 * x) / (width - 1));
            image->alpha[index] = (char) 255;
        }
}

void print_grayscale_table(const float* grayscale, int len) {
    int x = 0;
    for(int i = 0; i < len; i++) {
        if(grayscale[i] >= 0.0)
            #ifndef ARDUINO
            printf(" ");
            #else
            Serial.print(" ");
            #endif

        #ifndef ARDUINO
        printf("%.2f\t", grayscale[i]);
        #else
        Serial.print(grayscale[i]);
        Serial.print("\t");
        #endif

        x++;
        if(x == 5) {
            #ifndef ARDUINO
            printf("\r\n");
            #else
            Serial.println();
            #endif

            x = 0;
        }
    }
}

#ifdef ARDUINO

void setup() {
    Serial.begin(115200);

    mdif_t image;
    short width = 5, height = 5;

    create_gradient_image(&image, width, height);

    float* grayscale = (float*)malloc(width * height * sizeof(float));
    if(!grayscale) {
        Serial.println("Failed to allocate memory for grayscale.");

        mdif_free(&image);
        while(true);
    }

    mdif_error_t result = mdif_grayscale(&image, grayscale);
    if(result == MDIF_ERROR_NONE)
        print_grayscale_table(grayscale, width * height);
    else {
        Serial.print("Error: ");
        Serial.println(mdif_error_message(result));
    }

    free(grayscale);
    mdif_free(&image);
}

void loop() {
    delay(1000);
}

#else

int main() {
    mdif_t image;

    int width = 5, height = 5;
    create_gradient_image(&image, width, height);

    float* grayscale = (float*)malloc(width * height * sizeof(float));
    if(!grayscale) {
        printf("Failed to allocate memory for grayscale.");
        mdif_free(&image);

        return -1;
    }

    mdif_error_t result = mdif_grayscale(&image, grayscale);
    if(result == MDIF_ERROR_NONE)
        print_grayscale_table(grayscale, width * height);
    else printf("Error: %s\r\n", mdif_error_message(result));

    free(grayscale);
    mdif_free(&image);

    return 0;
}

#endif