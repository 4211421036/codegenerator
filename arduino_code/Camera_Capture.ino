/*
    Originally from Harvard_TinyMLx Example: test_camera.ino

    To give a demo of our cameara usage, we will show how to capture the image using Arduino Camera.
    Camera will capture the image if you press the button on the shield, print out the raw pixel value into Serial,
    and you can convert it to an image using Camera.ipynb.
*/

#include <TinyMLShield.h>

// Image Buffer
// Among various image quality given by
// QCIF: 176x144 x 2 bytes per pixel (RGB565)
byte image[176 * 144 * 2];
int bytesPerFrame = 176 * 144 * 2;

void setup()
{
  // Initialize the Serial 
  Serial.begin(9600);
  while (!Serial);

  // initialize the Shield
  initializeShield();

  // Initialize the OV7675 camera
  if (!Camera.begin(QCIF, RGB565, 1, OV7675))
  {
    Serial.println("Failed to initialize camera");
    while (1);
  }
}

void loop()
{
  int i = 0;

  bool clicked = readShieldButton();
  if (clicked)
  {
    Serial.println("\nImage data will be printed out!");
    delay(1000);
    Camera.readFrame(image);
    for (int i = 0; i < bytesPerFrame; i += 2)
    {
      Serial.print("0x");
      Serial.print(image[i + 1], HEX);
      Serial.print(image[i], HEX);
      if (i != bytesPerFrame - 2) Serial.print(", ");
    }
    Serial.println();
    Serial.println("Done!");
  }
}