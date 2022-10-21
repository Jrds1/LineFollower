#include "MicroBit.h"
#include <cstdio>

MicroBit uBit;

int speed = 10, dist;
bool moveFlag = false,leftGS, rightGS, edge;
unsigned long Time = 0, lastTime = 0;
uint8_t buf[3];

/*
Button A and Button Bhandler
*/
void onButtonA(MicroBitEvent e){
    //if a button pushed close program
    if (e.value == MICROBIT_BUTTON_EVT_CLICK){
        exit(0);  
    } 
}
void onButtonB(MicroBitEvent e){
    //if a button pushed close program
    if (e.value == MICROBIT_BUTTON_EVT_CLICK){
        moveFlag = !moveFlag;   
    }
}

/*
Motor driving functions
*/
void forward(){
    buf[0] = 0x00;  //motor id
    buf[1] = 0x00;  // 0 forward,  1 backward
    buf[2] = 0x15;  //speed
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
    buf[0] = 0x02;  //motor id
    uBit.i2c.write( 0x20, buf, 3); 
}
void backward(){
    buf[0] = 0x00;  //motor id
    buf[1] = 0x01;  // 0 forward,  1 backward
    buf[2] = 0x15;  //speed
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
    buf[0] = 0x02;  //motor id
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
}
void left(){
    buf[0] = 0x00;  //motor id
    buf[1] = 0x00;  // 0 forward,  1 backward
    buf[2] = 0x15;  //speed
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
    buf[0] = 0x02;  //motor id
    buf[2] = 0x00;  //speed
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.

}
void right(){
    buf[0] = 0x02;  //motor id
    buf[1] = 0x00;  // 0 forward,  1 backward
    buf[2] = 0x15;  //speed
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
    buf[0] = 0x00;  //motor id
    buf[2] = 0x00;  //speed
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
}
void stop(){
    buf[0] = 0x02;  //motor id
    buf[1] = 0x00;  // 0 forward,  1 backward
    buf[2] = 0x00;  //speed
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
    buf[0] = 0x00;  //motor id
    buf[1] = 0x00;  // 0 forward,  1 backward
    buf[2] = 0x00;  //speed
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
}


/*
Read IR sensor
*/
void readGS(){
    //Check if gray sensor pins are high or low
    if(uBit.io.P13.getDigitalValue()==false){
        leftGS = true;
    }
    else{
        leftGS = false;
    }
    if(uBit.io.P14.getDigitalValue()==false){
        rightGS = true;
    }
    else{
        rightGS = false;
    }
}

/*
Read US sensor
*/
void readUS(){
    int d = 0;
    if (uBit.io.P2.getDigitalValue() == 0){
        uBit.io.P1.setDigitalValue(0); 
        uBit.io.P1.setDigitalValue(1); 
        uBit.sleep(20);
        uBit.io.P1.setDigitalValue(0); 
        d = uBit.io.P2.getPulseUs(70000);
    }
    else {
        uBit.io.P1.setDigitalValue(1); 
        uBit.io.P1.setDigitalValue(0); 
        uBit.sleep(20);
        uBit.io.P1.setDigitalValue(1);
        d = uBit.io.P2.getPulseUs(70000);
    }
    dist = d / 59;
    uBit.display.scroll(dist);
}

/*
Functions to run robot
*/
void setup(){
    uBit.init();
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, onButtonA); // Listener for button A
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_EVT_ANY, onButtonB); // Listener for button B

    moveFlag = true;
}

int main(){
    while(1){
        unsigned long Time = system_timer_current_time_us();
        
        //Read sensors
        if(Time-lastTime >10){
            readGS();
        }
        
        //Set motor direction based on track readings
        if (leftGS == true && rightGS == true){
            forward();
        }
        else if (leftGS == true && rightGS == false){
            right();
        }
        else if (leftGS == false && rightGS == true){
            left();
        }
        else if (leftGS == false && rightGS == false){
            while(uBit.io.P13.getDigitalValue() == true && uBit.io.P14.getDigitalValue() == true){}
                backward();
            }
            edge = true;
        }
        else{
            stop();
        }
        uBit.sleep(100);

        if(edge == true){
            
        }
        lastTime = Time;
    }
}
