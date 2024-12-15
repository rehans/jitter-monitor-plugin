// Copyright(c) 2024 rehans.

#pragma once

#include <cstdint>

namespace jittering {
//------------------------------------------------------------------------
struct State
{
    double current            = 0.5;
    double min                = 0.5;
    double max                = 0.5;
    double average            = 0.5;
    int64_t system_time_stamp = 0;
};

//------------------------------------------------------------------------
auto reset(const State& state) -> State;
auto update(double sample_rate, int32_t num_samples, State& jitter) -> State;
auto get_current_system_time() -> int64_t;

//------------------------------------------------------------------------
} // namespace jittering
