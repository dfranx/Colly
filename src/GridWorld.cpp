#include <Colly/GridWorld.h>
#include <algorithm>

namespace cl
{
	GridWorld::GridWorld()
	{
	}
	void GridWorld::Create(int width, int height, int cellW, int cellH)
	{
		m_cellH = cellH;
		m_cellW = cellW;
		m_w = width;
		m_h = height;

		m_grid.resize(m_h, std::vector<int>(m_w, 0));

		GetCollisionType = [](int id) -> CollisionType {
			if (id == 0)
				return CollisionType::None;
			return CollisionType::Solid;
		};
	}
	void GridWorld::SetObject(int x, int y, int id)
	{
		if (x < 0 || x >= m_w || y < 0 || y >= m_h)
			return;
		m_grid[y][x] = id;
	}
	int GridWorld::GetObject(int x, int y)
	{
		if (x < 0 || x >= m_w || y < 0 || y >= m_h)
			return -1;
		return m_grid[y][x];
	}
	Point GridWorld::Check(int steps, Rect bounds, Point goal)
	{
		return Check(steps, bounds, goal, nullptr);
	}
	Point GridWorld::Check(int steps, Rect bounds, Point goal, std::function<void(int)> func)
	{
		Rect intersect;

		float xInc = (goal.X - bounds.X) / steps;
		float yInc = (goal.Y - bounds.Y) / steps;

		int sX, sY, eX, eY;

		for (int i = steps - 1; i >= 0; i--) {
			bounds.X += xInc;
			sY = std::max(0, (int)(bounds.Y / m_cellH) - 1);
			eY = std::min(m_h - 1, (int)((bounds.Y+bounds.Height) / m_cellH) + 1);
			sX = std::max(0, (int)(bounds.X / m_cellW) - 1);
			eX = std::min(m_w - 1, (int)((bounds.X + bounds.Width) / m_cellW) + 1);

			for (int y = sY; y <= eY; y++) {
				for (int x = sX; x <= eX; x++) {
					int id = m_grid[y][x];
					CollisionType type = GetCollisionType(id);
					Rect cell(x*m_cellW, y*m_cellH, m_cellW, m_cellH);

					if (type == CollisionType::None)
						continue;

					if (cell.Intersects(bounds, intersect)) {
						if (func != nullptr)
							func(id);

						if (type != CollisionType::Solid)
							continue;

						float xInter = intersect.Width;
						float yInter = intersect.Height;

						if (xInter < yInter) {
							if (bounds.X < cell.X)
								xInter *= -1;

							bounds.X += xInter;

							break;
						}
					}
				}
			}



			bounds.Y += yInc;
			sY = std::max(0, (int)(bounds.Y / m_cellH) - 1);
			eY = std::min(m_h - 1, (int)((bounds.Y + bounds.Height) / m_cellH) + 1);
			sX = std::max(0, (int)(bounds.X / m_cellW) - 1);
			eX = std::min(m_w - 1, (int)((bounds.X + bounds.Width) / m_cellW) + 1);

			for (int y = sY; y <= eY; y++) {
				for (int x = sX; x <= eX; x++) {
					int id = m_grid[y][x];
					CollisionType type = GetCollisionType(id);
					Rect cell(x*m_cellW, y*m_cellH, m_cellW, m_cellH);

					if (type == CollisionType::None)
						continue;

					if (cell.Intersects(bounds, intersect)) {
						if (func != nullptr)
							func(id);

						if (type == CollisionType::Cross)
							continue;

						float xInter = intersect.Width;
						float yInter = intersect.Height;

						if (yInter < xInter) {
							if (bounds.Y < cell.Y)
								yInter *= -1;

							bounds.Y += yInter;

							break;
						}
					}
				}
			}
		}

		return { bounds.X, bounds.Y };
	}
}
