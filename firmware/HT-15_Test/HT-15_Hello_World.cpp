#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();

    int8_t AUDIO_PIN = 25;
    gpio_init(AUDIO_PIN);
    gpio_set_dir(AUDIO_PIN, GPIO_OUT);

    gpio_put(AUDIO_PIN, 0);

    int8_t LED_PIN = 27;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(1500);
        gpio_put(LED_PIN, 0);
        sleep_ms(1500);
    }
}
