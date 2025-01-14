#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/rand.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"

#include "image.h"

#include "Disp/Img/Digimon_img/agu/agu_growl.h"
#include "Disp/Img/Digimon_img/agu/agu_move1.h"
#include "Disp/Img/Digimon_img/agu/agu_move2.h"

#include "Disp/Img/Digimon_img/papi/papimon_1.h"
#include "Disp/Img/Digimon_img/papi/papimon_2.h"
#include "Disp/Img/Digimon_img/papi/papimon_growl.h"

#include "Disp/Ssd1306/acme_5_outlines_font.h"
#include "Disp/Ssd1306/bubblesstandard_font.h"
#include "Disp/Ssd1306/crackers_font.h"
#include "Disp/Ssd1306/BMSPA_font.h"
#include "Disp/Ssd1306/ssd1306.h"

#include "Sound/Buzzer/buzzer.h"

// hardware PIN setting
#define BUTT_UP_PIN 22
#define BUTT_DOWN_PIN 9
#define BUTT_RIGHT_PIN 12
#define BUTT_LEFT_PIN 26
#define BUTT_START_PIN 14
#define SDA_PIN 20
#define SCL_PIN 21
#define BUZZER_PIN 6

#define SLEEPTIME 25

void battle_phase(ssd1306_t displ, uint slice_num)
{
    int i, j, x, y;
    int pos= 1;        // while loop agumon position setting integer
    int displSleepT;    // display sleep time variable
    uint8_t numOfEDigi= (uint8_t)get_rand_32()%3 +1; // generated random enemy digimon number 
    unsigned char *ViewEDigi;
    unsigned long ViewEDigiSize;

    ssd1306_clear(&displ);

    // battle phase fill the screen
    for(y=0; y<64; y+=2)
    {
        ssd1306_draw_line(&displ, 0, y, 128, y);
        ssd1306_show(&displ);
        sleep_ms(20);
    }

    ssd1306_clear(&displ);

    // print to enemy digimon screen from buffer
    for(i= 0; i< 6; i++)
    {
        if(i%2== 0)
        {
            ViewEDigi= papimon_1;
            ViewEDigiSize= papimon_1_size;
            displSleepT= 350;
        }
        else if(i== 5)          // enemy digimons are ready to attack
        {
            ViewEDigi= papimon_growl;
            ViewEDigiSize= papimon_growl_size;
            displSleepT= 800;
        }
        else
        {
            ViewEDigi= papimon_2;
            ViewEDigiSize= papimon_2_size;
            displSleepT= 350;
        }

        // put enemy digimon to buffer
        for(j=numOfEDigi; j> 0; j--)
        {
            x= j*5 + (j-1)*36;      // 5 : distance between digimons, 36 : enemy digimon img's width

            if(j%2== 0)     // if second digimon exists
            {
                y= 0;
            }
            else
            {
                y= 25;
            }
            ssd1306_bmp_show_image_with_offset(&displ, ViewEDigi, ViewEDigiSize, x , y);
        }

        ssd1306_show(&displ);
        ssd1306_clear(&displ);
        sleep_ms(displSleepT);
    }


    int wait_x = 30;
    int wait_y = 8;
    // agumon ready to fight screen
    ssd1306_bmp_show_image_with_offset(&displ, agu_move1, agu_move1_size, wait_x, wait_y);
    ssd1306_show(&displ);
    sleep_ms(350);
    ssd1306_clear(&displ);
    ssd1306_bmp_show_image_with_offset(&displ, agu_growl, agu_growl_size, wait_x, wait_y);
    ssd1306_show(&displ);
    sleep_ms(800);


    // agumon wait screen
    while(1)
    {
        ssd1306_clear(&displ);

        if(!gpio_get(BUTT_START_PIN))
        {
            play_melody(slice_num, finish_button, 500);
            pos= 0;
            return ;
        }

        if(pos)
        {   
            ssd1306_bmp_show_image_with_offset(&displ, agu_move1, agu_move1_size, wait_x, wait_y);
            ssd1306_draw_string(&displ, 90, 5, 1, "ATTACK");
            ssd1306_draw_string(&displ, 90, 15, 1, "ITEM");
            ssd1306_draw_string(&displ, 90, 25, 1, "VIEW");
            ssd1306_show(&displ);
            sleep_ms(250);
            pos--;
        }
        else
        {    
            ssd1306_bmp_show_image_with_offset(&displ, agu_move2, agu_move2_size, wait_x, wait_y);
            ssd1306_draw_string(&displ, 90, 5, 1, "ATTACK");
            ssd1306_draw_string(&displ, 90, 15, 1, "ITEM");
            ssd1306_draw_string(&displ, 90, 25, 1, "VIEW");
            ssd1306_show(&displ);
            sleep_ms(450);
            pos= 1;
        }
    }


}

void buttListener(ssd1306_t displ, uint slice_num)
{
     if(!gpio_get(BUTT_UP_PIN))
        {
            play_melody(slice_num, up_button, 500);
        }

        if(!gpio_get(BUTT_DOWN_PIN))
        {
            play_melody(slice_num, down_button, 500);
        }

        if(!gpio_get(BUTT_RIGHT_PIN))
        {
            play_melody(slice_num, right_button, 1000);
        }

        if(!gpio_get(BUTT_LEFT_PIN))
        {
            play_melody(slice_num, left_button, 800);
        }

        if(!gpio_get(BUTT_START_PIN))
        {
            int i= 0;
            for(;i<2; i++)
            {
                play_melody(slice_num, start_button, 300);
                sleep_us(500);
            } 
            battle_phase(displ, slice_num);
        }
}



void setup_gpios(void) {
    i2c_init(i2c0, 400000);
    gpio_init(BUTT_UP_PIN);
    gpio_init(BUTT_DOWN_PIN);
    gpio_init(BUTT_RIGHT_PIN);
    gpio_init(BUTT_LEFT_PIN);
    gpio_init(BUTT_START_PIN);
    gpio_set_dir(BUTT_UP_PIN, GPIO_IN);                  // UP BUTTON
    gpio_set_dir(BUTT_DOWN_PIN, GPIO_IN);                  // DOWN BUTTON
    gpio_set_dir(BUTT_RIGHT_PIN, GPIO_IN);                  // RIGHT BUTTON
    gpio_set_dir(BUTT_LEFT_PIN, GPIO_IN);                  // LEFT BUTTON
    gpio_set_dir(BUTT_START_PIN, GPIO_IN);               // START BUTTON
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);   // SDA
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);   // SCL
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM); // BUZZER
    gpio_pull_up(BUTT_UP_PIN);
    gpio_pull_up(BUTT_DOWN_PIN);
    gpio_pull_up(BUTT_RIGHT_PIN);
    gpio_pull_up(BUTT_LEFT_PIN);
    gpio_pull_up(BUTT_START_PIN);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    
}

void tamagotchi(ssd1306_t displ, uint slice_num)
{
    ssd1306_t disp = displ;
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c0);
    ssd1306_clear(&disp);
     

    
    int i, j;
    int x= 79;          // move start position_x
    int y= 8;       // move start position_y

    // agumon move left
    for(; x>0; x-=3)
    {
        ssd1306_clear(&disp);
        buttListener(disp, slice_num);
        if(x%2)
        {
            ssd1306_bmp_show_image_with_offset(&disp, agu_move1, agu_move1_size, x, y);
            ssd1306_show(&disp);
            sleep_ms(250);
        }
        else
        {
            ssd1306_bmp_show_image_with_offset(&disp, agu_move2, agu_move2_size, x, y);
            ssd1306_show(&disp);
            sleep_ms(250);
        }

        ssd1306_clear(&disp);
    }

    // agumon growling
    for(i= 4; i> 0; i--)
    {
        if(i%2)
        {
            ssd1306_bmp_show_image_with_offset(&disp, agu_move1, agu_move1_size, 0, y);
            ssd1306_show(&disp);
            sleep_ms(500);
        }
        else
        {
            ssd1306_bmp_show_image_with_offset(&disp, agu_growl, agu_growl_size, 0, y);
            ssd1306_show(&disp);
            sleep_ms(500);
        
        }
        
        ssd1306_clear(&disp);
    }

    //uint  slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    play_melody(slice_num, attack_sound, 3500);

    y= 32;

    // papimon move right
    for(x=0;x<79;x+=3)
    {
        ssd1306_clear(&disp);
        buttListener(disp, slice_num);
        if(x%2)
        {
            ssd1306_bmp_show_image_with_offset(&disp, papimon_1, papimon_1_size, x, y);
            ssd1306_show(&disp);
            sleep_ms(250);
        }
        else
        {
            ssd1306_bmp_show_image_with_offset(&disp, papimon_2, papimon_2_size, x, y);
            ssd1306_show(&disp);
            sleep_ms(250);
        }  
        ssd1306_clear(&disp);
    }

    //x-=3;       // papimon position reset

    // papimon growling
    for(i= 4; i> 0; i--)
    {
        if(i%2)
        {
            ssd1306_bmp_show_image_with_offset(&disp, papimon_1, papimon_1_size, x, y);
            ssd1306_show(&disp);
            sleep_ms(500);
        }
        else
        {
            ssd1306_bmp_show_image_with_offset(&disp, papimon_growl, papimon_growl_size, x, y);
            ssd1306_show(&disp);
            sleep_ms(500);
        
        }
        
        ssd1306_clear(&disp);
    }

    ssd1306_draw_string(&disp, 8, 24, 2, "Thanks!");
    ssd1306_show(&disp);
    sleep_ms(500);
    ssd1306_clear(&disp);

}


int main() 
{
    stdio_init_all();

    //printf("configuring pins...\n");
    setup_gpios();

    //printf("buzzer start...\n");
    uint  slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    ssd1306_t disp;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c0);
    ssd1306_clear(&disp);

    while(1)
    {
        ssd1306_clear(&disp);
        
        tamagotchi(disp, slice_num);
    }
    return 0;
}