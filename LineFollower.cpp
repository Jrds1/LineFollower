#include "MicroBit.h"
#include <cstdio>

MicroBit uBit;

int dist = 11;
bool moveFlag = false, leftGS = false, rightGS = false, inter=false, turn=false;
unsigned long Time = 0, lastTime = 0, edgeTimer;
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
    buf[2] = 0x20;  //speed
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
    buf[0] = 0x02;  //motor id
    uBit.i2c.write( 0x20, buf, 3); 
}
void backward(){
    buf[0] = 0x00;  //motor id
    buf[1] = 0x01;  // 0 forward,  1 backward
    buf[2] = 0x20;  //speed
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
    buf[0] = 0x02;  //motor id
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
}
void left(){
    buf[0] = 0x00;  //motor id
    buf[1] = 0x00;  // 0 forward,  1 backward
    buf[2] = 0x20;  //speed
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
    buf[0] = 0x02;  //motor id
    buf[1] = 0x01;  // 0 forward,  1 backward
    buf[2] = 0x20;  //speed
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.

}
void right(){
    buf[0] = 0x02;  //motor id
    buf[1] = 0x00;  // 0 forward,  1 backwaedgerd
    buf[2] = 0x20;  //speed
    uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
    buf[0] = 0x00;  //motor id
    buf[1] = 0x01;  // 0 forward,  1 backward
    buf[2] = 0x20;  //speed
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
Check for intersection or corner
*/
void checkInter(){
    edgeTimer = system_timer_current_time_us();
    while(uBit.io.P13.getDigitalValue() == true && uBit.io.P14.getDigitalValue() == true){
        forward();
        if(system_timer_current_time_us() - edgeTimer > 500000)
            break;

    }
    stop();
    if(system_timer_current_time_us() - edgeTimer < 500000){
        inter = true;
    }
    else {
        inter = false;
    }

    while(uBit.io.P13.getDigitalValue() != true && uBit.io.P14.getDigitalValue() != true){
        backward();
    }
}
/*
Read US sensor
*/
void readUS(){
    //send pulse
    int pulse = 0;
    uBit.io.P1.setDigitalValue(0); 
    uBit.sleep(1);
    uBit.io.P1.setDigitalValue(1); 
    uBit.sleep(10);
    uBit.io.P1.setDigitalValue(0); 

    //read pylse
    pulse = uBit.io.P2.getPulseUs(500 * 39);

    //change to cm
    dist = pulse / 59;
    //uBit.display.scroll(dist);
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
        //if(Time-lastTime >100){
            //readUS();
        //}
        if(Time-lastTime >10){
            readGS();
        }
        
        //Set motor direction based on track readings
        if(dist < 10){
            stop();
        }
    
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
            forward();
            turn = !turn;
            edgeTimer = system_timer_current_time_us();
            if(turn == true)
                while(uBit.io.P13.getDigitalValue() == true || uBit.io.P14.getDigitalValue() == true){
                    right();
                    if(system_timer_current_time_us() - edgeTimer > 750000)
                        break;
                
                }
            else if (turn == false)
                while(uBit.io.P13.getDigitalValue() == true || uBit.io.P14.getDigitalValue() == true){
                    left();
                    //if(system_timer_current_time_us() - edgeTimer > 500000)
                        //break;
                }
            forward();
        }
        
        uBit.sleep(100);
        lastTime = Time;
    } 
}  

