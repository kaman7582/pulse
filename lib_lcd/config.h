#ifndef CONFIG_H
#define CONFIG_H

#define PIN_CS    5
#define PIN_SCK   18
#define PIN_MOSI  23
#define PIN_MISO  19
#define REGS_PIN  15
#define RSTB_PIN  16

//SDA  SDA (default is GPIO 21)
//SCL SCL (default is GPIO 22)

#define SDA_PIN     21
#define SCL_PIN     22
#define I2C_HZ      400000
#define I2C_DEV     0x5b
#define MAX_IC_RANGE   0xFFF
#define WAVE_DELAY  2000

#define BTN_UP    14
#define BTN_DOWN  12

#define KEY_PRESSED             0
#define KEY_RELEASED            1

#define PULSE_SETTING_MAX   9
#define DEFAULT_PULSE_IDX   4

#define MAX_ROW             8
#define MAX_COL             96
#define TIME_VOUT_MS        2000
#define TIME_BLINK_MS       200
#define MAX_DISPLAY_ITEMS   3
#define CAPACITY_LEVEL      50
#define BLINKING_TIME       500 //ms
typedef unsigned char  uint8;
typedef unsigned short uint16;


//define for pwm
#define  PWM_FREQENCY   4    // 频率
#define  PWM_CHANNEL    0    // 通道
#define  PWM_RESOLUTION    8   // 分辨率
#define  PWM_PIN        12
#define  PWM_PERCENT    153
/**********************/
typedef struct menu_pos_s
{
    int column;
    int row;
    int len;
}menu_pos_t;


enum menu_item
{
    pulse_item=0,
    battery_item,
    pw_value,
    item_max
};

enum btn_state
{
    btn_state_init,
    btn_state_pressed,
    btn_state_released,
    btn_state_timeout,
};

enum btn_key_list
{
    btn_key_up = 0,
    btn_key_down,
    btn_key_max
};

enum timeout_state
{
    timeout_init,
    timeout_in,
    timeout_out
};


typedef struct timeout_ctrl_s
{
    int key_time;
    int key_triggle;
    int timeout_state;
    int timeout_time;
}timeout_ctrl_t;

enum timeout_type
{
    timeout_type_blink,
    timeout_type_vout,
    timeout_type_max
};

typedef struct btn_ctrl_s
{
    uint8 btn_num;
    uint8 btn_state;
    void (*btn_action)();
}btn_ctrl_t;


#endif