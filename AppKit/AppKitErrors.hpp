//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/AppKitErrors.hpp
//
// Copyright 2026 SylvaGX.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "AKDefines.hpp"
#include "Foundation.hpp"

namespace NS
{
_AK_ENUM(Integer, AppKitErrorCode) {
    WindowSharingRequestAlreadyRequested = 67456,
    WindowSharingRequestNoEligibleSession = 67457,
    WindowSharingRequestUnspecifiedError = 67458,
    WindowSharingErrorMinimum = 67456,
    WindowSharingErrorMaximum = 67466,
};
}
