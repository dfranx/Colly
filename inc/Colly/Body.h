#ifndef __COLLY_BODY_H__
#define __COLLY_BODY_H__
#include <Colly/Rect.h>
#include <Colly/Point.h>
#include <Colly/CollisionType.h>

namespace cl
{
	struct Body
	{
		int Id;
		Rect Bounds;
		CollisionType Type;
	};
}

#endif