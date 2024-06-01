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
 * Define TEC App Events IDs
 */

#ifndef TEC_EVENTS_H
#define TEC_EVENTS_H

#define TEC_RESERVED_EID      0
#define TEC_INIT_INF_EID      1
#define TEC_CC_ERR_EID        2
#define TEC_NOOP_INF_EID      3
#define TEC_RESET_INF_EID     4
#define TEC_MID_ERR_EID       5
#define TEC_CMD_LEN_ERR_EID   6
#define TEC_PIPE_ERR_EID      7
#define TEC_VALUE_INF_EID     8
#define TEC_CR_PIPE_ERR_EID   9
#define TEC_SUB_HK_ERR_EID    10
#define TEC_SUB_CMD_ERR_EID   11
#define TEC_TABLE_REG_ERR_EID 12

#endif /* TEC_EVENTS_H */
