// Copyright(c) 2024 rehans.

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace Steinberg::jitter_monitoring {
//------------------------------------------------------------------------
static const Steinberg::FUID
    kProcessorUID(0xA1DCF1D2, 0x0AD05BFE, 0x93E7CDC4, 0xEE65D072);
static const Steinberg::FUID
    kControllerUID(0x42C6CE2C, 0x28C45D2C, 0xB7E3DD1C, 0x2897CE14);

#define JitterMonitorVST3Category "Fx|OnlyRT"

//------------------------------------------------------------------------
} // namespace Steinberg::jitter_monitoring
