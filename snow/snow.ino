#include <avr/io.h>
#include <avr/power.h>
#include <avr/sleep.h>

#define PIN_1 0 // row 1
#define PIN_2 1 // row 2
#define PIN_3 2 // row 3
#define PIN_4 3 // col 1
#define PIN_BUTTON 4 // col 2

#define DEBOUNCE_TIME 80
#define NUM_PATTERNS 9

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
            return;
        case 2:
            randomBlink();
            return;
        case 3:
            counter();
            return;
        case 4:
            spin();
            return;
        // TODO: add more patterns
        case 5:
            spinReverse();
            return;
        case 6:
            allOn();
            return;
        case 7:
            allOnDim();
            return;
        case 8:
            piDigits();
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

// first 200 digits of pi
uint8_t digits[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3, 2, 3, 8, 4, 6, 2, 6, 4, 3, 3, 8, 3, 2, 7, 9, 5, 0, 2, 8, 8, 4, 1, 9, 7, 1, 6, 9, 3, 9, 9, 3, 7, 5, 1, 0, 5, 8, 2, 0, 9, 7, 4, 9, 4, 4, 5, 9, 2, 3, 0, 7, 8, 1, 6, 4, 0, 6, 2, 8, 6, 2, 0, 8, 9, 9, 8, 6, 2, 8, 0, 3, 4, 8, 2, 5, 3, 4, 2, 1, 1, 7, 0, 6, 7, 9, 8, 2, 1, 4, 8, 0, 8, 6, 5, 1, 3, 2, 8, 2, 3, 0, 6, 6, 4, 7, 0, 9, 3, 8, 4, 4, 6, 0, 9, 5, 5, 0, 5, 8, 2, 2, 3, 1, 7, 2, 5, 3, 5, 9, 4, 0, 8, 1, 2, 8, 4, 8, 1, 1, 1, 7, 4, 5, 0, 2, 8, 4, 1, 0, 2, 7, 0, 1, 9, 3, 8, 5, 2, 1, 1, 0, 5, 5, 5, 9, 6, 4, 4, 6, 2, 2, 9, 4, 8, 9, 5, 4, 9, 3, 0, 3, 8, 1, 9};

void piDigits()
{
    auto idx = ((millis() - button_debounce) / 250) % 200;
    auto digit = digits[idx];
    
    for (auto bit = 0; bit < 6; bit++)
    {
        if (digit & (1 << bit))
        {
            turnOnLed(bit);
            delayMicroseconds(50);
        }
        turnOffLeds();
        delayMicroseconds(5);
    }
}

void allOn()
{
    for (auto i = 0; i < 6; i++)
    {
        turnOnLed(i);
        delayMicroseconds(5);
    }
}

void allOnDim()
{
    for (auto i = 0; i < 6; i++)
    {
        turnOnLed(i);
        delayMicroseconds(50);
        turnOffLeds();
        delayMicroseconds(3);
    }
}

void spin()
{
    auto time = (millis() / 250) % 6;

    turnOnLed(time);
    delayMicroseconds(50);
    turnOffLeds();
    delayMicroseconds(3);
}

void spinReverse()
{
    auto time = (millis() / 250) % 6;

    turnOnLed(5 - time);
    delayMicroseconds(50);
    turnOffLeds();
    delayMicroseconds(3);
}

void counter()
{
    // counts in binary
    // 6 bits, so 2^5
    auto time = (millis() / 1000) % 64;
    
    for (auto bit = 0; bit < 6; bit++)
    {
        if (time & (1 << bit))
        {
            turnOnLed(bit);
            delayMicroseconds(50);
        }
        turnOffLeds();
        delayMicroseconds(5);
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