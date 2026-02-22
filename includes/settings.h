#pragma once

#include "pch.h"

namespace settings
{
    // ---- Cleanup Interval ----
    // How often (in seconds) the stale subtitle cleanup pass runs.
    inline float fCleanupInterval = 0.5f;

    // ---- Stuck Subtitle Timeout ----
    // How long (in seconds) a subtitle can linger before being force removed
    // as a last resort when normal checks fail.
    inline float fStuckSubtitleTimeout = 15.0f;

    void Load();
}