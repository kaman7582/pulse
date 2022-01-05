#include <UC1701.h>
#include <Timer.h>
#include "config.h"

// A custom glyph (a smiley)...
//static const byte glyph[] = { B00010000, B00110100, B00110000, B00110100, B00010000 };
//5pC/50→10pC→20pC→50pC→100pC→200pC→500pC→1000pC→2000pC
const char *pulse_setting[PULSE_SETTING_MAX]={"5pC","10pC","20pC","50pC","100pC","200pC","500pC","1000pC","2000pC"};

static UC1701 lcd(PIN_SCK,PIN_MOSI,PIN_CS,REGS_PIN,RSTB_PIN);

menu_pos_t m_pos[item_max]={{0,2,4},{10,0,1},{65,0,1}};

int current_idx = DEFAULT_PULSE_IDX;
int current_power = 100;

void display_pulse()
{
  menu_pos_t *pos = &m_pos[pulse_item];
  lcd.writeBigString(pulse_setting[current_idx],pos->column,pos->row);
}

void clear_pulse()
{
    menu_pos_t *pos = &m_pos[pulse_item];
    lcd.clearRow(pos->row,pos->len);
}

void display_battery()
{
    menu_pos_t *pos = NULL;
    char buf[6];
    pos = &m_pos[battery_item];
    lcd.writePosString("Battery:",pos->column,pos->row);
    pos = &m_pos[pw_value];
    snprintf(buf,6,"%d%%",current_power);
    lcd.writePosString(buf,pos->column,pos->row);

}

void display_screen()
{
    display_pulse();
    display_battery();
}



void setup() {
  lcd.begin();
  // Add the smiley to position "0" of the ASCII table...
  //lcd.createChar(0, glyph);
  lcd.clear();
  //lcd.setCursor(0,0);
  //lcd.writeBigString("2000pC",0,2);
  //lcd.setCursor(10,0);
  //lcd.writeString("Battery:");
  //lcd.setCursor(65,0);
  //lcd.writeString("100%");
  display_screen();
}


void loop() {
  delay(2000);
  clear_pulse();
  delay(2000);
  current_idx++;
  current_idx = current_idx%PULSE_SETTING_MAX;
  //current_idx = (current_idx++)%PULSE_SETTING_MAX;
  display_pulse();
}
