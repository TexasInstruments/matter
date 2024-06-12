/*
 *
 *    Copyright (c) 2019 Google LLC.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include <app/clusters/bindings/BindingManager.h>
#include <app/clusters/on-off-server/on-off-server.h>

#include <stdbool.h>
#include <stdint.h>

#include "AppEvent.h"

#include "FreeRTOS.h"
#include "timers.h" // provides FreeRTOS timer support
#include <app/util/basic-types.h>

#include <lib/core/CHIPError.h>
#include <lib/support/CodeUtils.h>

// comment out to disable binding
#define BINDING_ENABLED

using namespace ::chip;

class ApplicationManager
{
public:
    enum Action_t
    {
        ACTION_1 = 0,
        ACTION_2,
        INVALID_ACTION
    } Action;

    enum State_t
    {
        kState_Action2_Initiated = 0,
        kState_Action2_Completed,
        kState_Action1_Initiated,
        kState_Action1_Completed,
    } State;

    CHIP_ERROR Init();

    bool InitiateAction(int32_t aActor, Action_t aAction);

    typedef void (*Callback_fn_initiated)(Action_t, int32_t aActor);
    typedef void (*Callback_fn_completed)(Action_t);
    void SetCallbacks(Callback_fn_initiated aActionInitiated_CB, Callback_fn_completed aActionCompleted_CB);

private:
    friend ApplicationManager & ApplicationMgr(void);
    State_t mState;

    Callback_fn_initiated mActionInitiated_CB;
    Callback_fn_completed mActionCompleted_CB;

    void CancelTimer(void);
    void StartTimer(uint32_t aTimeoutMs);

    static void TimerEventHandler(TimerHandle_t xTimer);
    static void ActionTimerEventHandler(AppEvent * aEvent);

    static ApplicationManager sApplication;
};

inline ApplicationManager & ApplicationMgr(void)
{
    return ApplicationManager::sApplication;
}
