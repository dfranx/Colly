#include "Colly.h"
#include <algorithm>

namespace cl
{
	Rect::Rect()
	{
		X = Y = Width = Height = 0;
	}
	Rect::Rect(float x, float y, float w, float h)
	{
		X = x;
		Y = y;
		Width = w;
		Height = h;
	}
	bool Rect::Intersects(const Rect& o, Rect& intersection)
	{
		// following code is from: https://github.com/SFML/SFML/blob/247b03172c34f25a808bcfdc49f390d619e7d5e0/include/SFML/Graphics/Rect.inl#L109

		// Compute the min and max of the first rectangle on both axes
		float minX1 = std::min(X, X + Width);
		float maxX1 = std::max(X, X + Width);
		float minY1 = std::min(Y, Y + Height);
		float maxY1 = std::max(Y, Y + Height);


		// Compute the min and max of the second rectangle on both axes
		float minX2 = std::min(o.X, o.X + o.Width);
		float maxX2 = std::max(o.X, o.X + o.Width);
		float minY2 = std::min(o.Y, o.Y + o.Height);
		float maxY2 = std::max(o.Y, o.Y + o.Height);


		// Compute the intersection boundaries
		float interLeft = std::max(minX1, minX2);
		float interTop = std::max(minY1, minY2);
		float interRight = std::min(maxX1, maxX2);
		float interBottom = std::min(maxY1, maxY2);


		// If the intersection is valid (positive non zero area), then there is an intersection
		if (interLeft < interRight && interTop < interBottom) {
			intersection = Rect(interLeft, interTop, interRight - interLeft, interBottom - interTop);
			return true;
		}

		intersection = Rect(0, 0, 0, 0);
		return false;
	}



	QuadTree::QuadTree(const Rect& bnds)
	{
		m_bounds = bnds;
		m_capacity = 0;
		m_topLeft = nullptr;
		m_topRight = nullptr;
		m_bottomLeft = nullptr;
		m_bottomRight = nullptr;
	}
	QuadTree::~QuadTree()
	{
		delete m_topLeft;
		delete m_topRight;
		delete m_bottomLeft;
		delete m_bottomRight;
	}
	void QuadTree::Reset(const Rect & bnds)
	{
		m_bounds = bnds;
		m_capacity = 0;

		if (m_topLeft != nullptr) {
			delete m_topLeft;
			delete m_topRight;
			delete m_bottomLeft;
			delete m_bottomRight;
			m_topLeft = nullptr;
			m_topRight = nullptr;
			m_bottomLeft = nullptr;
			m_bottomRight = nullptr;
		}
	}
	void QuadTree::Insert(const Body& bdy)
	{
		Rect intersectRect; // not used

		if (!m_bounds.Intersects(bdy.Bounds, intersectRect))
			return;

		// do we have enough space? if yes, just add it to this node
		if (m_capacity < QuadTree::Capacity) {
			m_add(bdy);
			return;
		}

		// subdivide if we havent already
		if (m_topLeft == nullptr)
			m_subdivide();

		// insert into all regions that intersect with the rectangle
		m_topLeft->Insert(bdy);
		m_topRight->Insert(bdy);
		m_bottomLeft->Insert(bdy);
		m_bottomRight->Insert(bdy);
	}
	void QuadTree::Query(const Rect& bnd, std::vector<Body>& elements)
	{
		Rect intersectRect; // not used

		// check if our bounds intersect with the quad tree node
		if (!m_bounds.Intersects(bnd, intersectRect))
			return;

		// get all elements in this node and check if they intersect with our bounds
		for (int i = 0; i < m_capacity; i++) {
			if (m_elements[i].Type != CollisionType::None &&
				m_elements[i].Bounds.Intersects(bnd, intersectRect)) {

				bool unique = true;
				for (int j = 0; j < elements.size(); j++)
					if (elements[j] == m_elements[i]) {
						unique = false;
						break;
					}

				if (unique) elements.push_back(m_elements[i]);
			}
		}

		// just exit if we have no leaves
		if (m_topLeft == nullptr)
			return;

		// query all the regions
		m_topLeft->Query(bnd, elements);
		m_topRight->Query(bnd, elements);
		m_bottomLeft->Query(bnd, elements);
		m_bottomRight->Query(bnd, elements);
	}
	void QuadTree::m_subdivide()
	{
		m_topLeft = new QuadTree(Rect(m_bounds.X, m_bounds.Y, m_bounds.Width / 2, m_bounds.Height / 2));
		m_topRight = new QuadTree(Rect(m_bounds.X + m_bounds.Width / 2, m_bounds.Y, m_bounds.Width / 2, m_bounds.Height / 2));
		m_bottomLeft = new QuadTree(Rect(m_bounds.X, m_bounds.Y + m_bounds.Height / 2, m_bounds.Width / 2, m_bounds.Height / 2));
		m_bottomRight = new QuadTree(Rect(m_bounds.X + m_bounds.Width / 2, m_bounds.Y + m_bounds.Height / 2, m_bounds.Width / 2, m_bounds.Height / 2));
	}
	void QuadTree::m_add(const Body& element)
	{
		m_elements[m_capacity] = element;
		m_capacity++;
	}



	World::World() :
		m_tree(Rect())
	{}
	void World::UpdateQuadTree()
	{
		m_tree.Reset(m_getBounds());
		for (const Body& b : m_bodies)
			m_tree.Insert(b);
	}
	void World::Clear()
	{
		m_bodies.clear();
		UpdateQuadTree();
	}
	std::vector<Body*> World::GetObjects(int id)
	{
		std::vector<Body*> ret;
		for (int i = 0; i < m_bodies.size(); i++)
			if (m_bodies[i].Id == id)
				ret.push_back(&m_bodies[i]);
		return ret;
	}
	void World::RemoveObjects(int id)
	{
		for (int i = 0; i < m_bodies.size(); i++)
			if (m_bodies[i].Id == id)
				m_bodies.erase(m_bodies.begin() + i);
	}
	Point World::Check(int steps, Rect bounds, Point goal, std::function<void(Body&, World*)> func)
	{
		Rect intersect;

		// the increment per axis for each step
		float xInc = (goal.X - bounds.X) / steps;
		float yInc = (goal.Y - bounds.Y) / steps;

		// calculate quadtree region
		Rect checkRegion(std::min(goal.X, bounds.X), std::min(goal.Y, bounds.Y), std::max(goal.X, bounds.X + bounds.Width), std::max(goal.Y, bounds.Y + bounds.Height));
		checkRegion.X -= bounds.Width;
		checkRegion.Y -= bounds.Height;
		checkRegion.Width = (checkRegion.Width + bounds.Width) - checkRegion.X;
		checkRegion.Height = (checkRegion.Height + bounds.Height) - checkRegion.Y;

		// get only the bodies we have to check collision with
		std::vector<Body> bodies;
		m_tree.Query(checkRegion, bodies);

		// go thru each step
		for (int i = 0; i < steps; i++) {
			// increment along x axis and check for collision
			bounds.X += xInc;
			for (Body b : bodies) {
				if (b.Type == CollisionType::None) // no collision checking needed? just skip the body
					continue;

				// does our bound intersect with the body
				if (b.Bounds.Intersects(bounds, intersect)) {
					// call the user function
					if (func != nullptr)
						func(b, this);

					// only check for collision if we encountered a solid object
					if (b.Type != CollisionType::Solid)
						continue;

					float xInter = intersect.Width;
					float yInter = intersect.Height;

					if (xInter < yInter) {
						if (bounds.X < b.Bounds.X)
							xInter *= -1; // bounce in the direction that depends on the body and user position

						bounds.X += xInter; // move the user back

						break;
					}
				}
			}

			// increment along y axis and check for collision - repeat everything for Y axis
			bounds.Y += yInc;
			for (Body b : bodies) {
				if (b.Type == CollisionType::None)
					continue;

				if (b.Bounds.Intersects(bounds, intersect)) {
					if (func != nullptr)
						func(b, this);

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
	Rect World::m_getBounds()
	{
		// if there are any elements in this world, get the min and max positions
		if (m_bodies.size() > 0) {
			Rect ret(m_bodies[0].Bounds.X, m_bodies[0].Bounds.Y, m_bodies[0].Bounds.X, m_bodies[0].Bounds.Y);
			for (const Body& b : m_bodies) {
				ret.X = std::min(b.Bounds.X, ret.X);
				ret.Y = std::min(b.Bounds.Y, ret.Y);
				ret.Width = std::max(b.Bounds.X + b.Bounds.Width, ret.Width);
				ret.Height = std::max(b.Bounds.Y + b.Bounds.Height, ret.Height);
			}

			return Rect(ret.X, ret.Y, ret.Width - ret.X, ret.Height - ret.Y);
		}

		// else return 0px region
		return Rect(0, 0, 0, 0);
	}



	void GridWorld::Create(int width, int height, int cellW, int cellH)
	{
		m_cellH = cellH;
		m_cellW = cellW;
		m_w = width;
		m_h = height;

		// create the gird
		m_grid.resize(m_h, std::vector<int>(m_w, 0));

		// default filter - ID == 0 -> no collision else sold object
		GetCollisionType = [](int id) -> CollisionType
		{
			if (id == 0)
				return CollisionType::None;
			return CollisionType::Solid;
		};
	}
	Point GridWorld::Check(int steps, Rect bounds, Point goal, std::function<void(int, int, int, bool, GridWorld*)> func)
	{
		Rect intersect;

		// the increment per axis for each step
		float xInc = (goal.X - bounds.X) / steps;
		float yInc = (goal.Y - bounds.Y) / steps;

		// calculate subregion that needs to be checked
		Rect checkRegion(std::min(goal.X, bounds.X), std::min(goal.Y, bounds.Y), std::max(goal.X, bounds.X + bounds.Width), std::max(goal.Y, bounds.Y + bounds.Height));
		checkRegion.X = std::max(0, (int)(checkRegion.X - bounds.Width) / m_cellW);
		checkRegion.Y = std::max(0, (int)(checkRegion.Y - bounds.Height) / m_cellH);
		checkRegion.Width = std::min((int)(checkRegion.Width + bounds.Width) / m_cellW, m_w - 1);
		checkRegion.Height = std::min((int)(checkRegion.Height + bounds.Height) / m_cellH, m_h - 1);

		for (int i = 0; i < steps; i++) {
			// increment along x axis and check for collision
			bounds.X += xInc;
			for (int y = (int)checkRegion.Y; y <= (int)checkRegion.Height; y++) {
				for (int x = (int)checkRegion.X; x <= (int)checkRegion.Width; x++) {
					int id = m_grid[y][x];
					CollisionType type = GetCollisionType(id); // fetch the collision type through the filter GetCollisionType

					if (type == CollisionType::None) // no collision checking needed? just skip the body
						continue;

					Rect cell(x*m_cellW, y*m_cellH, m_cellW, m_cellH);

					if (cell.Intersects(bounds, intersect)) {
						// call the user function
						if (func != nullptr)
							func(id, x, y, true, this);

						// only check for collision if we encountered a solid object
						if (type != CollisionType::Solid)
							continue;

						float xInter = intersect.Width;
						float yInter = intersect.Height;

						if (xInter < yInter) {
							if (bounds.X < cell.X)
								xInter *= -1; // "bounce" in the direction that depends on the body and user position

							bounds.X += xInter; // move the user back

							break;
						}
					}
				}
			}

			// increment along y axis and check for collision - repeat everything for Y axis
			bounds.Y += yInc;
			for (int y = (int)checkRegion.Y; y <= (int)checkRegion.Height; y++) {
				for (int x = (int)checkRegion.X; x <= (int)checkRegion.Width; x++) {
					int id = m_grid[y][x];
					CollisionType type = GetCollisionType(id);

					if (type == CollisionType::None)
						continue;

					Rect cell(x*m_cellW, y*m_cellH, m_cellW, m_cellH);

					if (cell.Intersects(bounds, intersect)) {
						if (func != nullptr)
							func(id, x, y, false, this);

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