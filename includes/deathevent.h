#pragma once

#include "pch.h"

namespace deathevent
{
    class Handler : public RE::BSTEventSink<RE::TESDeathEvent>
    {
    public:
        static Handler* GetSingleton()
        {
            static Handler singleton;
            return &singleton;
        }

        RE::BSEventNotifyControl ProcessEvent(
            const RE::TESDeathEvent* a_event,
            RE::BSTEventSource<RE::TESDeathEvent>* a_source) override;

    private:
        Handler() = default;
        Handler(const Handler&) = delete;
        Handler(Handler&&) = delete;
        ~Handler() = default;

        Handler& operator=(const Handler&) = delete;
        Handler& operator=(Handler&&) = delete;
    };

    void Register();
}