/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the LICENSE file.
 */

#pragma once

#include "DistrhoUtils.hpp"

namespace rack {
namespace window {
struct Window;
}
}

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

enum WindowParameterList {
    kWindowParameterCableOpacity,
    kWindowParameterCableTension,
    kWindowParameterRackBrightness,
    kWindowParameterHaloBrightness,
    kWindowParameterCount,
};

struct WindowParameters {
    float cableOpacity = 0.5f;
    float cableTension = 0.5f;
    float rackBrightness = 1.0f;
    float haloBrightness = 0.25f;
    // KnobMode knobMode = KNOB_MODE_LINEAR;
};

struct WindowParametersCallback {
    virtual ~WindowParametersCallback() {}
    virtual void WindowParametersChanged(WindowParameterList param, float value) = 0;
};

void WindowParametersSave(rack::window::Window* window);
void WindowParametersRestore(rack::window::Window* window);
void WindowParametersSetCallback(rack::window::Window* window, WindowParametersCallback* callback);
void WindowParametersSetValues(rack::window::Window* window, const WindowParameters& params);

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO