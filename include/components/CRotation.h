#pragma once

#include "Component.h"

class CRotation : public Component {
public:
    float angle = 0;
    float speed = 0;

    CRotation() = default;

    CRotation(float s) : speed(s) {}
};