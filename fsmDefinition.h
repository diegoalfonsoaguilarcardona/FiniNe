#ifndef __FSM_DEF_H_
#define __FSM_DEF_H_

#if defined(__cplusplus)
extern "C"
{
#endif

#include "FsmDefErrorTypes.h"
#include "UefiTypes.h"


#define NOT_USE_FSM_TED


/** Define a NULL Event*/
#define FSM_DEF_EVENT_NULL    0xFFFFFFFF

/** Used to indicate that one event is dispatched for all states*/
#define FSM_DEF_EVT_FOR_ALL   0xFFFFFFFF

/** Used to define the size of events buffer in a FSM*/
#define FSM_EVENTS_BUFFER_MAX 5

/** Used to define the size of FSM context buffer*/
#define FSM_CONTEXT_BUFFER_MAX  15

/** Used to define the Timed Event Dispatcher buffer size*/
#define FSM_TIMER_DISPATCH_BUFF_MAX 10

/***************************************************************/

#define FSM_LOG_LEVEL_0 0 /**< Define FSM_LOG_LEVEL_0 as 0, NO FSM LOG */
#define FSM_LOG_LEVEL_1 1 /**< Define FSM_LOG_LEVEL_1 as 1, Used to FSM LOG with errors*/
#define FSM_LOG_LEVEL_2 2 /**< Define FSM_LOG_LEVEL_2 as 2, Used to FSM LOG with errors, Warnings*/
#define FSM_LOG_LEVEL_3 3 /**< Define FSM_LOG_LEVEL_3 as 3, Used to FSM LOG with errors, warnings, Info*/
#define FSM_LOG_LEVEL_4 4 /**< Define FSM_LOG_LEVEL_4 as 4, Used to FSM LOG with errors, warnings, Info, Debug*/

#define FSM_LOG_LEVEL_DEFAULT FSM_LOG_LEVEL_0 /**< Default Log level for FSM definition*/

#ifndef FSM_LOG_LEVEL
#define FSM_LOG_LEVEL FSM_LOG_LEVEL_DEFAULT
#endif

#if FSM_LOG_LEVEL == FSM_LOG_LEVEL_1

#define FSM_LOG_ERROR   HAL_FSM_DEF_LOG_ERROR
#define FSM_LOG_WARNING(...)
#define FSM_LOG_INFO(...)
#define FSM_LOG_DEBUG(...)
#elif FSM_LOG_LEVEL == FSM_LOG_LEVEL_2
#define FSM_LOG_ERROR   HAL_FSM_DEF_LOG_ERROR
#define FSM_LOG_WARNING   HAL_FSM_DEF_LOG_WARNING
#define FSM_LOG_INFO(...)
#define FSM_LOG_DEBUG(...)
#elif FSM_LOG_LEVEL == FSM_LOG_LEVEL_3
#define FSM_LOG_ERROR   HAL_FSM_DEF_LOG_ERROR
#define FSM_LOG_WARNING   HAL_FSM_DEF_LOG_WARNING
#define FSM_LOG_INFO    HAL_FSM_DEF_LOG_INFO
#define FSM_LOG_DEBUG(...)
#elif FSM_LOG_LEVEL == FSM_LOG_LEVEL_4
#define FSM_LOG_ERROR   HAL_FSM_DEF_LOG_ERROR
#define FSM_LOG_WARNING   HAL_FSM_DEF_LOG_WARNING
#define FSM_LOG_INFO    HAL_FSM_DEF_LOG_INFO
#define FSM_LOG_DEBUG   HAL_FSM_DEF_LOG_DEBUG
#else
#define FSM_LOG_ERROR(...)
#define FSM_LOG_WARNING(...)
#define FSM_LOG_INFO(...)
#define FSM_LOG_DEBUG(...)
#endif

/* Define NULL if it is not defined*/
#ifndef NULL
#define NULL 0
#endif

#define FSM_SUCCESSFUL_RETURN 0 /**< Successful exit status.*/
#define FSM_UNSUCCESSFUL_RETURN 1 /**< Failing exit status.*/

#define FSM_TIMED_EVT_DISPATCHER_DISABLE  0x00 /**< Used to identify a Timer Event Dispatcher as timer off*/
#define FSM_TIMED_EVT_DISPATCHER_ONE_SHOT 0x01 /**< Used to identify a Timer Event Dispatcher as one shot type timer.*/
#define FSM_TIMED_EVT_DISPATCHER_PERIODIC 0x02 /**< Used to identify a Timer Event Dispatcher as periodic timer.*/

/**
 * @brief Used to define the struct of the FSM context.
 */
typedef struct FSM_DEF_CONTEXT_S FSM_DEF_CONTEXT_T;

/**
 * @brief Structure used to define the relationship table between states
 * and events of a specific FSM.
 */
typedef struct
{

  UINTN state; /**< Current state of the FSM.*/
  UINTN event; /**< Event to be dispatched.*/
  INTN (*handlerFunct) (); /**< Function to run when event is dispatched.*/
  UINTN nextState; /**< State of the FSM after the event is dispatched.*/
} FSM_DEF_TABLE_T;

/**
 * @brief Structure used to initialize the relationship table in a FSM context.
 */
typedef struct
{
  UINTN tableSize; /**< Size of the FSM context table*/
  FSM_DEF_TABLE_T *pTable; /**< Pointer of the FSM context table*/
} FSM_DEF_TABLE_HND_T;

/**
 * @brief Structure used to define the event manager list of a FSM.
 */
typedef struct
{
  UINTN fsmUidToSubscribe; /**< FSM UID to be subscribed.*/
  UINTN evtToSubscribe; /**< event of the FSM to be subscribed*/
} FSM_EVENT_MANAGER_LIST_T;

/**
 * @brief Structure used to initialize the event manager list in a FSM context.
 */
typedef struct
{
  UINTN listSize; /**< Size of the event manager list*/
  FSM_EVENT_MANAGER_LIST_T *pList; /**< Pointer oF the event manager list*/
} FSM_DEF_EVT_MANAGER_T;

/**
 * @brief Structure used to initialize the event buffer in a FSM context.
 */
typedef struct
{
  UINTN eventIndex; /**< Event buffer index used to know which is the next event to be dispatched */
  UINTN eventBuff[FSM_EVENTS_BUFFER_MAX]; /**< Event Buffer*/
} FSM_DEF_EVT_HND_T;

/**
 * @brief Structure used to initialize a context of a FSM.
 */
struct FSM_DEF_CONTEXT_S
{
  UINTN fsmUid; /**< UID of the FSM*/
  FSM_DEF_TABLE_HND_T tableHnd; /**< FSM context table */
  UINTN currentState; /**< Current state (initial state)*/
  FSM_DEF_EVT_HND_T eventHnd; /**< Event Buffer to manage the events to be dispatched*/
  FSM_DEF_EVT_MANAGER_T eventManagerHnd; /**< Event manager table to manage the events from other FSMs*/
};

/**
 * @brief Function for initializing FSM definition.
 * @return FSM_DEF_RETURN_OP_E
 */
FSM_DEF_RETURN_OP_E
FsmDefInit ();

/**
 * @brief Function for creating a new FSM
 * @param _newFsmContext  Context of the new FSM
 * @return FSM_DEF_RETURN_OP_E
 */
FSM_DEF_RETURN_OP_E
FsmDefCreate (
  FSM_DEF_CONTEXT_T * _newFsmContext);

/**
 * @brief Function for running in a main loop.
 * In this function all created FSMs verify if there are pending events and dispatch its.
 * @return FSM_DEF_RETURN_OP_E
 */
FSM_DEF_RETURN_OP_E
FsmDefMainLoop ();

/**
 * @brief Function for handling the events of all created FSM.
 * @param _evt      Event to be dispatched.
 * @param _fsmContext FSM Context
 * @return FSM_DEF_RETURN_OP_E
 */
FSM_DEF_RETURN_OP_E
FsmDefHandler (
  UINTN _evt,
  FSM_DEF_CONTEXT_T * _fsmContext);

/**
 * @brief Function for handling the events of a lower FSM to a Higher FSM,
 * when Higher FSM subscribes to these.
 * @param _evt        Event to be dispatched.
 * @param _fsmContext   FSM Context
 * @return FSM_DEF_RETURN_OP_E
 */
FSM_DEF_RETURN_OP_E
FsmDefRunEventManager (
  UINTN _evt,
  FSM_DEF_CONTEXT_T * _fsmContext);

/**
 * @brief Function for dispatching  a new event of a FSM
 * @param _fsmUid   FSM UID.
 * @param _evt      Event to be dispatched.
 * @return FSM_DEF_RETURN_OP_E
 */
FSM_DEF_RETURN_OP_E
FsmDefSetNextEvent (
  UINTN _fsmUid,
  UINTN _evt);

/**
 * @brief Function for getting the state of a FSM.
 * @param _fsmUid Required FSM UID to get state
 * @return State of the FSM.
 */
UINTN
FsmDefGetState (
  UINTN _fsmUid);

/**
 * @brief Function for erasing all FSMs
 * @return FSM_DEF_RETURN_OP_E
 */
UINTN
FsmDefEraseFsmBuff ();

FSM_DEF_CONTEXT_T * FsmDefUidExist(
  UINTN _fsmUid);

FSM_DEF_RETURN_OP_E FsmDefConcatenateFsmUidToFsmTable(
  UINTN _fsmUid,
  FSM_DEF_TABLE_HND_T * _table
  );

BOOL FsmDefIsEvtInTable(
  UINTN _evt,
  FSM_DEF_CONTEXT_T * _fsmContext);

BOOL FsmDefIsStateInTable(
  UINTN _state,
  FSM_DEF_CONTEXT_T * _fsmContext);

FSM_DEF_RETURN_OP_E
FsmGetLastEvent(
  UINTN _fsmUid,
  UINTN * _lastEvent
  );

FSM_DEF_RETURN_OP_E
FsmDefPrintEvtBuff(
  UINTN _fsmUid);

FSM_DEF_RETURN_OP_E
FsmDefIsTherePendingEvents(BOOL *_FlagOcurs);

#if defined(__cplusplus)
}
#endif

#endif /* __FSM_DEF_H_ */
