//
// SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef STT_STREAMLINE_HPP
#define STT_STREAMLINE_HPP

#include "streamline_annotate.h"

#include <cstdint>

namespace sl {

enum : uint32_t {
    GROUP_STT      = 2,
    CH_INIT        = 20,
    CH_TRANSCRIBE  = 21,
    CH_POSTPROCESS = 22,
    CH_CONTROL     = 23,
};

inline void InitThreadOnce()
{
    static thread_local bool inited = false;
    if (inited) {
        return;
    }

    ANNOTATE_SETUP;

    ANNOTATE_NAME_GROUP(GROUP_STT, "STT");
    ANNOTATE_NAME_CHANNEL(CH_INIT, GROUP_STT, "Init");
    ANNOTATE_NAME_CHANNEL(CH_TRANSCRIBE, GROUP_STT, "Transcribe");
    ANNOTATE_NAME_CHANNEL(CH_POSTPROCESS, GROUP_STT, "Postprocess");
    ANNOTATE_NAME_CHANNEL(CH_CONTROL, GROUP_STT, "Control");

    inited = true;
}

struct Scope {
    uint32_t ch;

    Scope(uint32_t channel, uint32_t color, const char* name) : ch(channel)
    {
        ANNOTATE_CHANNEL_COLOR(ch, color, name);
    }

    ~Scope()
    {
        ANNOTATE_CHANNEL_END(ch);
    }
};

inline void marker(uint32_t color, const char* text)
{
    ANNOTATE_MARKER_COLOR_STR(color, text);
}

} // namespace sl

#endif // STT_STREAMLINE_HPP
