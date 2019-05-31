#include <PixyViSy.h>
#include <SPI.h>
#include <Pixy.h>
#include <XLCPixy.h>
#include <stdint.h>

#define FOC_LEN_X 266
#define FOC_LEN_Y 237

#define GOAL_SIG 1
#define BALL_SIG 2

#define MIN_GOAL_AREA 100

PixyViSy pixyViSy(FOC_LEN_X, FOC_LEN_Y);
uint16_t goal_distance, ball_distance;
int16_t ball_angle;
char action;
uint32_t time, loop_count = 0;
uint16_t min_time = ~0, max_time = 0, average_time = 0;

void setup()
{
    Serial.begin(9600);
    pixyViSy.setGoalSig(GOAL_SIG);
    pixyViSy.setBallSig(BALL_SIG);
    pixyViSy.setMinGoalArea(MIN_GOAL_AREA);
    /* You can also set another custom preferences (e.g. setBallSize) */
}

void loop()
{
    time = (uint32_t)micros();
    pixyViSy.update(PIXYVISY_GOAL | PIXYVISY_BALL);
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

    goal_distance = pixyViSy.getGoalDist();
    action = pixyViSy.getGoalAction();
    ball_distance = pixyViSy.getBallDist();
    ball_angle = pixyViSy.getBallAngle();
    Serial.println("GOAL");
    if (goal_distance == PIXYVISY_NOBLOCK) {
        Serial.print("No object(signature: ");
        Serial.print(GOAL_SIG);
        Serial.println(") in front of camera");
    }
    else {
        Serial.print("Distance: ");
        Serial.print(goal_distance);
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
    Serial.println("BALL");
    if (ball_distance == PIXYVISY_NOBLOCK) {
        Serial.print("No object(signature: ");
        Serial.print(BALL_SIG);
        Serial.println(") in front of camera");
    }
    else {
        Serial.print("Distance: ");
        Serial.print(ball_distance);
        Serial.print("cm Angle: ");
        Serial.print(ball_angle);
        Serial.println(" degrees");
    }
    Serial.println();
    delay(100);
}

/* vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab : */
