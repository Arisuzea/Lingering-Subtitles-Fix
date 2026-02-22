#include "pch.h"
#include "deathevent.h"
#include "subtitlecleaner.h"

namespace deathevent
{
    RE::BSEventNotifyControl Handler::ProcessEvent(
        const RE::TESDeathEvent* a_event,
        RE::BSTEventSource<RE::TESDeathEvent>*)
    {
        if (!a_event || !a_event->actorDying)
            return RE::BSEventNotifyControl::kContinue;

        auto handle = RE::BSPointerHandleManagerInterface<RE::TESObjectREFR>::GetHandle(
            a_event->actorDying.get());

        subtitlecleaner::remove_by_handle(handle);

        return RE::BSEventNotifyControl::kContinue;
    }

    void Register()
    {
        auto* eventSource = RE::ScriptEventSourceHolder::GetSingleton();
        if (eventSource) {
            eventSource->AddEventSink<RE::TESDeathEvent>(Handler::GetSingleton());
            SKSE::log::info("Lingering Subtitles Fix: Death event sink registered.");
        }
    }
}  