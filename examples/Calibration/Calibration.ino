#include <SPI.h>
#include <Pixy.h>
#include <XLCPixy.h>

#define REPEATING 20

XLCPixy<LinkSPI> pixy;

void setup()
{
    Serial.begin(9600);
    pixy.init();
}

void loop()
{
    Serial.println("Place your camera right in front of detected object.");
    Serial.println(\
        "The first received block in each frame will be taken into account.");
    Serial.println("Send something through serial to start.");

    while (!Serial.available());
    while (Serial.available()) Serial.read();

    uint32_t width = 0, height = 0;

    for (uint8_t i = 0; i < REPEATING;) {
        uint16_t count = pixy.getBlocks();
        if (count == 0) {
            Serial.println("No block");
            continue;
        }
        pixy.blocks[0].print();
        width += pixy.blocks[0].width;
        height += pixy.blocks[0].height;
        i++;
        delay(50);
    }
    width /= REPEATING;
    height /= REPEATING;

    Serial.print("pixel_Fy = ");
    Serial.print(height);
    Serial.println(" * distance_to_object / object_height");

    Serial.print("pixel_Fx = ");
    Serial.print(width);
    Serial.println(" * distance_to_object / object_width");
}
