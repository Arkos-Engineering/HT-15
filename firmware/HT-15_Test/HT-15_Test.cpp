#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "pindefs.c"


void set_audioamp_power(bool state){
    if(state){
        printf("Disabling Audio Amp Power\n");
    } else{
        printf("Enabling Audio Amp Power\n");
    }
    gpio_put(AUDIOAMP_POWER, !state);
}

void init_all(){
    //Audioamp power pin
    gpio_init(AUDIOAMP_POWER);
    gpio_set_dir(AUDIOAMP_POWER, GPIO_OUT);
    set_audioamp_power(true);

    //init status LED
    gpio_init(LED_STATUS);
    gpio_set_dir(LED_STATUS, GPIO_OUT);
    gpio_put(LED_STATUS, 1);

}

void core_0() {
    printf("Core 1 launched\n");


    bool led_state = false;
    gpio_put(LED_STATUS, 1);
   
    //define button matricies
    uint8_t button_sense_pin[6] = {BTN_MTX_0, BTN_MTX_1, BTN_MTX_2, BTN_MTX_3, BTN_MTX_4, BTN_MTX_5};
    uint8_t button_pwr_pin[4] = {BTN_MTX_A, BTN_MTX_B, BTN_MTX_C, BTN_MTX_D};
    char button_names[6][4][6] = {
        {"Left", "Up", "Right", "Side2"},
        {"Back", "Down", "Enter", "Side1"},
        {"1", "2", "3", "PTT"},
        {"4", "5", "6", "Lock"},
        {"7", "8", "9", "NA"},
        {"*", "0", "#", "NA"}
    };

    //button states [sense][pwr][debounce_pointer] debounce_pointer toggles every loop to store last two states
    bool button_states[6][4][2] = {0};
    printf("Button matrix initialized\n");

    //init button pins
    for (int i = 0; i < 6; i++) {
        gpio_init(button_sense_pin[i]);
        gpio_set_dir(button_sense_pin[i], GPIO_IN);
        gpio_pull_down(button_sense_pin[i]);
    }
    for (int i = 0; i < 4; i++) {
        gpio_init(button_pwr_pin[i]);
        gpio_set_dir(button_pwr_pin[i], GPIO_OUT);
        gpio_put(button_pwr_pin[i], 0);
    }

    bool button_debounce_pointer=0;
    bool button_has_been_pressed = false;

    uint8_t counter = 0;
    printf("Starting main loop on core 1\n");
    while (true) {
        //toggle LED
        if(!(counter%50)){
            led_state = !led_state;
            gpio_put(LED_STATUS, led_state);
        }

        //scan buttons
        for (int j = 0; j < 4; j++) {
            gpio_put(button_pwr_pin[j], 1);
            sleep_us(1);
            for (int i = 0; i < 6; i++) {
                button_states[i][j][button_debounce_pointer] = gpio_get(button_sense_pin[i]);
            }
            gpio_put(button_pwr_pin[j], 0);
            sleep_us(1);
        }
        button_debounce_pointer = !button_debounce_pointer;

        //check for button presses with debounce
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 4; j++) {
                if(button_states[i][j][0] && button_states[i][j][1]){
                    button_has_been_pressed = true;
                    printf(button_names[i][j]);
                    printf(" ");
                }
            }
        }

        //print new line if any button has been pressed, and reset flag
        if (button_has_been_pressed){
            button_has_been_pressed = false;
            printf("\n");
        }

        //manage counter
        counter++;
        if(counter>=100){
            counter = 0;
        }
        sleep_ms(10);
    }
}

void core_1() {
    while(true){
        sleep_ms(1000);
    }
}

int main(){
    
    stdio_init_all();
    init_all();
    sleep_ms(5000);

    printf("Device Initalized!\n");
    multicore_reset_core1();
    multicore_launch_core1(core_1);
    core_0();

}