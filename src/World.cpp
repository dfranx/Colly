#include <Colly/World.h>

namespace cl
{
	World::World()
	{
	}
	void World::Clear()
	{
		m_bodies.clear();
	}
	void World::AddObject(int id, Rect bounds, CollisionType type)
	{
		m_bodies.push_back({ id, bounds, type });
	}
	void World::AddObject(const Body & body)
	{
		m_bodies.push_back(body);
	}
	Body& World::GetObject(int id)
	{
		for (int i = 0; i < m_bodies.size(); i++)
			if (m_bodies[i].Id == id)
				return m_bodies[i];
	}
	std::vector<Body>& World::GetObjects()
	{
		return m_bodies;
	}
	void World::RemoveObject(int id)
	{
		for (int i = 0; i < m_bodies.size(); i++)
			if (m_bodies[i].Id == id)
				m_bodies.erase(m_bodies.begin() + i);
	}
	Point World::Check(int steps, Rect bounds, Point goal)
	{
		return Check(steps, bounds, goal, nullptr);
	}
	Point World::Check(int steps, Rect bounds, Point goal, std::function<void(Body&)> func)
	{
		Rect intersect;

		float xInc = (goal.X - bounds.X) / steps;
		float yInc = (goal.Y - bounds.Y) / steps;

		for (int i = steps - 1; i >= 0; i--) {
			bounds.X += xInc;
			for each (Body b in m_bodies) {
				if (b.Type == CollisionType::None)
					continue;

				if (b.Bounds.Intersects(bounds, intersect)) {
					if (func != nullptr)
						func(b);

					if (b.Type != CollisionType::Solid)
						continue;

					float xInter = intersect.Width;
					float yInter = intersect.Height;

					if (xInter < yInter) {
						if (bounds.X < b.Bounds.X)
							xInter *= -1;

						bounds.X += xInter;

						break;
					}
				}
			}

			bounds.Y += yInc;
			for each (Body b in m_bodies) {
				if (b.Type == CollisionType::None)
					continue;

				if (b.Bounds.Intersects(bounds, intersect)) {
					if (func != nullptr)
						func(b);
					
					if (b.Type != CollisionType::Solid)
						continue;

					float xInter = intersect.Width;
					float yInter = intersect.Height;

					if (yInter < xInter) {
						if (bounds.Y < b.Bounds.Y)
							yInter *= -1;

						bounds.Y += yInter;

						break;
					}
				}
			}
		}

		return { bounds.X, bounds.Y };
	}
}