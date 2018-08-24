// This #include statement was automatically added by the Particle IDE.
#include "RTClib.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_SSD1306.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_GFX.h"

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "Wire.h"

#include "Particle.h"

SYSTEM_MODE(SEMI_AUTOMATIC);


#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

RTC_DS1307 rtc;

unsigned long actMillis;
unsigned int sleepTime = 15000;
unsigned int tempSleep = 15000;

int numOfApps = 8;
int loc = 1;
int actLoc = 1;
int actScheme = 1;

bool WiFiMode = 0;

int ledR, ledG, ledB;
int clickLedR, clickLedG, clickLedB;

class joystick{
 public:
    String state;
    void scan(){
        if(digitalRead(2)==1)
            state = "left";
        if(digitalRead(3)==1)
            state = "right";
        if(digitalRead(4)==1)
            state = "up";
        if(digitalRead(5)==1)
            state = "down";
        if(digitalRead(6)==1)
            state = "click";
        if(digitalRead(2)==0&&digitalRead(3)==0&&digitalRead(4)==0&&digitalRead(5)==0&&digitalRead(6)==0){
            RGB.color(ledR,ledG,ledB);
            state = "none";
        }
        if(state!="none"){
            actMillis = millis();
            RGB.color(clickLedR,clickLedG,clickLedB);
        }
    }
};


joystick joy;

class factoryCatcher{
    public:
        int highScore;
        int actPos;
        int speed;
        int score;
        bool alive;
        int activeBalls = 0;
        int pipe[4]; // 0 - left upper corner, 1 - left lower corner, 2 - right upper corner, 3 - right lower corner; 1 - first position, 3 - on the edge, 4 - game over
        unsigned int pipeTime[4];
        unsigned int lastAdd;
        void howToPlay(){
            
        }
        
        void resetGame(){
            actPos = 1;
            speed = 30;
            score = 0;
            alive = 1;
            activeBalls = 0;
            lastAdd = 0;
            for(int a = 0; a < 4; a++){
                pipe[a]=0;
                pipeTime[a] = millis();
            }
        }
        
        void game(){
            
             // 1 - left upper corner, 2 - left lower corner, 3 - right upper corner, 4 - right lower corner;
            display.clearDisplay();
            display.setCursor(51,0);
            display.setTextSize(2);
            if(score < 10)
                display.setCursor(62,0);
            if(score < 100)
                display.setCursor(55,0);
            display.print(String(score));
            display.drawLine(0,15,32,23,WHITE);
            display.drawLine(0,47,32,55,WHITE);
            display.drawLine(128,15,96,23,WHITE);
            display.drawLine(128,47,96,55,WHITE);
            display.setTextSize(1);
            display.setCursor(0,25);
            display.print(String(pipe[0]));
            display.setCursor(0,57);
            display.print(String(pipe[1]));
            display.setCursor(128-18,25);
            display.print(String(pipe[2]));
            display.setCursor(128-18,57);
            display.print(String(pipe[3]));
            
            
            int gg = rand()%speed*30;
            if(gg < 300 && lastAdd+(speed*40)<millis()){
                int gh = rand()%4;
                if(pipe[gh]==0){
                    pipe[gh]=1;
                    lastAdd = millis();
                }
            }
            for(int a = 0; a < 4; a++){
                if(pipe[0] != 0){
                    display.drawLine(0,15-a,(32*pipe[0])/100,15+(8*pipe[0])/100-a,WHITE);
                }
                if(pipe[1] != 0){
                    display.drawLine(0,47-a,(32*pipe[1])/100,47+(8*pipe[1])/100-a,WHITE);
                   
                }
                if(pipe[2] != 0){
                    display.drawLine(128,15-a,128-(32*pipe[2])/100,15+(8*pipe[2])/100-a,WHITE);
                }
                   
                if(pipe[3] != 0){
                    display.drawLine(128,47-a,128-(32*pipe[3])/100,47+(8*pipe[3])/100-a,WHITE);
                }
            }
            
            for(int a = 0; a < 4; a++){
                if(pipe[a]!=0){
                    if(pipeTime[a]+speed<millis()){
                        pipe[a]+=(3+score/10);
                        if(pipe[a] > 100){
                            if(actPos-1 != a){
                                alive = 0;
                            }
                            pipe[a] = 0;
                            score++;
                        }
                        pipeTime[a] = millis();
                    }
                }
            }
        
        
            if(actPos == 1){
                display.drawLine(34,25,34,12,WHITE);
            }
            if(actPos == 2){
                display.drawLine(34,57,34,44,WHITE);
            }
            if(actPos == 3){
                display.drawLine(94,25,94,12,WHITE);
            }
            if(actPos == 4){
                display.drawLine(94,57,94,44,WHITE);
            }
            
            joy.scan();
            if(joy.state=="right"){
                if(actPos == 1)
                    actPos = 3;
                if(actPos == 2)
                    actPos = 4;
            }
            if(joy.state=="left"){
                if(actPos == 3)
                    actPos = 1;
                if(actPos == 4)
                    actPos = 2;
            }
            if(joy.state=="up"){
                if(actPos == 2)
                    actPos = 1;
                if(actPos == 4)
                    actPos = 3;
            }
            if(joy.state=="down"){
                if(actPos == 1)
                    actPos = 2;
                if(actPos == 3)
                    actPos = 4;
            }
            
            display.display();
            
        }
};

factoryCatcher factoryCatch;

String display_time(){
    DateTime now = rtc.now();
    String hour_temp = String(now.hour());
    if(hour_temp.length()<2)
        hour_temp = "0"+String(now.hour());
    String minute_temp = String(now.minute());
    if(minute_temp.length()<2)
        minute_temp = "0"+String(now.minute());
    String second_temp = String(now.second());
    if(second_temp.length()<2)
        second_temp = "0"+String(now.second());
    return hour_temp+":"+minute_temp+":"+second_temp;
}



void binsearch(int&i, int&j, String disp, bool big){
  int m;
  while(j - i > 0){
    m = (i+j)/2;
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.print(disp);
    display.setCursor(0,54);
    display.print("<" + String(i) + ";" + String(m) + ">");
    if(!big)
      display.setCursor(80,54);
    else
      display.setCursor(60,54);
    display.print("<" + String(m+1) + ";" + String(j) + ">");
    display.display();
    
    joy.scan();
    if(joy.state=="right"){
        i = m+1;
        while(joy.state=="right"){joy.scan();}
    }
    if(joy.state=="left"){
        j = m;
        while(joy.state=="left"){joy.scan();}
    }
   
  }
}

void mainScreen(){ //actLoc = 1
    display.clearDisplay();   // clears the screen and buffer
    display.setTextColor(WHITE,BLACK);
    display.drawLine(0,15,128,15,WHITE);
    display.drawLine(0,49,128,49,WHITE);
    display.drawLine(millis()%128,15,millis()%128+5,15,BLACK);
    display.drawLine(millis()%128,49,millis()%128+5,49,BLACK);
    display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
    display.setCursor(15,25);       // 128,64 pixels
    display.print(display_time());
    display.setCursor(0,56);
    display.setTextSize(1);
    
    sleepTime = tempSleep;
    
    String remainingTime = String(sleepTime-(millis()-actMillis));
    while(remainingTime.length()<10){
        remainingTime = "0" + remainingTime;
    }
    display.print(remainingTime);
    display.print("//SKL2");
    
    display.setCursor(0,0);
    
    if(WiFiMode == 1){
        display.print("WIFI:");
        if(WiFi.ready()){
            display.print(String(WiFi.SSID()));
        }
        else{
            if(WiFi.connecting()){
                display.print("Conncecting");
            }
        }
    }
    if(WiFiMode == 0){
        display.print("WiFi OFF");
    }
    
    display.display();
    
    
    
    joy.scan();
    if(joy.state!="none"){
        actMillis = millis();
    }
    joy.scan();
    while(joy.state=="click"){
        joy.scan();
        loc = 2;
    }

    if(millis()-actMillis>sleepTime){
        Particle.disconnect();
        display.ssd1306_command(SSD1306_DISPLAYOFF);
    
        display.display();

        System.sleep(D6, FALLING, 30000000);
        joy.scan();
        while(joy.state == "click"){joy.scan();}
        
        display.ssd1306_command(SSD1306_DISPLAYON);
       
        display.display();
        
        actMillis = millis();
        
        
    }
}

void menu(){ //id=2
    joy.scan();
    display.clearDisplay();
    display.drawLine(0,15,128,15,WHITE);
    display.drawLine(0,49,128,49,WHITE);
    display.drawLine(millis()%128,15,millis()%128+5,15,BLACK);
    display.drawLine(millis()%128,49,millis()%128+5,49,BLACK);
    display.setCursor(52,0);
    display.setTextSize(1);
    display.print("MENU");
    display.print(" " + String(actLoc));
    
    
    if(joy.state=="right"){
        actLoc++;
        while(joy.state=="right"){joy.scan();}
        if(actLoc >= numOfApps+1){
            actLoc = 1;
        }
    }
    if(joy.state=="left"){
        actLoc--;
        while(joy.state=="left"){joy.scan();}
        if(actLoc <= 0){
            actLoc = numOfApps;
        }
    }
    
    joy.scan();
    
    
    if(actLoc==1){
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(37,25);       // 128,64 pixels
        display.print("Clock");
        display.setCursor(0,56);
        display.setTextSize(1);
        display.print("FactoryC");
        display.setCursor(95,56);
        display.print("WiFi");
        
        if(joy.state=="click"){
            loc = 1;
            while(joy.state=="click"){joy.scan();}
        }
    }
    
    if(actLoc==2){
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(42,25);       // 128,64 pixels
        display.print("WiFi");
        display.setCursor(0,56);
        display.setTextSize(1);
        display.print("Clock");
        display.setCursor(73,56);
        display.print("Set Time");
        if(joy.state=="click"){
            loc = 3;
            while(joy.state=="click"){joy.scan();}
        }
    }
    
    if(actLoc==3){
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(15,25);       // 128,64 pixels
        display.print("Set Time");
        display.setCursor(0,56);
        display.setTextSize(1);
        display.print("WiFi");
        display.setCursor(90,56);
        display.print("Colors");
        if(joy.state=="click"){
            loc = 4;
            while(joy.state=="click"){joy.scan();}
        }
    }
    
    if(actLoc==4){
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(32,25);       // 128,64 pixels
        display.print("Colors");
        display.setCursor(0,56);
        display.setTextSize(1);
        display.print("Set Time");
        display.setCursor(65,56);
        display.print("Flashlight");
        if(joy.state=="click"){
            loc = 5;
            while(joy.state=="click"){joy.scan();}
        }
    }
    if(actLoc==5){
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(3,25);       // 128,64 pixels
        display.print("Flashlight");
        display.setCursor(0,56);
        display.setTextSize(1);
        display.print("Colors");
        display.setCursor(73,56);
        display.print("RGB Show");
        if(joy.state=="click"){
            loc = 6;
            while(joy.state=="click"){joy.scan();}
        }
    }
    if(actLoc==6){
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(17,25);       // 128,64 pixels
        display.print("RGB Show");
        display.setCursor(0,56);
        display.setTextSize(1);
        display.print("Flashlight");
        display.setCursor(67,56);
        display.print("Screensvr");
        if(joy.state=="click"){
            loc = 7;
            while(joy.state=="click"){joy.scan();}
        }
    }
    if(actLoc==7){
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(12,25);       // 128,64 pixels
        display.print("Screensvr");
        display.setCursor(0,56);
        display.setTextSize(1);
        display.print("RGB Show");
        display.setCursor(70,56);
        display.print("FactoryC");
        if(joy.state=="click"){
            loc = 8;
            while(joy.state=="click"){joy.scan();}
        }
    }
    if(actLoc==8){
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(17,25);       // 128,64 pixels
        display.print("FactoryC");
        display.setCursor(0,56);
        display.setTextSize(1);
        display.print("Screensvr");
        display.setCursor(95,56);
        display.print("Clock");
        if(joy.state=="click"){
            loc = 9;
            while(joy.state=="click"){joy.scan();}
        }
    }
    
    
    display.display();
    
}

void wifi(){ //actLoc = 2
    if(!WiFiMode){
        tempSleep = sleepTime;
        sleepTime = 20000;
        
        RGB.control(false);
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(2,25);
        display.print("Connecting");
        display.setTextSize(1);
        display.setCursor(5,56);
        display.print("Please be patient...");
        display.display();
        Particle.connect();
        delay(400);
        actMillis = millis();
        WiFiMode = 1;
        loc = 1;
    }
    else{
        RGB.control(false);
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(10,25);
        display.print("WiFi OFF");
        display.setTextSize(1);
        display.setCursor(5,56);
        display.print("Please be patient...");
        display.display();
        Particle.disconnect();
        delay(400);
        actMillis = millis();
        WiFiMode = 0;
        loc = 1;
    }
    actMillis = millis();
    RGB.control(true);
    
    
}

void setTime(){
    
    
    int hTemp = 0;
    int mTemp = 0;
    int actChange = 0; //0 - hours, 1 - minutes
    
    joy.scan();
    while(joy.state != "click"){
        
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setCursor(35,0);
        display.print("Set Time:");
        display.setCursor(12,56);
        display.print("Click when ready");
                
        String hour_temp = String(hTemp);
        if(hour_temp.length()<2)
            hour_temp = "0"+String(hTemp);
        String minute_temp = String(mTemp);
        if(minute_temp.length()<2)
            minute_temp = "0"+String(mTemp);
            
            
        int modifier = 0;
        joy.scan();
        
        if(joy.state == "up"){
            modifier++;
        }
        if(joy.state == "down"){
            modifier--;
        }
        
        if(joy.state == "right" || joy.state == "left"){
            if(actChange == 0)
                actChange = 1;
            else
                actChange = 0;
            while(joy.state != "none"){joy.scan();}
        }
        
        if(actChange == 0){
            hTemp += modifier;
            hTemp %=24;
        }
        if(actChange == 1){
            mTemp += modifier;
            mTemp %=60;
        }
        
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(33,25);       // 128,64 pixels
        if(actChange == 0){
            display.setTextColor(BLACK,WHITE);
            display.print(hour_temp);
            display.setTextColor(WHITE);
            display.print(":");
            display.print(minute_temp);
        }
        if(actChange == 1){
            display.setTextColor(WHITE);
            display.print(hour_temp);
            display.print(":");
            display.setTextColor(BLACK,WHITE);
            display.print(minute_temp);
        }
        display.display();
        
    }
    
    rtc.adjust(DateTime(2001, 12, 18, hTemp, mTemp, 0));
    
    loc = 1;
}

void colorScheme(){
    joy.scan();
    display.clearDisplay();
    display.setCursor(25,0);
    display.setTextSize(1);
    display.print("Color Schemes");

    int numOfSchemes = 6;
    
    joy.scan();
    
    if(joy.state=="right"){
        actScheme++;
        while(joy.state=="right"){joy.scan();}
        if(actScheme >= numOfSchemes+1){
            actScheme = 1;
        }
    }
    if(joy.state=="left"){
        actScheme--;
        while(joy.state=="left"){joy.scan();}
        if(actScheme <= 0){
            actScheme = numOfSchemes;
        }
    }
    
    if(actScheme==1){
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(28,25);       // 128,64 pixels
        display.print("Pinkie");
        display.setCursor(0,56);
        display.setTextSize(1);
        display.print("Blazing");
        display.setCursor(92,56);
        display.print("Trench");
        ledR = 255;
        ledG = 0;
        ledB = 255;
        clickLedR = 255;
        clickLedG = 0;
        clickLedB = 0;
        
    }
    
    if(actScheme==2){
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(27,25);       // 128,64 pixels
        display.print("Trench");
        display.setCursor(0,56);
        display.setTextSize(1);
        display.print("Pinkie");
        display.setCursor(92,56);
        display.print("White");

        ledR = 150;
        ledG = 120;
        ledB = 0;
        clickLedR = 255;
        clickLedG = 255;
        clickLedB = 0;

    }
    
    if(actScheme==3){
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(35,25);       // 128,64 pixels
        display.print("White");
        display.setCursor(0,56);
        display.setTextSize(1);
        display.print("Trench");
        display.setCursor(73,56);
        display.print("Cyberpunk");

        ledR = 0;
        ledG = 0;
        ledB = 0;
        clickLedR = 200;
        clickLedG = 200;
        clickLedB = 255;

    }
    
    if(actScheme==4){
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(11,25);       // 128,64 pixels
        display.print("Cyberpunk");
        display.setCursor(0,56);
        display.setTextSize(1);
        display.print("White Click");
        display.setCursor(92,56);
        display.print("Galaxy");
     
        ledR = 0;
        ledG = 255;
        ledB = 0;
        clickLedR = 0;
        clickLedG = 225;
        clickLedB = 200;
 
    }
    
     if(actScheme==5){
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(28,25);       // 128,64 pixels
        display.print("Galaxy");
        display.setCursor(0,56);
        display.setTextSize(1);
        display.print("Cyberpunk");
        display.setCursor(86,56);
        display.print("Blazing");
     
        ledR = 30;
        ledG = 30;
        ledB = 255;
        clickLedR = 0;
        clickLedG = 0;
        clickLedB = 255;
    }
    
    if(actScheme==6){
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(23,25);       // 128,64 pixels
        display.print("Blazing");
        display.setCursor(0,56);
        display.setTextSize(1);
        display.print("Galaxy");
        display.setCursor(92,56);
        display.print("Pinkie");
     
        ledR = 255;
        ledG = 0;
        ledB = 0;
        clickLedR = 255;
        clickLedG = 100;
        clickLedB = 0;
    }
    
    if(joy.state=="click"){
        loc = 1;
        while(joy.state=="click"){joy.scan();}
    }
    
    display.display();
}

void flashlight(){
    RGB.color(255,255,255);
    digitalWrite(7,HIGH);
    for(int i = 0; i <= 64; i++){
        display.drawLine(0,i,128,i,WHITE);
    }
    display.display();
    joy.scan();
    if(joy.state=="click"){
        display.clearDisplay();
        display.display();
        digitalWrite(7,LOW);
        loc = 1;
        while(joy.state=="click"){joy.scan();}
    }
}

void rgbShow(){
    
    int actChange = 0; //0 - R, 1 - G, 2 - B
    
    int rTemp = 0;
    int gTemp = 0;
    int bTemp = 0;
    
    joy.scan();
    while(joy.state != "click"){
        
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setCursor(35,0);
        display.print("RGB Show:");
            
            
        int modifier = 0;
        joy.scan();
        
        if(joy.state == "up"){
            actChange--;
            if(actChange < 0)
                actChange = 0;
            while(joy.state == "up"){joy.scan();}
        }
        if(joy.state == "down"){
            actChange++;
            if(actChange > 2)
                actChange = 2;
            while(joy.state == "down"){joy.scan();}
        }
        
        if(joy.state == "right"){
            modifier+=3;
        }
        if(joy.state == "left"){
            modifier-=3;    
        }
        
        if(actChange == 0){
            rTemp += modifier;
            rTemp %=256;
        }
        if(actChange == 1){
            gTemp += modifier;
            gTemp %=256;
        }
        if(actChange == 2){
            bTemp += modifier;
            bTemp %=256;
        }
        
        display.setTextSize(2);           // from 1-9 sensible actually can be bigger but useless
        display.setCursor(0,15);       // 128,64 pixels
        if(actChange == 0){
            display.setTextColor(WHITE);
            display.print("R:");
            display.setTextColor(BLACK,WHITE);
            display.println(String(rTemp));
            display.setTextColor(WHITE);
            display.print("G:");
            display.println(String(gTemp));
            display.print("B:");
            display.println(String(bTemp));
        }
        if(actChange == 1){
            display.setTextColor(WHITE);
            display.print("R:");
            
            display.println(String(rTemp));
            
            display.print("G:");
            display.setTextColor(BLACK,WHITE);
            display.println(String(gTemp));
            display.setTextColor(WHITE);
            display.print("B:");
            display.println(String(bTemp));
        }
        if(actChange == 2){
            display.setTextColor(WHITE);
            display.print("R:");
            display.println(String(rTemp));
            display.print("G:");
            display.println(String(gTemp));
            display.print("B:");
            display.setTextColor(BLACK,WHITE);
            display.println(String(bTemp));
            display.setTextColor(WHITE);
        }
        ledR = rTemp;
        ledG = gTemp;
        ledB = bTemp;
        clickLedR = rTemp;
        clickLedG = gTemp;
        clickLedB = bTemp;
        display.display();
    }
    
    if(joy.state=="click"){
        display.clearDisplay();
        display.display();
        loc = 1;
        while(joy.state=="click"){joy.scan();}
    }
    
}

void screenSaver(){
    display.clearDisplay();
    
    
    //display.setTextSize(2);
    
    int screenTime = 15000;
    
    while(joy.state!="click"){
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setCursor(24,0);
        display.print("Screensaver:");
        display.setCursor(11,56);
        display.print("Click when ready");
        display.setCursor(39,25);
        display.setTextSize(2);
        display.setTextColor(BLACK,WHITE);
        display.print(String(screenTime/1000));
        display.setTextColor(WHITE);
        display.print("s");
        joy.scan();
        if(joy.state=="left"){
            screenTime -= 1000;
            if(screenTime < 2000)
                screenTime = 2000;
        }
        if(joy.state=="right"){
            screenTime += 1000;
            if(screenTime > 60000){
                screenTime = 60000;
            }
        }
        display.display();
    }
    sleepTime = screenTime;
    tempSleep = screenTime;
    
    if(joy.state=="click"){
        display.clearDisplay();
        display.display();
        loc = 1;
        while(joy.state=="click"){joy.scan();}
    }
}

void factoryGame(){
    factoryCatch.resetGame();
    while(joy.state!="click" && factoryCatch.alive){
        factoryCatch.game();
        joy.scan();
    }
    loc = 1;
    while(joy.state=="click"){joy.scan();}
}

void setup () {
    Serial.begin(9600);
    
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
    display.ssd1306_command(SSD1306_DISPLAYON);
    display.clearDisplay();
    display.display();
    
    actMillis = millis();
    
    RGB.control(true);
    
    ledR = 255;
    ledG = 0;
    ledB = 255;
    clickLedR = 255;
    clickLedG = 0;
    clickLedB = 0;
    
    pinMode(2,INPUT);
    pinMode(3,INPUT);
    pinMode(4,INPUT);
    pinMode(5,INPUT);
    pinMode(6,INPUT);
    pinMode(7,OUTPUT);
    
    randomSeed(millis());
}


void loop () {
    
    RGB.color(ledR, ledG, ledB);
    
    if(loc == 1)
        mainScreen();
    if(loc == 2)
        menu();
    if(loc == 3)
        wifi();
    if(loc == 4)
        setTime();
    if(loc == 5)
        colorScheme();
    if(loc == 6)
        flashlight();
    if(loc == 7)
        rgbShow();
    if(loc == 8)
        screenSaver();
    if(loc == 9)
        factoryGame();
    
}
