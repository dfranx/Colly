#ifndef __COLLY_GRID_WORLD_H__
#define __COLLY_GRID_WORLD_H__

#include <vector>
#include <functional>
#include <Colly/Body.h>

namespace cl
{
	class GridWorld
	{
	public:
		GridWorld();

		void Create(int width, int height, int cellW, int cellH);

		void SetObject(int x, int y, int id);
		int GetObject(int x, int y);

		Point Check(int steps, Rect body, Point goal);
		Point Check(int steps, Rect body, Point goal, std::function<void(int)> func);

		std::function<CollisionType(int)> GetCollisionType; // filter which tells us if given id represents a solid/whatever cell
	private:
		int m_w, m_h, m_cellW, m_cellH;
		std::vector<std::vector<int>> m_grid;
	};
}

#endif