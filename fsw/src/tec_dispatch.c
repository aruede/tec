/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * \file
 *   This file contains the source code for the TEC App.
 */

/*
** Include Files:
*/
#include "tec.h"
#include "tec_dispatch.h"
#include "tec_cmds.h"
#include "tec_eventids.h"
#include "tec_msgids.h"
#include "tec_msg.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* Verify command packet length                                               */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool TEC_VerifyCmdLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    size_t            ActualLength = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode      = 0;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        CFE_EVS_SendEvent(TEC_CMD_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        TEC_Data.ErrCounter++;
    }

    return result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TEC ground commands                                                     */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void TEC_ProcessGroundCommand(const CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process TEC app ground commands
    */
    switch (CommandCode)
    {
        case TEC_NOOP_CC:
            if (TEC_VerifyCmdLength(&SBBufPtr->Msg, sizeof(TEC_NoopCmd_t)))
            {
                TEC_NoopCmd((const TEC_NoopCmd_t *)SBBufPtr);
            }
            break;

        case TEC_RESET_COUNTERS_CC:
            if (TEC_VerifyCmdLength(&SBBufPtr->Msg, sizeof(TEC_ResetCountersCmd_t)))
            {
                TEC_ResetCountersCmd((const TEC_ResetCountersCmd_t *)SBBufPtr);
            }
            break;

        case TEC_PROCESS_CC:
            if (TEC_VerifyCmdLength(&SBBufPtr->Msg, sizeof(TEC_ProcessCmd_t)))
            {
                TEC_ProcessCmd((const TEC_ProcessCmd_t *)SBBufPtr);
            }
            break;

        case TEC_DISPLAY_PARAM_CC:
            if (TEC_VerifyCmdLength(&SBBufPtr->Msg, sizeof(TEC_DisplayParamCmd_t)))
            {
                TEC_DisplayParamCmd((const TEC_DisplayParamCmd_t *)SBBufPtr);
            }
            break;

        case TEC_GET_TEMPERATURE_CC:
            if (TEC_VerifyCmdLength(&SBBufPtr->Msg, sizeof(TEC_TemperatureHkCmd_t)))
            {
                TEC_ConvertTemperatureCmd((const TEC_TemperatureHkCmd_t *)SBBufPtr);
            }
            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(TEC_CC_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid ground command code: CC = %d",
                              CommandCode);
            break;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the TEC    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TEC_TaskPipe(const CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case TEC_CMD_MID:
            TEC_ProcessGroundCommand(SBBufPtr);
            break;

        case TEC_SEND_HK_MID:
            TEC_SendHkCmd((const TEC_SendHkCmd_t *)SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(TEC_MID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "TEC: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }
}
