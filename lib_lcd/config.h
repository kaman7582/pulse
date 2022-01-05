#ifndef CONFIG_H
#define CONFIG_H

#define PIN_CS    5
#define PIN_SCK   18
#define PIN_MOSI  23
#define PIN_MISO  19
#define REGS_PIN  15
#define RSTB_PIN  16

#define PULSE_SETTING_MAX   9
#define DEFAULT_PULSE_IDX   4

#define MAX_ROW             8
#define MAX_COL             96

#define MAX_DISPLAY_ITEMS   3
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
#endif