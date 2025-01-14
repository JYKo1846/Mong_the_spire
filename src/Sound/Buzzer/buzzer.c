#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "buzzer.h"

// #define NOTE_B0  31
// #define NOTE_C1  33
// #define NOTE_CS1 35
// #define NOTE_D1  37
// #define NOTE_DS1 39
// #define NOTE_E1  41
// #define NOTE_F1  44
// #define NOTE_FS1 46
// #define NOTE_G1  49
// #define NOTE_GS1 52
// #define NOTE_A1  55
// #define NOTE_AS1 58
// #define NOTE_B1  62
// #define NOTE_C2  65
// #define NOTE_CS2 69
// #define NOTE_D2  73
// #define NOTE_DS2 78
// #define NOTE_E2  82
// #define NOTE_F2  87
// #define NOTE_FS2 93
// #define NOTE_G2  98
// #define NOTE_GS2 104
// #define NOTE_A2  110
// #define NOTE_AS2 117
// #define NOTE_B2  123
// #define NOTE_C3  131
// #define NOTE_CS3 139
// #define NOTE_D3  147
// #define NOTE_DS3 156
// #define NOTE_E3  165
// #define NOTE_F3  175
// #define NOTE_FS3 185
// #define NOTE_G3  196
// #define NOTE_GS3 208
// #define NOTE_A3  220
// #define NOTE_AS3 233
// #define NOTE_B3  247
// #define NOTE_C4  262
// #define NOTE_CS4 277
// #define NOTE_D4  294
// #define NOTE_DS4 311
// #define NOTE_E4  330
// #define NOTE_F4  349
// #define NOTE_FS4 370
// #define NOTE_G4  392
// #define NOTE_GS4 415
// #define NOTE_A4  440
// #define NOTE_AS4 466
// #define NOTE_B4  494
// #define NOTE_C5  523
// #define NOTE_CS5 554
// #define NOTE_D5  587
// #define NOTE_DS5 622
// #define NOTE_E5  659
// #define NOTE_F5  698
// #define NOTE_FS5 740
// #define NOTE_G5  784
// #define NOTE_GS5 831
// #define NOTE_A5  880
// #define NOTE_AS5 932
// #define NOTE_B5  988
// #define NOTE_C6  1047
// #define NOTE_CS6 1109
// #define NOTE_D6  1175
// #define NOTE_DS6 1245
// #define NOTE_E6  1319
// #define NOTE_F6  1397
// #define NOTE_FS6 1480
// #define NOTE_G6  1568
// #define NOTE_GS6 1661
// #define NOTE_A6  1760
// #define NOTE_AS6 1865
// #define NOTE_B6  1976
// #define NOTE_C7  2093
// #define NOTE_CS7 2217
// #define NOTE_D7  2349
// #define NOTE_DS7 2489
// #define NOTE_E7  2637
// #define NOTE_F7  2794
// #define NOTE_FS7 2960
// #define NOTE_G7  3136
// #define NOTE_GS7 3322
// #define NOTE_A7  3520
// #define NOTE_AS7 3729
// #define NOTE_B7  3951
// #define NOTE_C8  4186
// #define NOTE_CS8 4435
// #define NOTE_D8  4699
// #define NOTE_DS8 4978
// #define REST      0

/*
typedef struct{
  unsigned short frequency;
  short  duration;
} note_struct;
*/

note_struct attack_sound[]= {
    {NOTE_C4, 1},
    {NOTE_AS4,1},
    {NOTE_AS4,1},
    {NOTE_C4,2},
    {NOTE_D4,-1},
    {NOTE_C4,-2},
    {NOTE_G4,-1},
    {REST,0}
};

note_struct up_button[] = {
    {NOTE_DS8, 4},
    {NOTE_FS4, 3},
    {REST,0}
};

note_struct down_button[] = {
    {NOTE_D8,3},
    {NOTE_CS8,3},
    {NOTE_C8, 3},
    {NOTE_B7,3},
    {REST,0}
};

note_struct right_button[] = {
    {NOTE_GS6,-3},
    {NOTE_GS6,-3},
    {NOTE_GS6,-3},
    {REST,0}
};

note_struct left_button[] = {
    {NOTE_B0,2},
    /*{NOTE_C1,3},
    {NOTE_B0,1},
    {NOTE_CS1,2},
    {NOTE_D1,1},*/
    {REST,0}
};

note_struct start_button[] = {
    {NOTE_C4,8},
    {NOTE_G4,8},
    {NOTE_B7,3},
    {NOTE_A4,-4},
    {REST,0}
};

note_struct finish_button[] = {
    {NOTE_E5, 3},
    {NOTE_E5, 3},
    {NOTE_E5, 3},
    {NOTE_E5, -2},
    {NOTE_C5, -2},
    {NOTE_D5, -2},
    {NOTE_E5, -2},
    {NOTE_D5, 3},
    {NOTE_E5, -2},
    {REST, 2}
};


static inline void pwm_calcDivTop(pwm_config *c,int frequency,int sysClock)
{
  uint  count = sysClock * 16 / frequency;
  uint  div =  count / 60000;  // to be lower than 65535*15/16 (rounding error)
  if(div < 16)
      div=16;
  c->div = div;
  c->top = count / div;
}


// If tempo parameter is large, melody speed will faster
void  play_melody(uint slice_num, note_struct * n,uint tempo)
{
   
    pwm_config cfg = pwm_get_default_config();

    pwm_set_chan_level(slice_num,PWM_CHAN_A,0);

    int  wholenote = (60000 * 4) / tempo;

    int loop;
    int duration;

    for(loop=0;;loop++)
    {
      duration = n[loop].duration;
      if(duration == 0) break;

      if(duration>0)
          duration = wholenote / duration;
      else
          duration = ( 3 * wholenote / (-duration))/2;

      if(n[loop].frequency!=REST)
      {
          pwm_set_enabled(slice_num, false);
          //get new frequency
          pwm_calcDivTop(&cfg,n[loop].frequency,125000000);
          pwm_init(slice_num,&cfg,true);
          pwm_set_chan_level(slice_num,PWM_CHAN_A,cfg.top / 2);
          pwm_set_enabled(slice_num, true);
          sleep_us(900 * duration);
          pwm_set_chan_level(slice_num,PWM_CHAN_A,0);
          sleep_us(100 * duration);
      }
      else
          sleep_us(1000 * duration);
    }

}