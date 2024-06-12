/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
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

#include "ApplicationManager.h"
#include "BindingHandler.h"

#include "AppConfig.h"
#include "AppTask.h"
#include <FreeRTOS.h>

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/cluster-objects.h>
#include <app/clusters/bindings/BindingManager.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace ::chip::DeviceLayer;

ApplicationManager ApplicationManager::sApplication;

TimerHandle_t sApplicationTimer;

CHIP_ERROR ApplicationManager::Init()
{
    // Create FreeRTOS sw timer for Application timer.
    sApplicationTimer = xTimerCreate("ApplicationTmr", // Just a text name, not used by the RTOS kernel
                                     1,                // == default timer period (mS)
                                     false,            // no timer reload (==one-shot)
                                     (void *) this,    // init timer id = Application obj context
                                     TimerEventHandler // timer callback handler
    );

    if (sApplicationTimer == NULL)
    {
        PLAT_LOG("sApplicationTimer timer create failed");
        return APP_ERROR_CREATE_TIMER_FAILED;
    }

#ifdef BINDING_ENABLED
    CHIP_ERROR error = InitBindingHandler();
    if (error != CHIP_NO_ERROR)
    {
        PLAT_LOG("InitBindingHandler() failed!");
        return APP_ERROR_CREATE_TIMER_FAILED;
    }
#endif
    mState = kState_Action2_Completed;

    return CHIP_NO_ERROR;
}

void ApplicationManager::SetCallbacks(Callback_fn_initiated aActionInitiated_CB, Callback_fn_completed aActionCompleted_CB)
{
    mActionInitiated_CB = aActionInitiated_CB;
    mActionCompleted_CB = aActionCompleted_CB;
}

bool ApplicationManager::InitiateAction(int32_t aActor, Action_t aAction)
{
    bool action_initiated = false;
    State_t new_state;

#ifdef BINDING_ENABLED
    BindingCommandData * data = Platform::New<BindingCommandData>();
    data->clusterId           = chip::app::Clusters::OnOff::Id;
    data->isGroup             = false;
#endif

    // Initiate On/Off Action only when the previous one is complete.
    if ((mState == kState_Action1_Completed || mState == kState_Action2_Completed) && aAction == ACTION_1)
    {
        action_initiated = true;
        new_state        = kState_Action1_Initiated;
#ifdef BINDING_ENABLED
        data->commandId = chip::app::Clusters::OnOff::Commands::On::Id;
#endif
    }
    else if ((mState == kState_Action1_Completed || mState == kState_Action2_Completed) && aAction == ACTION_2)
    {
        action_initiated = true;
        new_state        = kState_Action2_Initiated;
#ifdef BINDING_ENABLED
        data->commandId = chip::app::Clusters::OnOff::Commands::Off::Id;
#endif
    }

    if (action_initiated)
    {
        StartTimer(ACTUATOR_MOVEMENT_PERIOD_MS);
        // Since the timer started successfully, update the state and trigger callback
        mState = new_state;

        if (mActionInitiated_CB)
        {
            mActionInitiated_CB(aAction, aActor);
        }
    }

    return action_initiated;
}

void ApplicationManager::StartTimer(uint32_t aTimeoutMs)
{
    if (xTimerIsTimerActive(sApplicationTimer))
    {
        PLAT_LOG("app timer already started!");
        CancelTimer();
    }
    // timer is not active, change its period to required value (== restart).
    // FreeRTOS- Block for a maximum of 100 ticks if the change period command
    // cannot immediately be sent to the timer command queue.
    if (xTimerChangePeriod(sApplicationTimer, pdMS_TO_TICKS(aTimeoutMs), 100) != pdPASS)
    {
        PLAT_LOG("sApplicationTimer timer start() failed");
    }
}

void ApplicationManager::CancelTimer(void)
{
    if (xTimerStop(sApplicationTimer, 0) == pdFAIL)
    {
        PLAT_LOG("sApplicationTimer stop() failed");
    }
}

void ApplicationManager::TimerEventHandler(TimerHandle_t xTimer)
{
    // Get Application obj context from timer id.
    ApplicationManager * Action = static_cast<ApplicationManager *>(pvTimerGetTimerID(xTimer));
    // The timer event handler will be called in the context of the timer task
    // once sApplicationTimer expires. Post an event to apptask queue with the actual handler
    // so that the event can be handled in the context of the apptask.
    AppEvent event;
    event.Type                     = AppEvent::kEventType_AppEvent;
    event.ApplicationEvent.Context = Action;
    event.Handler                  = ActionTimerEventHandler;
    AppTask::GetAppTask().PostEvent(&event);
}

void ApplicationManager::ActionTimerEventHandler(AppEvent * aEvent)
{
    Action_t actionCompleted = INVALID_ACTION;
#ifdef BINDING_ENABLED
    BindingCommandData * data = Platform::New<BindingCommandData>();
    data->clusterId           = chip::app::Clusters::OnOff::Id;
#endif

    ApplicationManager * Action = static_cast<ApplicationManager *>(aEvent->ApplicationEvent.Context);
    if (Action->mState == kState_Action2_Initiated)
    {
        Action->mState  = kState_Action2_Completed;
        actionCompleted = ACTION_2;
#ifdef BINDING_ENABLED
        data->commandId = chip::app::Clusters::OnOff::Commands::Off::Id;
#endif
    }
    else if (Action->mState == kState_Action1_Initiated)
    {
        Action->mState  = kState_Action1_Completed;
        actionCompleted = ACTION_1;
#ifdef BINDING_ENABLED
        data->commandId = chip::app::Clusters::OnOff::Commands::On::Id;
#endif
    }

    if (actionCompleted != INVALID_ACTION)
    {
#ifdef BINDING_ENABLED
        DeviceLayer::PlatformMgr().ScheduleWork(ApplicationWorkerFunction, reinterpret_cast<intptr_t>(data));
#endif
        if (Action->mActionCompleted_CB)
        {
            Action->mActionCompleted_CB(actionCompleted);
        }
    }
}
