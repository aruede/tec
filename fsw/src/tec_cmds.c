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
 *   This file contains the source code for the TEC App Ground Command-handling functions
 */

/*
** Include Files:
*/
#include "tec.h"
#include "tec_cmds.h"
#include "tec_msgids.h"
#include "tec_eventids.h"
#include "tec_version.h"
#include "tec_tbl.h"
#include "tec_utils.h"
#include "tec_msg.h"

/* The sample_lib module provides the SAMPLE_Function() prototype */
#include "sample_lib.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
CFE_Status_t TEC_SendHkCmd(const TEC_SendHkCmd_t *Msg)
{
    int i;

    /*
    ** Get command execution counters...
    */
    TEC_Data.HkTlm.Payload.CommandErrorCounter = TEC_Data.ErrCounter;
    TEC_Data.HkTlm.Payload.CommandCounter      = TEC_Data.CmdCounter;

    /*
    ** Get spacecraft temperature...
    */

    TEC_Data.HkTlm.Payload.Temperature = TEC_Data.Temperature;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(TEC_Data.HkTlm.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(TEC_Data.HkTlm.TelemetryHeader), true);

    /*
    ** Manage any pending table loads, validations, etc.
    */
    for (i = 0; i < TEC_NUMBER_OF_TABLES; i++)
    {
        CFE_TBL_Manage(TEC_Data.TblHandles[i]);
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TEC NOOP commands                                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
CFE_Status_t TEC_NoopCmd(const TEC_NoopCmd_t *Msg)
{
    TEC_Data.CmdCounter++;

    CFE_EVS_SendEvent(TEC_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "TEC: NOOP command %s",
                      TEC_VERSION);

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
CFE_Status_t TEC_ResetCountersCmd(const TEC_ResetCountersCmd_t *Msg)
{
    TEC_Data.CmdCounter = 0;
    TEC_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(TEC_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "TEC: RESET command");

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function Process Ground Station Command                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
CFE_Status_t TEC_ProcessCmd(const TEC_ProcessCmd_t *Msg)
{
    CFE_Status_t               status;
    void *                     TblAddr;
    TEC_ExampleTable_t *TblPtr;
    const char *               TableName = "TEC.ExampleTable";

    /* Example Use of Example Table */

    status = CFE_TBL_GetAddress(&TblAddr, TEC_Data.TblHandles[0]);

    if (status < CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("TEC App: Fail to get table address: 0x%08lx", (unsigned long)status);
        return status;
    }

    TblPtr = TblAddr;
    CFE_ES_WriteToSysLog("TEC App: Example Table Value 1: %d  Value 2: %d", TblPtr->Int1, TblPtr->Int2);

    TEC_GetCrc(TableName);

    status = CFE_TBL_ReleaseAddress(TEC_Data.TblHandles[0]);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("TEC App: Fail to release table address: 0x%08lx", (unsigned long)status);
        return status;
    }

    /* Invoke a function provided by TEC_LIB */
    SAMPLE_LIB_Function();

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* A simple example command that displays a passed-in value                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
CFE_Status_t TEC_DisplayParamCmd(const TEC_DisplayParamCmd_t *Msg)
{
    TEC_Data.CmdCounter++;

    CFE_EVS_SendEvent(TEC_VALUE_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "TEC: ValU32=%lu, ValI16=%d, ValStr=%s", (unsigned long)Msg->Payload.ValU32,
                      (int)Msg->Payload.ValI16, Msg->Payload.ValStr);

    return CFE_SUCCESS;
}

CFE_Status_t TEC_GetTemperatureCmd(const TEC_GetTemperatureCmd_t *Msg)
{
    TEC_Data.CmdCounter++;

    CFE_EVS_SendEvent(TEC_TEMPERATURE_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "TEC: Requested temperature in %c", Msg->Payload.Unit);

    TEC_GetTemperature(Msg->Payload.Unit);

    return CFE_SUCCESS;
}