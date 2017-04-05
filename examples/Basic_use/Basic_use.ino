#include <PixyViSy.h>
#include <SPI.h>
#include <Pixy.h>
#include <stdint.h>

#define FOC_LEN_X 266
#define FOC_LEN_Y 237

#define GOAL_SIG 1
#define BALL_SIG 2

#define GOAL_HEIGHT 10
#define BALL_DIAMETER 7

#define MIN_GOAL_SIZE 100
#define MIN_BALL_SIZE 0

PixyViSy pixyViSy(FOC_LEN_X, FOC_LEN_Y,
                  GOAL_SIG, GOAL_HEIGHT, MIN_GOAL_SIZE,
                  BALL_DIAMETER, BALL_SIG, MIN_BALL_SIZE,
                  PIXYVISY_GOAL | PIXYVISY_BALL);
uint16_t goal_distance, ball_distance;
int8_t ball_angle;
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

    goal_distance = pixyViSy.getGoalDist();
    action = pixyViSy.getGoalAction();
    ball_distance = pixyViSy.getBallDist();
    ball_angle = pixyViSy.getBallAngle();
    Serial.println("GOAL");
    if (goal_distance == ~0) {
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
    if (ball_distance == ~0) {
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
