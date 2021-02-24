/*
 * About this file: FiniNe.h file of the project.
 *                  Arduino parser of the library
 *
 * Author:  Diego A. Aguilar C. – TaIO Systems SAS
 * Version: 1.0
 * Date: Feb 24, 2021
 */


#ifndef FiniNe_h
#define FiniNe_h

#include "FsmDefinition.h"

class FiniNe {
  public:
    FiniNe();
    FSM_DEF_RETURN_OP_E begin(UINTN fsmId, UINTN initialState, FSM_DEF_TABLE_T table[], UINTN tableSize, FSM_EVENT_MANAGER_LIST_T eventManagerList[], UINTN eventManagerListSize);
    FSM_DEF_RETURN_OP_E setNextEvent(UINTN eventId);

  private:
    //FSM_DEF_TABLE_T* fsmTable;
    FSM_DEF_CONTEXT_T fsmContext;
    UINTN fsmId;
};

#endif
