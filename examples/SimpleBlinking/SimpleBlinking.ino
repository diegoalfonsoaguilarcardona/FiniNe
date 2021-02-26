/*
 * About this file: FiniNeBlink.ino file.
 *                  Arduino FiniNe (Fi-nite state machi-Ne) blinking example
 *
 * Author:  Diego A. Aguilar C. – TaIO Systems SAS
 * Version: 1.0
 * Date: Feb 24, 2021
 */

#include "FiniNe.h"

/*
 * This is the state machine diagram:
 * 
 * +------------+       +------------+  Switch      +--------------+
 * |            |       |            +------------->+              |
 * |    OFF     +------>+  Led Off   |              |    Led On    |
 * |            | Start |            +<-------------+              |
 * +------------+       +------------+  Switch      +--------------+
 * 
 */


// States 
typedef enum
{
    FSM1_STATE_OFF,
    FSM1_STATE_LED_ON,
    FSM1_STATE_LED_OFF,
} FSM1_STATES;


// Events
typedef enum
{
  FSM1_EVT_START,
  FSM1_EVT_SWITCH,
} FSM1_EVENTS;

// Callback functions
int Fsm1OnStart(){
  pinMode(LED_BUILTIN, OUTPUT);
  return FSM_SUCCESSFUL_RETURN;
}

int Fsm1OnSwitchLedOn(){
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)  
  return FSM_SUCCESSFUL_RETURN;
}

int Fsm1OnSwitchLedOff(){
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  return FSM_SUCCESSFUL_RETURN;
}

// Table implementation for FSM1
FiniNeTable handlerTable1[] = {
  //.state             .event           .handler_funct      .next state
  {FSM1_STATE_OFF,     FSM1_EVT_START,  Fsm1OnStart,        FSM1_STATE_LED_OFF},
  {FSM1_STATE_LED_OFF, FSM1_EVT_SWITCH, Fsm1OnSwitchLedOn,  FSM1_STATE_LED_ON},
  {FSM1_STATE_LED_ON,  FSM1_EVT_SWITCH, Fsm1OnSwitchLedOff, FSM1_STATE_LED_OFF},
};

// FSM Object
FiniNe Fsm1;

void setup() {
  FsmDefInit();
  Fsm1.begin(1, FSM1_STATE_OFF, handlerTable1, 3); //The first argument is the Id of the State machine (starting from 1)
  Fsm1.setNextEvent(FSM1_EVT_START);
}

void loop() {
    FsmDefMainLoop();
    delay(1000);
    Fsm1.setNextEvent(FSM1_EVT_SWITCH);
}