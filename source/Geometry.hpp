#pragma once

#include <glm/glm.hpp>

class HitBox: public glm::vec4
{
public:
	HitBox() : glm::vec4()
	{
	}

	HitBox(float _x, float _y, float _z, float _w) : glm::vec4(_x, _y, _z, _w)
	{
	}

	~HitBox() = default;

	const glm::vec2 getPosition() const
	{
		return glm::vec2(x, y);
	}

	const glm::vec2 getSize() const
	{
		return glm::vec2(z, w);
	}

	const glm::vec2 getOrigin() const
	{
		return glm::vec2(z * 0.5f, w * 0.5f);
	}

	bool contains(const glm::vec2& point)
	{
		if (x > point.x)     return false;
		if (y > point.y)     return false;
		if (x + z < point.x) return false;
		if (y + w < point.y) return false;
		return true; // within bounds
	}

	bool intersects(const HitBox& other_box)
	{
		if (x + z < other_box.x)           return false;
		if (x > other_box.x + other_box.z) return false;
		if (y + w < other_box.y)           return false;
		if (y > other_box.y + other_box.w) return false;
		return true; // intersection
	}
};