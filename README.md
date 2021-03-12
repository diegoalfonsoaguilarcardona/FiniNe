# FiniNe
Arduino version of the Fini-te state machi-Ne Library from [TaIO Systems](https://www.taiosystems.com)

## Instalation
To install this library copy the content of this repository in the *libraries* folder of your sketchbook (you can find or change the location of your sketchbook folder at File > Preferences > Sketchbook location).

For example, if you go to your *sketckbook/libraries* folder, you should have something like this:
```
.
├── FiniNe
│   ├── FiniNe.cpp
│   ├── FiniNe.h
│   ├── FsmDefErrorTypes.h
│   ├── FsmDefinition.c
│   ├── LICENSE
│   ├── README.md
│   ├── UefiTypes.h
│   ├── examples
│   │   └── SimpleBlinking
│   │       └── SimpleBlinking.ino
│   ├── fsmDefinition.h
│   └── keywords.txt
├── Servo
│   ├── README.adoc
.
.
.
└── readme.txt
```
Then start the Arduino Software (IDE), go to *Sketch > Include* Library. Verify that the **FiniNe** library is available in the list. Also, you can go to the *File > Examples > FiniNe* and open the **SimpleBlinking** example.

## How to use this library
The starting point can be the examples. Also, there are a series of articles about FiniNe and related topics in [TaIO Systems Blog](https://taiosystems.com/blog/)

Simply, start creating an **enum** with the desired states:
```c++
// States 
typedef enum
{
    FSM1_STATE_OFF,
    FSM1_STATE_LED_ON,
    FSM1_STATE_LED_OFF,
} FSM1_STATES;
```

Then create another **enum** with the system events:
```c++
// Events
typedef enum
{
  FSM1_EVT_START,
  FSM1_EVT_SWITCH,
} FSM1_EVENTS;
```
Define the callback functions for the specific transitions:
```c++
// Callback functions
int Fsm1OnStart(){
  pinMode(LED_BUILTIN, OUTPUT);
  return FSM_SUCCESSFUL_RETURN;
}

int Fsm1OnSwitchLedOn(){
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  ...
  ...
  ...
```
Implement the transition table:
```c++
// Table implementation for FSM1
FiniNeTable handlerTable1[] = {
  //.state             .event           .handler_funct      .next state
  {FSM1_STATE_OFF,     FSM1_EVT_START,  Fsm1OnStart,        FSM1_STATE_LED_OFF},
  {FSM1_STATE_LED_OFF, FSM1_EVT_SWITCH, Fsm1OnSwitchLedOn,  FSM1_STATE_LED_ON},
  {FSM1_STATE_LED_ON,  FSM1_EVT_SWITCH, Fsm1OnSwitchLedOff, FSM1_STATE_LED_OFF},
};
```
And define a FiniNe object:
```c++
// FSM Object
FiniNe Fsm1;
```
Inside the *setup* function, start the FiniNe system that can handle all the state machines (**FsmDefInit** function), begin the state machine with the transition table and generate the first event:
```c++
void setup() {
  FsmDefInit();
  Fsm1.begin(1, FSM1_STATE_OFF, handlerTable1, 3); //The first argument is the Id of the State machine (starting from 1)
  Fsm1.setNextEvent(FSM1_EVT_START);
}
```
Finally inside the *loop* function, call the FiniNe main loop function (**FsmDefMainLoop**) and generate the desired events.
```c++
void loop() {
    FsmDefMainLoop();
    delay(1000);
    Fsm1.setNextEvent(FSM1_EVT_SWITCH);
}
```




