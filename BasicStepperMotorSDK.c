#include <stdio.h>
#include "pico/stdlib.h"

#define step_pin 7 // Pin 3 connected to Steps pin on EasyDriver
#define dir_pin 6  // Pin 2 connected to Direction pin

#define MS1 10 // Pin 5 connected to MS1 pin
#define MS2 11 // Pin 4 connected to MS2 pin
#define MS3 12

#define CW LOW
#define CCW HIGH

int step_time_delay = 1;
int count = 0;

// A4988 stepper motor driver
enum stepperSteps
{
    fullStep = 0b000,
    halfStep = 0b100,
    quarterStep = 0b010,
    eighthStep = 0b110,
    sixteenthStep = 0b111,

    direction = 1,

    fastest = 1,
    fast = 2,
    medium = 4,
    slow = 8,
    superSlow = 16,
    slowest = 32
};

void runMotor(u_int32_t motorArg)
{
    u_int32_t steps = motorArg >> 8;
    steps *= 200;
    gpio_put(MS1, (motorArg >> 2) & 0b1);
    gpio_put(MS2, (motorArg >> 1) & 0b1);
    gpio_put(MS3, (motorArg >> 0) & 0b1);

    gpio_put(dir_pin, ((motorArg) >> 3) & 0b1); // set direction

    while (count <= steps)
    {
        gpio_put(step_pin, 1);
        sleep_ms(step_time_delay); // duty cycle
        gpio_put(step_pin, 0);
        sleep_ms(step_time_delay);
        count++;
    }
    count = 0;
    return;
}

int main()
{
    stdio_init_all();

    gpio_init(dir_pin);
    gpio_set_dir(dir_pin, GPIO_OUT);

    gpio_init(step_pin);
    gpio_set_dir(step_pin, GPIO_OUT);

    gpio_init(MS1);
    gpio_set_dir(MS1, GPIO_OUT);

    gpio_init(MS2);
    gpio_set_dir(MS2, GPIO_OUT);

    gpio_init(MS3);
    gpio_set_dir(MS3, GPIO_OUT);

    while (1)
    {
        u_int32_t a = fastest << 8;      // how many steps (* 200) for 360º, push into 2nd byte
        u_int32_t b = direction << 3;    // CW or CCW (first byte)
        u_int32_t c = fullStep;          // bit pattern for MS* pins (still first byte)
        u_int32_t motorArgs = a + b + c; // add together into final 32-bit byte
        runMotor(motorArgs);
        sleep_ms(100);
        motorArgs = motorArgs ^ 0b1000; // flip the direction bit
        runMotor(motorArgs);
        sleep_ms(100);

        a = (fast << 8);
        b = direction << 3;
        c = halfStep;
        motorArgs = a + b + c;
        runMotor(motorArgs);
        sleep_ms(100);
        motorArgs = motorArgs ^ 0b1000;
        runMotor(motorArgs);
        sleep_ms(100);

        a = (medium << 8);
        b = direction << 3;
        c = quarterStep;
        motorArgs = a + b + c;
        runMotor(motorArgs);
        sleep_ms(100);
        motorArgs = motorArgs ^ 0b1000;
        runMotor(motorArgs);
        sleep_ms(100);

        a = (slow << 8);
        b = direction << 3;
        c = eighthStep;
        motorArgs = a + b + c;
        runMotor(motorArgs);
        sleep_ms(100);
        motorArgs = motorArgs ^ 0b1000;
        runMotor(motorArgs);
        sleep_ms(100);

        a = (superSlow << 8);
        b = direction << 3;
        c = sixteenthStep;
        motorArgs = a + b + c;
        runMotor(motorArgs);
        sleep_ms(100);
        motorArgs = motorArgs ^ 0b1000;
        runMotor(motorArgs);
        sleep_ms(100);
    }
    return 0;
}
