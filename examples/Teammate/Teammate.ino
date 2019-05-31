#include <PixyViSy.h>
#include <SPI.h>
#include <Pixy.h>
#include <XLCPixy.h>
#include <stdint.h>

#define TEAM_SIG 012

#define FOC_LEN_X 266
#define FOC_LEN_Y 237

PixyViSy pixyViSy(FOC_LEN_X, FOC_LEN_Y);

uint16_t team_distance;
int16_t team_angle, team_dx;
uint32_t time, loop_count = 0;
uint16_t min_time = ~0, max_time = 0, average_time = 0;

void setup()
{
    Serial.begin(9600);
    pixyViSy.setTeamSig(TEAM_SIG);
    pixyViSy.setTeamSize(5);
}

void loop()
{
    time = (uint32_t)micros();
    pixyViSy.update(PIXYVISY_TEAM);
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

    team_distance = pixyViSy.getTeamDist();
    team_angle = pixyViSy.getTeamAngle();
    team_dx = pixyViSy.getTeamDx();
    Serial.println("TEAMMATE");
    if (team_distance == PIXYVISY_NOBLOCK) {
        Serial.print("No object(signature: ");
        Serial.print(TEAM_SIG);
        Serial.println(") in front of camera");
    }
    else {
        Serial.print("Distance: ");
        Serial.print(team_distance);
        Serial.print("cm Dx: ");
        Serial.print(team_dx);
        Serial.print("cm Angle: ");
        Serial.print(team_angle);
    }
    Serial.println();
    delay(50);
}
