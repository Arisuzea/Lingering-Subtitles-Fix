#include "pch.h"
#include "subtitlecleaner.h"
#include "settings.h"

#include <chrono>
#include <unordered_map>

namespace subtitlecleaner
{
    // ---- Stale Subtitle Tracker ----
    // Only populated when normal checks fail.
    // Key: speaker handle value
    // Value: real time point when first seen unresolved.
    static std::unordered_map<std::uint32_t, std::chrono::steady_clock::time_point> trackedSubtitles;

    void remove_by_handle(const RE::ObjectRefHandle& a_handle)
    {
        auto mgr = RE::SubtitleManager::GetSingleton();
        if (!mgr)
            return;

        RE::BSSpinLockGuard guard(mgr->lock);

        auto& subs = mgr->subtitles;
        for (auto it = subs.begin(); it != subs.end();) {
            if (it->speaker == a_handle) {
                trackedSubtitles.erase(it->speaker.native_handle());
                it = subs.erase(it);
            } else {
                ++it;
            }
        }
    }

    void remove_stale()
    {
        auto mgr = RE::SubtitleManager::GetSingleton();
        if (!mgr)
            return;

        const bool dialogueOpen = RE::UI::GetSingleton()->IsMenuOpen(
            RE::DialogueMenu::MENU_NAME);

        const auto now = std::chrono::steady_clock::now();

        RE::BSSpinLockGuard guard(mgr->lock);

        auto& subs = mgr->subtitles;
        for (auto it = subs.begin(); it != subs.end();) {
            const auto ref = it->speaker.get();
            const auto key = it->speaker.native_handle();

            // ---- Invalid Speaker ----
            if (!it->speaker || !ref) {
                trackedSubtitles.erase(key);
                it = subs.erase(it);
                continue;
            }

            const auto actor = ref->As<RE::Actor>();
            if (actor) {
                const auto lifeState = actor->AsActorState()->GetLifeState();

                // ---- Dying / Dead Actor ----
                // kDying catches actors mid-death-animation (e.g. bone dragons)
                // where IsDead() returns false but they're already effectively gone.
                if (lifeState == RE::ACTOR_LIFE_STATE::kDying ||
                    lifeState == RE::ACTOR_LIFE_STATE::kDead) {
                    trackedSubtitles.erase(key);
                    it = subs.erase(it);
                    continue;
                }

                // ---- Ghost Actor ----
                // Ghosts fade out on death rather than transitioning life states normally,
                // so any non-alive state or finished speaking is enough to remove.
                if (actor->IsGhost()) {
                    if (lifeState != RE::ACTOR_LIFE_STATE::kAlive || actor->QSpeakingDone()) {
                        SKSE::log::trace("subtitlecleaner: removing ghost subtitle");
                        trackedSubtitles.erase(key);
                        it = subs.erase(it);
                        continue;
                    }
                }

            // ---- Dead Non-Actor Ref (e.g. Talking Activators) ----
            } else if (ref->IsDead()) {
                trackedSubtitles.erase(key);
                it = subs.erase(it);
                continue;
            }

            // ---- Speaker Finished Talking ----
            // Skip this check during formal dialogue to avoid disrupting
            // proxy-based dialogue systems (e.g. Simply Order Summons).
            if (!dialogueOpen) {
                if (actor && actor->QSpeakingDone()) {
                    SKSE::log::trace("subtitlecleaner: removing orphaned subtitle, speaker is done talking");
                    trackedSubtitles.erase(key);
                    it = subs.erase(it);
                    continue;
                }
            }

            // ---- Timestamp Fallback ----
            // Last resort for cases where actor state is unreliable 
            // (e.g. death alternative mods corrupting nearby NPC state post-revive).
            // Force removes the subtitle after fStuckSubtitleTimeout seconds.
            auto [trackIt, inserted] = trackedSubtitles.emplace(key, now);
            if (!inserted) {
                const float elapsed = std::chrono::duration<float>(now - trackIt->second).count();
                if (elapsed >= settings::fStuckSubtitleTimeout) {
                    SKSE::log::warn("subtitlecleaner: force removing subtitle stuck for {:.1f}s", elapsed);
                    trackedSubtitles.erase(trackIt);
                    it = subs.erase(it);
                    continue;
                }
            }

            ++it;
        }

        // ---- Stale Tracker Cleanup ----
        // Remove tracked entries for subtitles that no longer exist in the array.
        for (auto it = trackedSubtitles.begin(); it != trackedSubtitles.end();) {
            const bool found = std::any_of(subs.begin(), subs.end(), [&](const auto& sub) {
                return sub.speaker.native_handle() == it->first;
            });
            it = found ? std::next(it) : trackedSubtitles.erase(it);
        }
    }
}