#include "pch.h"
#include "updatehook.h"
#include "subtitlecleaner.h"
#include "settings.h"

namespace updatehook
{
    struct PlayerCharacterUpdateHook
    {
        static void Update(RE::PlayerCharacter* a_pc, float a_delta)
        {
            _Update(a_pc, a_delta);

            static float elapsed = 0.f;
            elapsed += a_delta;

            if (elapsed >= settings::fCleanupInterval) {
                subtitlecleaner::remove_stale();
                elapsed = 0.f;
            }
        }

        static inline REL::Relocation<decltype(Update)> _Update;

        static void Install()
        {
            REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_PlayerCharacter[0] };
            _Update = vtbl.write_vfunc(0xAD, Update);
            SKSE::log::info("LingeringSubtitlesFix: PlayerCharacter::Update hooked.");
        }
    };

    void Install()
    {
        PlayerCharacterUpdateHook::Install();
    }
}