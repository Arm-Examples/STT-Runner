//
// SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#pragma once


/**
 * @file SttErrorMessages.hpp
 * @brief Contains error message constants for STT operations.
 */
namespace SttErrorMessages {

// WhisperImpl
/**
 * Error raised when Whisper context initialization fails.
 * Expected format argument: model path.
 */
inline constexpr const char* INIT_CONTEXT_FAIL = "Failed to initialise Whisper context from %s";

/** FullTranscribe was invoked without a valid context pointer. */
inline constexpr const char* FULL_TRANSCRIBE_NULL_CTX = "FullTranscribe called with null contextPtr";

/**
 * FullTranscribe received an invalid audio buffer.
 * Format arguments: audio buffer pointer and length.
 */
inline constexpr const char* FULL_TRANSCRIBE_INVALID_AUDIO = "FullTranscribe called with invalid audio buffer (ptr=%p len=%d)";

} // namespace SttErrorMessages
