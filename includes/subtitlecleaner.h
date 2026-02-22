#pragma once

#include "pch.h"

namespace subtitlecleaner
{
    void remove_by_handle(const RE::ObjectRefHandle& a_handle);
    void remove_stale();
}