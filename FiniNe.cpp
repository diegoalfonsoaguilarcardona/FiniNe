/*
 * About this file: FiniNe.cpp file of the project.
 *                  Arduino parser of the library
 *
 * Author:  Diego A. Aguilar C. – TaIO Systems SAS
 * Version: 1.0
 * Date: Feb 24, 2021
 */


#include "FiniNe.h"

FiniNe::FiniNe()
{
}

FSM_DEF_RETURN_OP_E FiniNe::begin(UINTN fsmId, UINTN initialState, FSM_DEF_TABLE_T table[], UINTN tableSize, FSM_EVENT_MANAGER_LIST_T eventManagerList[], UINTN eventManagerListSize)
{
  this->fsmId = fsmId << 16;
  fsmContext.fsmUid = this->fsmId;
  // Implement context for this FSM
  fsmContext.currentState = initialState;
  fsmContext.eventHnd.eventIndex=   0;
  fsmContext.eventHnd.eventBuff[0] = FSM_DEF_EVENT_NULL;
  fsmContext.tableHnd.tableSize =     tableSize;
  fsmContext.tableHnd.pTable = (FSM_DEF_TABLE_T *) table;
  fsmContext.eventManagerHnd.listSize = eventManagerListSize;
  fsmContext.eventManagerHnd.pList =  (FSM_EVENT_MANAGER_LIST_T*)eventManagerList;
  return FsmDefCreate(&fsmContext);
}

FSM_DEF_RETURN_OP_E FiniNe::begin(UINTN fsmId, UINTN initialState, FSM_DEF_TABLE_T table[], UINTN tableSize)
{
  this->fsmId = fsmId << 16;
  fsmContext.fsmUid = this->fsmId;
  // Implement context for this FSM
  fsmContext.currentState = initialState;
  fsmContext.eventHnd.eventIndex=   0;
  fsmContext.eventHnd.eventBuff[0] = FSM_DEF_EVENT_NULL;
  fsmContext.tableHnd.tableSize =     tableSize;
  fsmContext.tableHnd.pTable = (FSM_DEF_TABLE_T *) table;
  fsmContext.eventManagerHnd.listSize = 0;
  fsmContext.eventManagerHnd.pList =  (FSM_EVENT_MANAGER_LIST_T*) &emptyEventManager;
  return FsmDefCreate(&fsmContext);
}

FSM_DEF_RETURN_OP_E FiniNe::setNextEvent(UINTN eventId){
  return FsmDefSetNextEvent(fsmId, eventId);
}
