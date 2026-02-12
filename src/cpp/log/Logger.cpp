//
// SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//
#include "Logger.hpp"

#include <cstdarg>
#include <vector>
#include <stdexcept>
#include <cstdio>

namespace SttLog {

    /**
     * Format a printf-style string using a supplied `va_list` and return it
     * as a std::string. This does a sizing pass first, then formats into an
     * appropriately sized buffer.
     */
    std::string vformat_va(const char* format, va_list args) {
        // First pass: duplicate the va_list because std::vsnprintf consumes it
        va_list sizingArgs;
        va_copy(sizingArgs, args);
        const int requiredSize = std::vsnprintf(nullptr, 0, format, sizingArgs);
        va_end(sizingArgs);

        if (requiredSize < 0) {
            throw std::runtime_error(
                "Error while logging a message: logging string sizing failed.\n");
        }

        // Second pass: allocate exact buf (+1 for null terminator) and format
        std::vector<char> buf(static_cast<size_t>(requiredSize) + 1);
        const int charsWritten = std::vsnprintf(buf.data(), buf.size(), format, args);
        (void)charsWritten;

        return buf.data();
    }
} // namespace SttLog
