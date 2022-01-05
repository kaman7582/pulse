
#define BTN1   16
#define logprint(arg)   Serial.println(arg)
int button_state= 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BTN1,OUTPUT);
  digitalWrite(BTN1,1);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val = digitalRead(BTN1);
  if(val == 0 && button_state == 0)
  {
     logprint("button press");
     button_state = 1;
     //delay(100);
     delay(100);
  }
  
  if(val == 1 && button_state==1 )
  {
    logprint("button release");
    button_state = 0;
    //delay(100);
    delay(100);
  }
  //delay(1000);
}
