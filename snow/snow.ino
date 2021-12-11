#include <avr/io.h>
#include <avr/power.h>
#include <avr/sleep.h>

#define PIN_1 0 // row 1
#define PIN_2 1 // row 2
#define PIN_3 2 // row 3
#define PIN_4 3 // col 1
#define PIN_BUTTON 4 // col 2

#define DEBOUNCE_TIME 80
#define NUM_PATTERNS 8

// track idle time
volatile unsigned long wake_time = 0;
volatile unsigned long button_debounce = 0;

// 0 - sleeping
// 1 - random blinking
volatile int pattern = 1;

ISR(PCINT0_vect)
{
    // this applies when awake
    if (pattern != 0 && digitalRead(PIN_BUTTON))
    {
        // only update pattern if less than debounce time
        auto time = millis();
        if ((time - button_debounce) > DEBOUNCE_TIME)
        {
            pattern += 1;
            pattern %= NUM_PATTERNS;

            button_debounce = millis();
        }
    }
}

void setup()
{
    power_all_disable(); // only enable what we need
    power_timer0_enable();

    // pin change interrupts
    GIMSK = (1 << PCIE);
    PCMSK |= (1 << PCINT4);

    pinMode(PIN_1, OUTPUT);
    pinMode(PIN_2, OUTPUT);
    pinMode(PIN_3, OUTPUT);
    pinMode(PIN_4, OUTPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);

    turnOffLeds();
    delay(50); // prevent LEDs being on messing with programming

    button_debounce = millis();
    
    animateWakeup();
}

void animateWakeup()
{
    for (auto i = 0; i < 6; i++)
    {
        turnOffLeds();
        turnOnLed(i);
        delay(50);
    }

    turnOffLeds();
    delay(100);
}

void animateShutoff()
{
    for (auto i = 0; i < 6; i++)
    {
        turnOffLeds();
        turnOnLed(6 - i);
        delay(50);
    }

    turnOffLeds();
    delay(100);
}

void loop()
{
    switch (pattern)
    {
        case 1:
            pwm();
            // turnOnLed(0);
            // delay(5);
            // turnOffLeds();
            //delay(250);
            return;
        case 2:
            randomBlink();
            return;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            turnOnLed(pattern - 2);
            return;
        case 0:
        default:
            sleep();

            animateWakeup();
            // unset flag on wake
            pattern = 1;
            return;
    }
}

void randomBlink()
{
    // turn on 1..3 leds each time
    for (auto i = 1; i < 4; i++)
    {
        turnOnLed(random(6));
        delay(3);
    }

    turnOffLeds();

    delay(300 + random(300));
}

void softPwmFade(int period, int iters, int set, bool inverse, int biasOn, int biasOff)
{
    for (auto i = 0; i < period; i++)
    {
        auto y = inverse ? period - i : i;
        y += biasOn;
        auto z = inverse ? i : period - i;
        z += biasOff;
        for (auto d = 0; d < iters; d++)
        {
            // PORTB = set;
            overlayPORTB(set);
            delayMicroseconds(y);

            turnOffLeds();
            delayMicroseconds(z);
        }
    }
}

void pwm()
{
    // turn on odd LEDs
    auto odd = 0b0001;
    auto even = 0b1110;
    auto period = 250;
    auto iters = 15;
    auto biasOn = 0;
    auto biasOff = 100;

    softPwmFade(period, iters, odd, false, biasOn, biasOff);
    softPwmFade(period, iters, odd, true, biasOn, biasOff);
    turnOffLeds();

    delay(100);

    softPwmFade(period, iters, even, false, biasOn, biasOff);
    softPwmFade(period, iters, even, true, biasOn, biasOff);
    turnOffLeds();

    delay(100);

}

void sleep()
{
    animateShutoff();

    turnOffLeds();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    power_all_disable();
    sleep_cpu();

    // on wake
    power_timer0_enable();
    wake_time = millis();
}

void turnOnLed(int idx)
{
   int lut[] = {
        0b1101,
        0b0010,
        0b1011,
        0b0100,
        0b0111,
        0b1000,
    };

    auto v = lut[idx] & 0b1111;

    auto existing = PORTB & 0b10000;

    PORTB = existing | v;
}

void overlayPORTB(int value)
{
    auto existing = PORTB & 0b10000;

    PORTB = existing | value;
}

void turnOffLeds()
{
    PORTB &= 0b10000;
}