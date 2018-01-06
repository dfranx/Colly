#ifndef __COLLY_RECT_H__
#define __COLLY_RECT_H__

#include <Colly/Point.h>

namespace cl
{
	class Rect
	{
	public:
		Rect();
		Rect(float x, float y, float width, float height);

		float X, Y, Width, Height;

		bool Intersects(const Rect& other, Rect& intersection);
	};
}

#endif