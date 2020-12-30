#include "Color.hpp"

Color::Color() : glm::vec4(255.0f)
{
}

Color::Color(float red_value, float green_value, float blue_value, float alpha_channel):
	glm::vec4(Color::normalize(red_value, green_value, blue_value, alpha_channel))
{	
}

const glm::vec4 Color::normalize(float red_value, float green_value, float blue_value, float alpha_channel)
{
	if (red_value     < 0.0f) red_value     = 0.0f; if (red_value     > 255.0f) red_value     = 255.0f;
	if (green_value   < 0.0f) green_value   = 0.0f; if (green_value   > 255.0f) green_value   = 255.0f;
	if (blue_value    < 0.0f) blue_value    = 0.0f; if (blue_value    > 255.0f) blue_value    = 255.0f;
	if (alpha_channel < 0.0f) alpha_channel = 0.0f; if (alpha_channel > 255.0f) alpha_channel = 255.0f;

	if (red_value)     red_value     /= 255.0f;
	if (green_value)   green_value   /= 255.0f;
	if (blue_value)    blue_value    /= 255.0f;
	if (alpha_channel) alpha_channel /= 255.0f;

	return glm::vec4(red_value, green_value, blue_value, alpha_channel);
}

const Color Color::BLUE(0.0f, 0.0f, 255.0f, 255.0f);
const Color Color::WHITE(255.0f, 255.0f, 255.0f, 255.0f);
const Color Color::RED(255.0f, 0.0f, 0.0f, 255.0f);
const Color Color::GREEN(0.0f, 255.0f, 0.0f, 255.0f);
const Color Color::YELLOW(255.0f, 255.0f, 0.0f, 255.0f);
const Color Color::MAGENTA(255, 0, 255, 255.0f);
const Color Color::CYAN(0, 255, 255, 255.0f);
