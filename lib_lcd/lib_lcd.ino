#include <UC1701.h>
#include <Wire.h>

#include "config.h"

// A custom glyph (a smiley)...
//static const byte glyph[] = { B00010000, B00110100, B00110000, B00110100, B00010000 };
//5pC/50→10pC→20pC→50pC→100pC→200pC→500pC→1000pC→2000pC
void key_up_action();
void key_down_action();
void change_data(unsigned short DAC_data);
void timeout_start();
void timeout_stop();
int  timeout_check();

const char *pulse_setting[PULSE_SETTING_MAX]={"5pC","10pC","20pC","50pC","100pC","200pC","500pC","1000pC","2000pC"};
//float voltage_level[PULSE_SETTING_MAX]={5/CAPACITY_LEVEL,10/CAPACITY_LEVEL,20/CAPACITY_LEVEL,50/CAPACITY_LEVEL,100/CAPACITY_LEVEL,200/CAPACITY_LEVEL,500/CAPACITY_LEVEL,10,10};
float voltage_level[]={0.1,0.2,0.4,1,2,4,10,10,10};

static UC1701 lcd(PIN_SCK,PIN_MOSI,PIN_CS,REGS_PIN,RSTB_PIN);
timeout_ctrl_t tm_ctrl={0,0,TIME_OUT_MS};

menu_pos_t m_pos[item_max]={{0,2,7},{10,0,1},{65,0,1}};
btn_ctrl_t btns[btn_key_max]={{BTN_UP,btn_state_init,timeout_init,key_up_action},{BTN_DOWN,btn_state_init,timeout_init,key_down_action}};
int current_idx = DEFAULT_PULSE_IDX;
int current_power = 100;
int generate_wave = 1;

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

void display_hint()
{
    lcd.writePosString("#############",0,7);
}

void display_screen()
{
    
    display_pulse();
    display_battery();
}
int previous_dis = 1;
int blinking_start = 0, bl_cur_tm = 0;

void key_up_action()
{
    if(current_idx == 0)
    {
        current_idx = PULSE_SETTING_MAX-1;
    }
    else
    {
        current_idx = current_idx-1;
    }
    clear_pulse();
    display_pulse();
    //lcd.drawLine(7);
    //display_screen();
    btns[btn_key_up].btn_state = btn_state_init;
  //current_idx = current_idx%PULSE_SETTING_MAX;
    //blinking
    blinking_start = millis();
}

void key_down_action()
{

}

uint8 btn_key_get(uint8 btn)
{
    return (btn == BTN_UP)?btn_key_up:btn_key_down;
}

int blinking_check()
{
    bl_cur_tm = millis();
    int gap = bl_cur_tm - blinking_start;
    if(gap < 200)
        return timeout_in;
    blinking_start = millis();
    return timeout_out;
}

void  blinking_menu()
{
    if(blinking_check() == timeout_out)
    {
        if(previous_dis == 1)
        {
            previous_dis = 0;
            clear_pulse();
        }
        else if(previous_dis == 0)
        {
            previous_dis =1;
            display_pulse();
        }
    }
}

void btn_action(uint8 btn_key,int val)
{
    if(KEY_PRESSED == val && btns[btn_key].btn_state == btn_state_init)
    {
        btns[btn_key].btn_state = btn_state_pressed;
        delayMicroseconds(1000);
        
    }
    if(KEY_RELEASED == val && btns[btn_key].btn_state == btn_state_pressed)
    {
        btns[btn_key].btn_state = btn_state_released;
        delayMicroseconds(1000);
        btns[btn_key].timeout_state = timeout_init;
        generate_wave = 0;
        timeout_start();
        btns[btn_key].btn_action();
        
    }
    else if(KEY_RELEASED == val && btns[btn_key].btn_state == btn_state_init&&(generate_wave ==0))
    {
            if(timeout_check() == timeout_out)
            {
                generate_wave = 1;
                //timeout_stop();
                clear_pulse();
                display_pulse();
                //display_screen();
            }
            else if(timeout_check() == timeout_in)
            {
                blinking_menu();
                //display_hint();
            }
    }
}


void btn_up_isr()
{
    int val = digitalRead(BTN_UP);
    uint8 key = btn_key_get(BTN_UP);
    btn_action(key,val);
}

void btn_state_check(int btn_pin)
{
    int val = digitalRead(btn_pin);
    uint8 key = btn_key_get(btn_pin);
    btn_action(key,val);
}



//blinking three seconds
void timeout_start()
{
    tm_ctrl.key_time = millis();
    tm_ctrl.key_triggle = 1;
}

void timeout_stop()
{
    tm_ctrl.key_time = 0;
    tm_ctrl.key_triggle = 0;
}


int timeout_blinking()
{
    int cur_tm = millis();
    if(tm_ctrl.key_triggle == 0)
        return timeout_init;
    int tm_gap = cur_tm - tm_ctrl.key_time;
    int val = tm_gap/BLINKING_TIME;
    return ((val %2)==1)?1:0;
}

int timeout_check()
{
    int cur_tm = millis();
    if(tm_ctrl.key_triggle == 0)
        return timeout_init;
    if((cur_tm - tm_ctrl.key_time)<tm_ctrl.timeout_time)
        return timeout_in;
    else
    {
        return timeout_out;
    }
    return timeout_init;
}
//start to blinking
int previous_time = 0;
int dis_blank = 0,dis_data = 0;
#if 0
void check_btn_state(uint8 btn)
{
    if(btns[btn].btn_state == btn_state_released && btns[btn].timeout_state == timeout_init)
    {
        btns[btn].btn_state = btn_state_init;
        btns[btn].timeout_state = timeout_in;
        generate_wave = 0;
        timeout_start();
        if(previous_time == 0)
            previous_time = millis();
    }
    else if(btns[btn].btn_state == btn_state_init && btns[btn].timeout_state == timeout_in)
    {
        int t_check = timeout_check();
        int now_tm = millis();
        int gap = now_tm - previous_time;
        if((gap)<200)
        {
            dis_data = 1;
            dis_blank = 0;
        }
        else if((gap)>=200&& gap <400)
        {
            dis_data = 0;
            dis_blank = 1;
        }
        else if(gap >=400)
        {
          previous_time = millis();
        }

        if(t_check == timeout_in)
        {
            if(dis_data)
            {
                display_pulse();
            }
            if(dis_blank)
                clear_pulse();
        }
        else if(t_check == timeout_out)
        {
            btns[btn].timeout_state = timeout_out;
            display_pulse();
        }
    }
    else if(btns[btn].btn_state == btn_state_init && btns[btn].timeout_state == timeout_out)
    {

        btns[btn].timeout_state = timeout_init;
        clear_pulse();
        display_pulse();
        generate_wave = 1;
        //todo setpulse
    }
}
#endif

void voltage_set(float vout)
{
  //unsigned short DAC_data = (unsigned short)((vout * MAX_IC_RANGE)/10);
  float cal_val = (vout*MAX_IC_RANGE)/10;
  uint16 DAC_data = (uint16)cal_val;
  //Serial.printf("set data:%08x  cal:%f\n\r",DAC_data,cal_val);
  Wire.beginTransmission(I2C_DEV); // transmit to device #4
  Wire.write(0x02);        // sends five bytes
  Wire.write(DAC_data%256);  //数据 低八位                      
  Wire.write(DAC_data>>8); //数据 高四位    
  Wire.endTransmission();    // stop transmitting
}


void generate_pulse()
{
    //set_val = (set_val == 0)?vout:0;
    float vout = voltage_level[current_idx];
    voltage_set(vout);
    delayMicroseconds(WAVE_DELAY);
    voltage_set(0);
    delayMicroseconds(WAVE_DELAY);

}

void gpio_init()
{
    Serial.begin(115200);
    for(int i =0 ; i < btn_key_max;i++)
    {
        pinMode(btns[i].btn_num,OUTPUT);
        digitalWrite(btns[i].btn_num,1);
    }
    //pinMode(BTN_UP,OUTPUT);
    //digitalWrite(BTN_UP,1);
    //pinMode(BTN_UP,OUTPUT);
    //attachInterrupt(digitalPinToInterrupt(BTN_UP), btn_up_isr, CHANGE);
    Wire.begin(SDA_PIN,SCL_PIN,I2C_HZ);
}

void setup() {
  gpio_init();
  lcd.begin();
  lcd.clear();
  //lcd.setCursor(0,0);
  //lcd.writeBigString("2000pC",0,2);
  //lcd.setCursor(10,0);
  //lcd.writeString("Battery:");
  //lcd.setCursor(65,0);
  //lcd.writeString("100%");
  display_screen();
  //gpio_init();
}

void loop() {
  #if 0
  delay(1000);
  clear_pulse();
  delay(1000);
  current_idx++;
  current_idx = current_idx%PULSE_SETTING_MAX;
  //current_idx = (current_idx++)%PULSE_SETTING_MAX;
  display_pulse();
  #endif
  //if(digitalRead(BTN_UP) == 0)
  //{
    //  Serial.println("button pressed");
  //}
  if(generate_wave)
      generate_pulse();
  
  for(int i =0 ; i < btn_key_max; i++)
  {
      btn_state_check(btns[i].btn_num);
  }
}
