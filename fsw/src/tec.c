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
#include "tec_cmds.h"
#include "tec_utils.h"
#include "tec_eventids.h"
#include "tec_dispatch.h"
#include "tec_tbl.h"
#include "tec_version.h"

/*
** global data
*/
TEC_Data_t TEC_Data;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/*                                                                            */
/* Application entry point and main process loop                              */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void TEC_Main(void)
{
    CFE_Status_t     status;
    CFE_SB_Buffer_t *SBBufPtr;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(TEC_PERF_ID);

    /*
    ** Perform application-specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = TEC_Init();
    if (status != CFE_SUCCESS)
    {
        TEC_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** TEC App Runloop
    */
    while (CFE_ES_RunLoop(&TEC_Data.RunStatus) == true)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(TEC_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer(&SBBufPtr, TEC_Data.CommandPipe, CFE_SB_PEND_FOREVER);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(TEC_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            TEC_TaskPipe(SBBufPtr);
        }
        else
        {
            CFE_EVS_SendEvent(TEC_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "TEC APP: SB Pipe Read Error, App Will Exit");

            TEC_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(TEC_PERF_ID);

    CFE_ES_ExitApp(TEC_Data.RunStatus);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* Initialization                                                             */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
CFE_Status_t TEC_Init(void)
{
    CFE_Status_t status;
    char         VersionString[TEC_CFG_MAX_VERSION_STR_LEN];

    /* Zero out the global data structure */
    memset(&TEC_Data, 0, sizeof(TEC_Data));

    TEC_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    TEC_Data.Temperature = 55;

    /*
    ** Initialize app configuration data
    */
    TEC_Data.PipeDepth = TEC_PIPE_DEPTH;

    strncpy(TEC_Data.PipeName, "TEC_CMD_PIPE", sizeof(TEC_Data.PipeName));
    TEC_Data.PipeName[sizeof(TEC_Data.PipeName) - 1] = 0;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("TEC App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
    }
    else
    {
        /*
         ** Initialize housekeeping packet (clear user data area).
         */
        CFE_MSG_Init(CFE_MSG_PTR(TEC_Data.HkTlm.TelemetryHeader), CFE_SB_ValueToMsgId(TEC_HK_TLM_MID),
                     sizeof(TEC_Data.HkTlm));

        /*
         ** Create Software Bus message pipe.
         */
        status = CFE_SB_CreatePipe(&TEC_Data.CommandPipe, TEC_Data.PipeDepth, TEC_Data.PipeName);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(TEC_CR_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "TEC App: Error creating SB Command Pipe, RC = 0x%08lX", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        /*
        ** Subscribe to Housekeeping request commands
        */
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(TEC_SEND_HK_MID), TEC_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(TEC_SUB_HK_ERR_EID, CFE_EVS_EventType_ERROR,
                              "TEC App: Error Subscribing to HK request, RC = 0x%08lX", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        /*
        ** Subscribe to ground command packets
        */
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(TEC_CMD_MID), TEC_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(TEC_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
                              "TEC App: Error Subscribing to Commands, RC = 0x%08lX", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        /*
        ** Register Example Table(s)
        */
        status = CFE_TBL_Register(&TEC_Data.TblHandles[0], "ExampleTable", sizeof(TEC_ExampleTable_t),
                                  CFE_TBL_OPT_DEFAULT, TEC_TblValidationFunc);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(TEC_TABLE_REG_ERR_EID, CFE_EVS_EventType_ERROR,
                              "TEC App: Error Registering Example Table, RC = 0x%08lX", (unsigned long)status);
        }
        else
        {
            status = CFE_TBL_Load(TEC_Data.TblHandles[0], CFE_TBL_SRC_FILE, TEC_TABLE_FILE);
        }

        CFE_Config_GetVersionString(VersionString, TEC_CFG_MAX_VERSION_STR_LEN, "TEC App", TEC_VERSION,
                                    TEC_BUILD_CODENAME, TEC_LAST_OFFICIAL);

        CFE_EVS_SendEvent(TEC_INIT_INF_EID, CFE_EVS_EventType_INFORMATION, "TEC App Initialized.%s",
                          VersionString);
    }

    return status;
}

void TEC_GetTemperature(char Unit)
{

    if(Unit == 'C')
    {
        TEC_Data.Temperature = 69;
    }
    else if (Unit == 'F')
    {
        TEC_Data.Temperature = 99;
    }
    else
    {   
        CFE_EVS_SendEvent(TEC_INVALID_ERR_EID, CFE_EVS_EventType_ERROR,
                            "TEC: Invalid unit specifier %c. Please use C or F.\n", Unit);
    }
}
