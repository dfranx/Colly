#ifndef __COLLY_WORLD_H__
#define __COLLY_WORLD_H__

#include <vector>
#include <functional>
#include <Colly/Body.h>

namespace cl
{
	class World
	{
	public:
		World();

		void Clear();

		void AddObject(int id, Rect bounds, CollisionType type);
		void AddObject(const Body& body);
		Body& GetObject(int id);
		std::vector<Body>& GetObjects();
		void RemoveObject(int id);

		Point Check(int steps, Rect body, Point goal);
		Point Check(int steps, Rect body, Point goal, std::function<void(Body&, World*)> func);

	private:
		std::vector<Body> m_bodies;
	};
}

#endif