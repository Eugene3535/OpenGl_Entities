#pragma once

#include <glm/glm.hpp>

namespace glm
{
	template<class T>
	class Rect 
	{
	public:
		Rect() : 
			left(0), top(0), width(0), height(0)
		{
		}

		Rect(T x, T y, T w, T h) :
			left(x), top(y), width(w), height(h)
		{
		}

		const tvec2<T> getPosition() const
		{
			return tvec2<T>(left, top);
		}

		const tvec2<T> getCenter() const
		{
			return tvec2<T>(width * 0.5f, height * 0.5f);
		}

		const tvec2<T> getSize() const
		{
			return tvec2<T>(width, height);
		}

		bool contains(const tvec2<T>& point)
		{
			if (left > point.x)         return false;
			if (top > point.y)          return false;
			if (left + width < point.x) return false;
			if (top + height < point.y) return false;

			return true; // within bounds
		}

		bool intersects(const Rect& rect)
		{
			if (left + width < rect.left)      return false;
			if (left > rect.left + rect.width) return false;
			if (top + height < rect.top)       return false;
			if (top > rect.top + rect.height)  return false;

			return true; // intersection
		}

		T left;
		T top;
		T width;
		T height;
	};

	template<class T>
	bool operator==(const Rect<T>& left, const Rect<T>& right)
	{
		return (left.left   == right.left)  && 
			   (left.width  == right.width) &&
			   (left.top    == right.top)   &&
			   (left.height == right.height);
	}

	template<class T>
	bool operator!=(const Rect<T>& left, const Rect<T>& right)
	{
		return !(left == right);
	}

	using iRect = Rect<int>;
	using uRect = Rect<unsigned>;
	using fRect = Rect<float>;
}

