#pragma once

#include "Component.h"

class CInput : public Component {
public:
    bool up    = false;
    bool down  = false;
    bool left  = false;
    bool right = false;
    bool shoot = false;

    CInput() = default;
};