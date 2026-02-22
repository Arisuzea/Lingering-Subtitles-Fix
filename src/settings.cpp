#include "pch.h"
#include "settings.h"
#include "SimpleIni.h"

namespace settings
{
    void Load()
    {
        constexpr auto path = "Data/SKSE/Plugins/LingeringSubtitlesFix.ini";

        CSimpleIniA ini;
        ini.SetUnicode();

        if (ini.LoadFile(path) < 0) {
            SKSE::log::warn("Could not load ini, using defaults.");
            return;
        }

        fCleanupInterval     = static_cast<float>(ini.GetDoubleValue("General", "fCleanupInterval", fCleanupInterval));
        fStuckSubtitleTimeout = static_cast<float>(ini.GetDoubleValue("General", "fStuckSubtitleTimeout", fStuckSubtitleTimeout));

        SKSE::log::info("Settings loaded: fCleanupInterval={}, fStuckSubtitleTimeout={}",
            fCleanupInterval, fStuckSubtitleTimeout);
    }
}