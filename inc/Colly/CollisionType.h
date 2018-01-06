#ifndef __COLLY_COLLISION_TYPE_H__
#define __COLLY_COLLISION_TYPE_H__

namespace cl
{
	enum class CollisionType
	{
		None,	// no collision will occur
		Solid,	// collision will be handled
		Cross	// used for coins, other pick ups, etc...
	};
}

#endif