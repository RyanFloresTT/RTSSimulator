#pragma once

#include "Component.h"

class CTransform : public Component {
public:
    sf::Vector2f position = {0.0, 0.0};
    sf::Vector2f velocity = {0.0, 0.0};

    CTransform() = default;

    CTransform(const sf::Vector2f &p, const sf::Vector2f &v) : position(p), velocity(v) {}
};
