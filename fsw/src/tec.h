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
 * @file
 *
 * Main header file for the TEC application
 */

#ifndef TEC_H
#define TEC_H

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_config.h"

#include "tec_mission_cfg.h"
#include "tec_platform_cfg.h"

#include "tec_perfids.h"
#include "tec_msgids.h"
#include "tec_msg.h"

/************************************************************************
** Type Definitions
*************************************************************************/

// #define LocalProcessorID    CFE_PLATFORM_TBL_VALID_PRID_1

/*
** Global Data
*/
typedef struct
{
    /*
    ** Command interface counters...
    */
    uint8 CmdCounter;
    uint8 ErrCounter;

    /*
    ** Sensor inputs...
    */
    
    // This is the majority voted temperature
    uint32 Temperature;
    // This is the locally measure temperature
    uint32 TemperatureHk;
    char TemperatureUnitHk;
    // Those are the remotely measured temperatures
    uint32 RemoteTemperatures[2];

    /*
    ** Housekeeping telemetry packet...
    */
    TEC_HkTlm_t HkTlm;

    /*
    ** Run Status variable used in the main processing loop
    */
    uint32 RunStatus;

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t CommandPipe;

    /*
    ** Initialization data (not reported in housekeeping)...
    */
    char   PipeName[CFE_MISSION_MAX_API_LEN];
    uint16 PipeDepth;

    CFE_TBL_Handle_t TblHandles[TEC_NUMBER_OF_TABLES];

    // uint32 ProcessorID;
} TEC_Data_t;

/*
** Global data structure
*/
extern TEC_Data_t TEC_Data;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (TEC_Main), these
**       functions are not called from any other source module.
*/
void         TEC_Main(void);
CFE_Status_t TEC_Init(void);
void         TEC_ConvertHkTemperature(char Unit);

#endif /* TEC_H */
