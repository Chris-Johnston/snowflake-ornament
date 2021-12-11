#include <avr/io.h>

#define PIN_1 0 // row 1
#define PIN_2 1 // row 2
#define PIN_3 2 // row 3
#define PIN_4 3 // col 1
#define PIN_5 4 // col 2

void setup()
{
    pinMode(PIN_1, OUTPUT);
    pinMode(PIN_2, OUTPUT);
    pinMode(PIN_3, OUTPUT);
    pinMode(PIN_4, OUTPUT);
    pinMode(PIN_5, INPUT_PULLUP);

    while (1) {
        
        if (!digitalRead(PIN_5))
        {
            for (int i = 0; i < 6; i++)
            {
                turnOffLeds();
                turnOnLed(i);
                delay(50);
            }
            turnOffLeds();
            delay(5);
            // for (int i = 1; i < 6; i++)
            // {
            //     turnOffLeds();
            //     turnOnLed(5 - i);
            //     delay(300);
            // }
        }
        else
        {
            turnOnLed(random(6));
            delay(5);
            turnOnLed(random(6));
            delay(5);
            turnOffLeds();
            delay(300 + random(300));

        }
        

    // 0b0010 - 1
    // 0b1101 - 2
    // 0b0100
    // 0b1011 - 4
    // 0b1000
    // 0b0111
    // for (int i = 0; i < 6; i++)
    // {
    //     turnOnLed(i);

    //     delayMicroseconds(5);

    //     turnOffLeds();

    //     delay(35);
    // }
    
    // digitalWrite(PIN_1, 0);
    // digitalWrite(PIN_2, 0);
    // digitalWrite(PIN_3, 1);
    // digitalWrite(PIN_4, 1); // col 
    // digitalWrite(PIN_5, 0); // col 

    // delay(1000);

    // digitalWrite(PIN_1, LOW);
    // digitalWrite(PIN_2, LOW);
    // digitalWrite(PIN_3, LOW);
    // digitalWrite(PIN_4, LOW);
    // digitalWrite(PIN_5, LOW);

    // delay(1000);

    }
}

void loop()
{
    for (auto i = 0; i < 8; i++)
    {
        // 4 and 5 do not work
        for (auto k = 1; k < i + 1; k++)
        {
            turnOnLed(i);
            delay(50);

            turnOffLeds();
            delay(250);
        }

        turnOffLeds();
        delay(1000);
    }

    // turnOnLed(3);
    // delay(1000);

    // turnOnLed(4);
    // delay(1000);

    // turnOffLeds();

    // delay(3000);
}

void turnOnLed(int idx)
{
    // 1 | 2 | 3 | 4 |  LED
    // --------------------
    // 1   0   0   0    0
    // 0   1   0   0    2
    // 0   0   1   0    4
    // 0   0   0   1    6

    // 0   1   1   1    1
    // 1   0   1   1    3
    // 1   1   0   1    5
    // 1   1   1   0    7

    int lut[] = {
        0b1101,
        0b0010,
        0b1011,
        0b0100,
        0b0111,
        0b1000,
        // 0b1000,
        // 0b0100,
        // 0b0010,
        // 0b0001,
        // 0b0111,
        // 0b1011,
        // 0b1101,
        // 0b1110,
    };

    auto v = lut[idx] & 0b1111;

    auto existing = PORTB & 0b10000;

    PORTB = existing | v;

    // digitalWrite(PIN_1, (v & 0b1) > 0);
    // digitalWrite(PIN_2, (v & 0b10) > 0);
    // digitalWrite(PIN_3, (v & 0b100) > 0);
    // digitalWrite(PIN_4, (v & 0b1000) > 0);

    // auto high = HIGH;
    // auto low = LOW;

    // if (idx % 2 == 0)
    // {
    //     high = LOW;
    //     low = HIGH;
    // }

    // for (auto i = 1; i < 5; i++)
    // {
    //     digitalWrite(i, low);
    // }
    // digitalWrite((idx / 2) + 1, high);
}

void turnOffLeds()
{
    // digitalWrite(PIN_1, LOW);
    // digitalWrite(PIN_2, LOW);
    // digitalWrite(PIN_3, LOW);
    // digitalWrite(PIN_4, LOW);

    PORTB &= 0b10000;
}