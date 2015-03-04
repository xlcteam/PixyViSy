#include <PixyViSy.h>
#include <SPI.h>
#include <Pixy.h>
#include <stdint.h>

#define SIG 1

PixyViSy pixyViSy(SIG, 24*100, 10);
uint16_t distance;
char action;

void setup()
{
  Serial.begin(9600);
}
void loop()
{
  pixyViSy.update();
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
  delay(100);
}
