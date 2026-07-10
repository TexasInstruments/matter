/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
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

#include <app/TestEventTriggerDelegate.h>

namespace chip {

class DefaultTestEventTriggerDelegate : public TestEventTriggerDelegate
{
public:
    explicit DefaultTestEventTriggerDelegate(const ByteSpan & enableKey) : mEnableKey(enableKey) {}

    bool DoesEnableKeyMatch(const ByteSpan & enableKey) const override;

private:
    ByteSpan mEnableKey;
};

class DefaultTestEventTriggerHandler : public TestEventTriggerHandler
{
    // This is a sample EventTrigger value for testing purposes
    static constexpr uint64_t kSampleTestEventTriggerAlwaysSuccess = static_cast<uint64_t>(0xFFFF'FFFF'0007'0000ull);

public:
    CHIP_ERROR HandleEventTrigger(uint64_t eventTrigger) override
    {
        ChipLogProgress(Support, "Saw TestEventTrigger: " ChipLogFormatX64, ChipLogValueX64(eventTrigger));

        if (eventTrigger == kSampleTestEventTriggerAlwaysSuccess)
        {
            // Do nothing, successfully
            ChipLogProgress(Support, "Handling \"Always success\" internal test event");
            return CHIP_NO_ERROR;
        }

        return CHIP_ERROR_INVALID_ARGUMENT;
    }
};

} // namespace chip
