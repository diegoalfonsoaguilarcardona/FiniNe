/*
 * About this file: FsmDefinitionV20.c file of the project.
 *
 * Author:  Ricardo Esteven Lucano – TaIO Systems SAS
 * Version: 1.0
 * Date: Mar 8, 2018
 */

#include "FsmDefinition.h"


//! Used to store the pointer of the FSMs context.
STATIC FSM_DEF_CONTEXT_T * fsmContextBuff[FSM_CONTEXT_BUFFER_MAX] = { NULL };
STATIC UINTN fsmContextBuffCounter = 0;
STATIC BOOL pendingEventsFlag = FALSE;

FSM_DEF_RETURN_OP_E
FsmDefInit ()
{
  // HAL_GPIO_CONF_OUTPUT (15);

  //! Initialize the timer to handle the Timed Event Dispatcher.
#ifndef NOT_USE_FSM_TED
  return FsmDefTimerInit();
#else
  return FSM_SUCCESSFUL_RETURN;
#endif  
}

FSM_DEF_RETURN_OP_E
FsmDefCreate (
  FSM_DEF_CONTEXT_T * _newFsmContext)
{

  if (_newFsmContext == NULL ||
    _newFsmContext->tableHnd.pTable == NULL ||
    _newFsmContext->eventManagerHnd.pList == NULL)
  {
    return FSM_DEF_INVALID_PARAMETER;
  }

  if( (_newFsmContext->fsmUid & 0x0000FFFF) )
  {
    FSM_LOG_ERROR("FSM Error: FSM Context with invalid UID\r\n");
    return FSM_DEF_INVALID_PARAMETER;
  }

  if (fsmContextBuffCounter >= FSM_CONTEXT_BUFFER_MAX)
  {
    FSM_LOG_WARNING("FSM Warning: FSM Context buff overflow (FSM_CONTEXT_BUFFER_MAX = %d)\r\n",
    FSM_CONTEXT_BUFFER_MAX);

    return FSM_DEF_CONTEXT_BUFF_OVERFLOW;
  }

  //! Go through all the contexts.
  UINTN i;
  for (i = 0; i < fsmContextBuffCounter; i++)
  {
    if (fsmContextBuff[i]->fsmUid == _newFsmContext->fsmUid)
    {
      FSM_LOG_ERROR("FSM Error: FSM Context with repeated UID[0x%08x]\r\n",
      _newFsmContext->fsmUid);

      return FSM_DEF_REPEATED_UID;
    }
  }

  //_newFsmContext->currentState |= _newFsmContext->fsmUid;
  FSM_LOG_DEBUG("FSM Error: _newFsmContext->currentState[0x%08x]\r\n",
      _newFsmContext->currentState);
  if ( !FsmDefIsStateInTable(_newFsmContext->currentState , _newFsmContext) )
  {
    return FSM_DEF_INVALID_PARAMETER;
  }

  _newFsmContext->currentState |= _newFsmContext->fsmUid;

  FSM_LOG_DEBUG("FSM Debug:%s\r\n",__FUNCTION__); FSM_LOG_INFO("FSM Info:\tNew FSM with UID: 0x%08x\r\n",
  _newFsmContext->fsmUid); FSM_LOG_INFO("FSM Info:\tFSM table size : %d\r\n",
  _newFsmContext->tableHnd.tableSize); FSM_LOG_INFO("FSM Info:\tFSM event_manager list size: %d\r\n",
  _newFsmContext->eventManagerHnd.listSize); FSM_LOG_INFO("FSM Info:\t********************************************\r\n");

  if ( FsmDefConcatenateFsmUidToFsmTable(_newFsmContext->fsmUid, &_newFsmContext->tableHnd) != FSM_DEF_SUCCESSFUL_RETURN)
    return FSM_DEF_INVALID_PARAMETER;

  memset( _newFsmContext->eventHnd.eventBuff,
      (0xFF) ,
      sizeof(_newFsmContext->eventHnd.eventBuff) );
  
  //! Save the new FSM context address in buffer.
  fsmContextBuff[fsmContextBuffCounter] = _newFsmContext;
  fsmContextBuffCounter++;

  return FSM_DEF_SUCCESSFUL_RETURN;
}

FSM_DEF_RETURN_OP_E
FsmDefMainLoop ()
{
  // FSM_LOG_DEBUG("FSM Debug:%s\r\n",__FUNCTION__);

  STATIC BOOL flagFsmTimerOverFlow = FALSE;
#ifndef NOT_USE_FSM_TED
  FsmDefTimerIsTimeout( &flagFsmTimerOverFlow );
#endif

  while ( (pendingEventsFlag  == TRUE ) || (flagFsmTimerOverFlow) )
  { 
#ifndef NOT_USE_FSM_TED
    FsmDefTimerCheck();
    FsmDefTimerUpdate();
    FsmTimedEvtDispatchManager();
#endif    
    
    pendingEventsFlag = FALSE;

    UINTN swapEvt = FSM_DEF_EVENT_NULL;

    //! Go through all the contexts.
    UINTN i;
    for (i = 0; i < fsmContextBuffCounter; i++)
    { 
      //! If there is not a pending event in a FSM context then continue with the next FSM context
      if (fsmContextBuff[i]->eventHnd.eventBuff[fsmContextBuff[i]->eventHnd.eventIndex]
      != FSM_DEF_EVENT_NULL)
      {
        // HAL_GPIO_PIN_SET(15);

        //! Save the pending event in swap.
        swapEvt =
        fsmContextBuff[i]->eventHnd.eventBuff[fsmContextBuff[i]->eventHnd.eventIndex];

        //! Remove event from events buffer.
        fsmContextBuff[i]->eventHnd.eventBuff[fsmContextBuff[i]->eventHnd.eventIndex] =
        FSM_DEF_EVENT_NULL;

        //! To control the events buffer.
        fsmContextBuff[i]->eventHnd.eventIndex =
        ((fsmContextBuff[i]->eventHnd.eventIndex == 0)
        || (fsmContextBuff[i]->eventHnd.eventIndex
        > FSM_EVENTS_BUFFER_MAX)) ?
        0 : (fsmContextBuff[i]->eventHnd.eventIndex - 1);

        //! Process event in FSM context.
        FsmDefHandler(swapEvt,
          fsmContextBuff[i]);

#ifndef NOT_USE_FSM_TED
        FsmDefTimerCheck();
        FsmDefTimerUpdate();
        FsmTimedEvtDispatchManager();
#endif  
        // HAL_GPIO_PIN_CLEAR (15); 
      }
    }

    
    //! Go through all the contexts.
    for (i = 0; i < fsmContextBuffCounter; i++)
    { 
      //! Check pending events of FSM in context
      for (UINT8 j = 0; j <= fsmContextBuff[i]->eventHnd.eventIndex; j++)
      {
        if (fsmContextBuff[i]->eventHnd.eventBuff[fsmContextBuff[j]->eventHnd.eventIndex]
        != (UINTN) FSM_DEF_EVENT_NULL)
        {
          pendingEventsFlag = TRUE;
          break;
        }
      }
    }

    if( !pendingEventsFlag )
    {   
      break;
    }
    
  }

  //! Power save
  //HAL_FSM_DEF_POWER_SAVE();

  return FSM_DEF_SUCCESSFUL_RETURN;

}

FSM_DEF_RETURN_OP_E
FsmDefHandler (
  UINTN _evt,
  FSM_DEF_CONTEXT_T * _fsmContext)
{
  FSM_LOG_DEBUG("FSM Debug:%s\r\n",__FUNCTION__);
  INTN ret = 0;

  if (_fsmContext == NULL)
    return FSM_DEF_INVALID_PARAMETER;
  
  //! Go through the FSM table.
  UINTN i;
  for (i = 0; i < _fsmContext->tableHnd.tableSize; i++)
  {
    //! To know if the event dispatched have some relation in the FSM table.
    if ((_fsmContext->tableHnd.pTable[i].event == _evt)
    && ((_fsmContext->tableHnd.pTable[i].state
    == _fsmContext->currentState)
    || (_fsmContext->tableHnd.pTable[i].state == FSM_DEF_EVT_FOR_ALL)))
    {

      //! Run event manager to dispatch the event to others FSMs (if it is required).
      FsmDefRunEventManager(_evt, _fsmContext);

      UINTN CONST nextState = _fsmContext->tableHnd.pTable[i].nextState;

      FSM_LOG_INFO("FSM Info:\tfsm.[0x%08x] c_st.[0x%08x] evt.[0x%08x] next_st.[0x%08x]\r\n",
      _fsmContext->fsmUid,
      _fsmContext->currentState,
      _evt,
      (nextState == FSM_DEF_EVT_FOR_ALL)? _fsmContext->currentState : nextState);

      if (_fsmContext->tableHnd.pTable[i].handlerFunct == NULL)
      {
        //! If the function in the FSM table is NULL.
        FSM_LOG_INFO("FSM Info:\tfsm.[0x%08x] NULL Callback with Event[0x%08x]\r\n",
        _fsmContext->fsmUid,
        _evt);
        ret = FSM_DEF_NULL_CALLBACK;
      } else
      {
        //! Call the function in the FSM table.
        ret = _fsmContext->tableHnd.pTable[i].handlerFunct();
        if (ret != 0)
        {
          FSM_LOG_WARNING("FSM Warning:\tReturn != 0\r\n");
        }
      }

      //! Change the next state.
      if (_fsmContext->tableHnd.pTable[i].nextState
      != FSM_DEF_EVT_FOR_ALL)
      {
        _fsmContext->currentState =
        _fsmContext->tableHnd.pTable[i].nextState;
      }
      return ret;
    }
  }

  //! Warning if the event dispatched is not supported by current state.
  FSM_LOG_WARNING("FSM Warning: FSM_UID[0x%08x] with Event[0x%08x] - Current State[0x%08x]\r\n",
  _fsmContext->fsmUid,
  _evt,
  _fsmContext->currentState);

  return FSM_DEF_UNMANAGED_EVENT;
}

FSM_DEF_RETURN_OP_E
FsmDefRunEventManager (
  UINTN _evt,
  FSM_DEF_CONTEXT_T * _fsmContext)
{
  FSM_DEF_RETURN_OP_E ret = FSM_DEF_EVENT_NO_FSM_SUBSCRIBED;

  FSM_LOG_DEBUG("FSM Debug:%s\r\n",__FUNCTION__);

  if (_fsmContext == NULL ||
    _fsmContext->eventManagerHnd.pList == NULL)
  {
    ret = FSM_DEF_INVALID_PARAMETER;
  }

  //! Go through all the contexts.
  UINTN i, j;
  for (i = 0; i < fsmContextBuffCounter; i++)
  {
    //! Go through event list manager of FSM in context.
    for (j = 0; j < fsmContextBuff[i]->eventManagerHnd.listSize; j++)
    {
      UINTN EvtSubscribed = (fsmContextBuff[i]->eventManagerHnd.pList[j].fsmUidToSubscribe | fsmContextBuff[i]->eventManagerHnd.pList[j].evtToSubscribe);
      //! To verify if the event dispatched is subscribed in FSM in context.
      if ( (fsmContextBuff[i]->eventManagerHnd.pList[j].fsmUidToSubscribe
          == _fsmContext->fsmUid)
         && ( EvtSubscribed == _evt )
         )
      {
        //! Dispatch the event in FSM
        FsmDefSetNextEvent(fsmContextBuff[i]->fsmUid,
          _evt);
        ret = FSM_DEF_SUCCESSFUL_RETURN;
        break;
      }
    }
  }
  return ret;
}

FSM_DEF_RETURN_OP_E
FsmDefSetNextEvent (
  UINTN _fsmUid,
  UINTN _evt)
{
  FSM_LOG_DEBUG("FSM Debug:%s\r\n",__FUNCTION__);

  FSM_DEF_RETURN_OP_E ret = FSM_DEF_EVENT_NOT_ADDED;


  CONST UINTN evtPlusUid = !(_evt & 0xFFFF0000)? _fsmUid | _evt : _evt;

  //! Go through all the contexts.
  UINTN i;
  for (i = 0; i < fsmContextBuffCounter; i++)
  {
    //! If the FSM UID is the same of the FSM in context buffer.
    //! If not then continue with next FSM context
    if ((fsmContextBuff[i]->fsmUid != _fsmUid))
    {
      continue;
    }

    //! If event buffer is not empty
    if (fsmContextBuff[i]->eventHnd.eventBuff[fsmContextBuff[i]->eventHnd.eventIndex]
    != FSM_DEF_EVENT_NULL)
    {
      //! Check if the event buffer is overflow
      if (fsmContextBuff[i]->eventHnd.eventIndex
      >= (FSM_EVENTS_BUFFER_MAX - 1))
      {
        FSM_LOG_ERROR("FSM Error: FSM_UID[0x%08x] Buffer Overflow, write Event [evt.%x to evt.%x]\r\n",
        fsmContextBuff[i]->fsmUid,
        fsmContextBuff[i]->eventHnd.eventBuff[fsmContextBuff[i]->eventHnd.eventIndex],
        evtPlusUid);

        ret = FSM_DEF_EVENT_BUFF_OVERFLOW;
      }
      //! Add event in next events-buffer position.
      else
      {

        FSM_LOG_INFO("FSM Info: FSM_UID[0x%08x] Event[evt.%x] added in Buff\r\n",
        fsmContextBuff[i]->fsmUid,
        evtPlusUid);

        fsmContextBuff[i]->eventHnd.eventIndex++;

        for (UINT8 k = fsmContextBuff[i]->eventHnd.eventIndex;
        k > 0; k--)
        {
          fsmContextBuff[i]->eventHnd.eventBuff[k] =
          fsmContextBuff[i]->eventHnd.eventBuff[k - 1];
        }

        ret = FSM_DEF_SUCCESSFUL_RETURN;
        fsmContextBuff[i]->eventHnd.eventBuff[0] = evtPlusUid;
        //for (UINT8 h = 0; h < FSM_EVENTS_BUFFER_MAX; h++)
        //{
        //  FSM_LOG_DEBUG("FSM_UID[0x%08x] Evt Buff[%d]: 0x%08x\r\n",fsmContextBuff[i]->fsmUid,h,fsmContextBuff[i]->eventHnd.eventBuff[h]);
        //}
      }
    }
    //! If event buffer is empty. Only add the event in buffer.
    else
    {
      FSM_LOG_DEBUG("FSM Debug: FSM_UID[0x%08x] Event added [evt.%x]\r\n",fsmContextBuff[i]->fsmUid, evtPlusUid);
      fsmContextBuff[i]->eventHnd.eventBuff[fsmContextBuff[i]->eventHnd.eventIndex] =
      evtPlusUid;
      ret = FSM_DEF_SUCCESSFUL_RETURN;
    }
    break;
  }

  if( ret == FSM_DEF_EVENT_NOT_ADDED)
  {
    FSM_LOG_ERROR("FSM Error: FSM_UID[0x%08x] Event not added [0x%08x]\r\n",
        _fsmUid,
        evtPlusUid);
  }

  pendingEventsFlag = (ret == FSM_DEF_SUCCESSFUL_RETURN) ? TRUE : FALSE;

  return ret;
}

UINTN

FsmDefGetState (
  UINTN _fsmUid)
{
  FSM_LOG_DEBUG("FSM Debug:%s\r\n",__FUNCTION__);

  UINTN retSwapSt = FSM_DEF_EVT_FOR_ALL;
  
  //! Go through all the contexts.
  UINTN i;
  for (i = 0; i < fsmContextBuffCounter; i++)
  {
    //! If the FSM UID is the same of the FSM in context.
    if ((fsmContextBuff[i]->fsmUid == _fsmUid))
    {
      retSwapSt = fsmContextBuff[i]->currentState;
      break;
    }
  }
  return (0x0000ffff & retSwapSt);
}

UINTN
FsmDefEraseFsmBuff ()
{
  FSM_LOG_DEBUG("FSM Debug:%s\r\n",__FUNCTION__);

  //memset(fsmContextBuff, NULL, sizeof(fsmContextBuff) );  //FSM_CONTEXT_BUFFER_MAX
  for (UINT8 i = 0; i < FSM_CONTEXT_BUFFER_MAX ; i++ )
  {
    fsmContextBuff[i] = NULL;
  }

  fsmContextBuffCounter = 0;

  return FSM_DEF_SUCCESSFUL_RETURN;
}

FSM_DEF_CONTEXT_T *
FsmDefUidExist(
  UINTN _fsmUid)
{
  FSM_LOG_DEBUG("FSM Debug: %s\r\n",__FUNCTION__);

  FSM_DEF_CONTEXT_T * ret = NULL;
  //! Go through all the contexts.
  UINTN i;
  for (i = 0; i < fsmContextBuffCounter; i++)
  {
    //! If the FSM UID is the same of the FSM in context.
    if ((fsmContextBuff[i]->fsmUid == _fsmUid))
    {
      ret = fsmContextBuff[i];
      return ret;
    }
  }
  return ret;
}


FSM_DEF_RETURN_OP_E
FsmDefConcatenateFsmUidToFsmTable(
  UINTN _fsmUid,
  FSM_DEF_TABLE_HND_T * _table
  )
{
  if(_table == NULL )
    return FSM_DEF_INVALID_PARAMETER; 

  UINTN j;
  for(j = 0; j < _table->tableSize ; j++)
  {
    _table->pTable[j].state |= _fsmUid;
    _table->pTable[j].nextState |= _fsmUid;

    if( !(_table->pTable[j].event & 0xFFFF0000) )
    {
      _table->pTable[j].event |= _fsmUid;
    }

    FSM_LOG_DEBUG("FSM Debug:0x%08x -> 0x%08x ->(hnd) -> 0x%08x \r\n",
      _table->pTable[j].state,
      _table->pTable[j].event,
      _table->pTable[j].nextState);
  }

  return FSM_DEF_SUCCESSFUL_RETURN;
}

BOOL
FsmDefIsEvtInTable(
  UINTN _evt,
  FSM_DEF_CONTEXT_T * _fsmContext)
{
  FSM_LOG_DEBUG("FSM Debug: %s\r\n",__FUNCTION__);

  if (_fsmContext == NULL)
    return FALSE;

  //! Go through the FSM table.
  UINTN i;
  for (i = 0; i < _fsmContext->tableHnd.tableSize; i++)
  {
    //! To know if the event dispatched have some relation in the FSM table.
    if ( (_fsmContext->tableHnd.pTable[i].event == (_fsmContext->fsmUid|_evt) ) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

BOOL
FsmDefIsStateInTable(
  UINTN _state,
  FSM_DEF_CONTEXT_T * _fsmContext)
{
  FSM_LOG_DEBUG("FSM Debug: %s\r\n",__FUNCTION__);

  if (_fsmContext == NULL)
    return FALSE;

  //! Go through all the contexts.
  UINTN i;
  for (i = 0; i < _fsmContext->tableHnd.tableSize; i++)
  {
    //! If the FSM UID is the same of the FSM in context.
    if ((_fsmContext->tableHnd.pTable[i].state == _state) ||
      (_fsmContext->tableHnd.pTable[i].nextState == _state) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

FSM_DEF_RETURN_OP_E
FsmGetLastEvent(
  UINTN _fsmUid,
  UINTN * _lastEvent
  )
{

  if(_lastEvent == NULL)
    return FSM_DEF_UNMANAGED_EVENT;

  *_lastEvent = FSM_DEF_EVENT_NULL;
  
  //! Go through all the contexts.
  UINTN i;
  for (i = 0; i < fsmContextBuffCounter; i++)
  {
    //! If the FSM UID is the same of the FSM in context buffer.
    //! If not then continue with next FSM context
    if ((fsmContextBuff[i]->fsmUid != _fsmUid))
    {
      continue;
    }

    if (fsmContextBuff[i]->eventHnd.eventBuff[0]
    == FSM_DEF_EVENT_NULL)
    {
      return FSM_DEF_SUCCESSFUL_RETURN;
    }

    *_lastEvent =  fsmContextBuff[i]->eventHnd.eventBuff[0];
    return FSM_DEF_SUCCESSFUL_RETURN;
  }

  return FSM_DEF_SUCCESSFUL_RETURN;
}

FSM_DEF_RETURN_OP_E
FsmDefPrintEvtBuff(
  UINTN _fsmUid)
{

    //! Go through all the contexts.
  UINTN i;
  for (i = 0; i < fsmContextBuffCounter; i++)
  {
    //! If the FSM UID is the same of the FSM in context buffer.
    //! If not then continue with next FSM context
    if ((fsmContextBuff[i]->fsmUid != _fsmUid))
    {
      continue;
    }

    for (UINT8 h = 0; h < FSM_EVENTS_BUFFER_MAX; h++)
    {
      FSM_LOG_DEBUG("FSM_UID[0x%08x] Evt Buff[%d]: 0x%08x\r\n",fsmContextBuff[i]->fsmUid,h,fsmContextBuff[i]->eventHnd.eventBuff[h]);
    }
    return FSM_DEF_SUCCESSFUL_RETURN;
  }
  
  return FSM_DEF_UNMANAGED_EVENT;
}

FSM_DEF_RETURN_OP_E
FsmDefIsTherePendingEvents(BOOL *_FlagOut)
{
  if(_FlagOut == NULL)
    return FSM_DEF_INVALID_PARAMETER;
  
  *_FlagOut = pendingEventsFlag;

  return FSM_DEF_SUCCESSFUL_RETURN;
}
