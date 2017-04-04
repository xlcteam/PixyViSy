#include <PixyViSy.h>
#include <SPI.h>
#include <Pixy.h>
#include <stdint.h>

#define SIG 1

PixyViSy pixyViSy(SIG, 237, 266, 10);
uint16_t distance;
char action;
uint32_t time, loop_count = 0;
uint16_t min_time = ~0, max_time = 0, average_time = 0;

void setup()
{
    Serial.begin(9600);
}
void loop()
{
    time = (uint32_t)micros();
    pixyViSy.update();
    time = (uint32_t)micros() - time;
    if (time < min_time) {
        min_time = time;
    } else if (time > max_time) {
        max_time = time;
    }
    average_time = (average_time * loop_count + time) / (loop_count + 1);
    loop_count++;
    Serial.print("Average time: ");
    Serial.print(average_time);
    Serial.print(" Min time: ");
    Serial.print(min_time);
    Serial.print(" Max time: ");
    Serial.println(max_time);
    Serial.print("Goal height: ");
    Serial.print(pixyViSy.getGoalPixHeight());
    Serial.println(" pixels");

    distance = pixyViSy.getDistance();
    action = pixyViSy.getAction();
    if (distance == ~0) {
        Serial.print("No object(signature: ");
        Serial.print(SIG);
        Serial.println(") in front of camera");
    }
    else {
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.print("cm Suggested action: ");
        switch (action) {
            case 'K': {
                Serial.println("KICK!");
                break;
            }
            case 'L': {
                Serial.println("MOVE LEFT!");
                break;
            }
            case 'R': {
                Serial.println("MOVE RIGHT!");
                break;
            }
            default: {
                Serial.println("ERROR!");
                break;
            }
        }
    }
    Serial.println();
    delay(100);
}

/* vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab : */
