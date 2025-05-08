#pragma once

#include <random>
#include "SFML/System/Vector2.hpp"

class VectorUtils {
public:
    static sf::Vector2f generateNormalizedVector() {
        std::random_device                    rd;
        std::mt19937                          rng(rd());
        std::uniform_real_distribution<float> dist(-1.0, 1.0);

        float x = dist(rng);
        float y = dist(rng);

        // Calculate magnitude
        float magnitude = std::sqrt(x * x + y * y);

        // Normalize vector
        if (magnitude > 0) {
            x /= magnitude;
            y /= magnitude;
        }
        return sf::Vector2f{x, y};
    }
};