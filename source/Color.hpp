#pragma once

#include <glm/glm.hpp>

class Color : public glm::vec4
{
public:
    Color();
    Color(float red_value, float green_value, float blue_value, float alpha_channel);
    ~Color() = default;

    static const glm::vec4 normalize(float red_value, float green_value, float blue_value, float alpha_channel);

    static const Color WHITE;
    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color YELLOW;
    static const Color MAGENTA;
    static const Color CYAN;
};

