#ifndef __COLLY_H__
#define __COLLY_H__

#include <vector>
#include <functional>

namespace cl
{
	/*
		2D point structure
	*/
	struct Point
	{
		float X, Y;
	};



	/*
		Rectangle structure. A rectangle has a position(top-left corner)
		and Width and Height property.
	*/
	class Rect
	{
	public:
		float X, Y, Width, Height;

		Rect();
		Rect(float x, float y, float width, float height);

		// does this rectangle intersect another rectangle?
		bool Intersects(const Rect& other, Rect& intersection);

		// is this rectangle exactly the same as another rectangle (has same position and size)
		inline bool operator==(const Rect& r) { return X == r.X && Y == r.Y && Width == r.Width && Height == r.Height; }
	};



	/*
		Collision types - can be collided, is a 'pass-through' object or
		should be ignored. Later we can add other types of collision
		handling (example: one way blocks)
	*/
	enum class CollisionType
	{
		None,	// no collision will occur
		Solid,	// collision will be handled
		Cross	// used for coins and other pick ups
	};



	/*
		Body is just a rectangle with addition information. It can have its own ID or 
		some user data attached. It also has a collision type.
	*/
	struct Body
	{
		int Id;
		Rect Bounds;
		CollisionType Type;
		void* UserData;

		bool operator==(const Body& bdy) { return Id == bdy.Id && Bounds == bdy.Bounds && Type == bdy.Type && UserData == bdy.UserData; }
	};



	/*
		QuadTree is used to optimize collision checking in large non grid/tile-based
		worlds. Using quad tree we can gather only elements that need to be checked.
		Therefore, having 10, 1000 or 10000 objects wont slow our physics too much.
	*/
	class QuadTree
	{
	public:
		// the max capacity of this quad tree
		static constexpr int Capacity = 4;

		QuadTree(const Rect& bnds);
		~QuadTree();

		// delete all the data that was stored in this quadtree
		void Reset(const Rect& bnds);

		// insert an object in this quadtree
		void Insert(const Body& bdy);

		// get all the objects in a given range
		void Query(const Rect& bnd, std::vector<Body>& elements);

	private:
		// subdivide this node
		void m_subdivide();

		// add an element to an array
		void m_add(const Body& bdy);

		// list of our elements
		Body m_elements[QuadTree::Capacity];
		int m_capacity;

		// node bounds and child nodes
		Rect m_bounds;
		QuadTree * m_topLeft, *m_topRight, *m_bottomLeft, *m_bottomRight;
	};



	/*
		World is the main class. It keeps track of all the elements
		in the level and organizes them in the QuadTree (note: you
		must manually build the QuadTree by calling the UpdateQuadTree
		function).
	*/
	class World
	{
	public:
		World();

		// getting and adding an object
		inline std::vector<Body>& GetObjects() { return m_bodies; }
		inline void AddObject(int id, Rect bounds, CollisionType type, void* data = nullptr) { AddObject({ id, bounds, type, data }); }
		inline void AddObject(const Body& body) { m_bodies.push_back(body); }

		// reset the world
		void Clear();

		// build the tree
		void UpdateQuadTree();

		// get and remove objects with a given id
		std::vector<Body*> GetObjects(int id);
		void RemoveObjects(int id);

		// Check for the collision and calculate the aftermath position. You should use
		// multiple steps for larger differences between bodys current position and goal
		// position so that the object doesnt pass through other solid objects in this 
		// world. For example, if the difference between the goal and current horizontal
		// position was 10 and we did 2 steps, we would go 5 pixels left, check for collision,
		// go another 5 pixels left and then check for collision again. More checks = more precise
		// but also takes more CPU time.
		Point Check(int steps, Rect body, Point goal, std::function<void(Body&, World*)> func = nullptr);

		// get the instance of the quadtree
		inline QuadTree& Tree() { return m_tree; }

	private:
		// calculate min and max of element position
		Rect m_getBounds();

		// linear list of all elements + list of elements organized in a quad tree
		QuadTree m_tree;
		std::vector<Body> m_bodies;
	};



	/*
		GridWorld is a class similar to World and does almost everything similar to the
		World class except it is used for worlds where elements are organized in a grid.
		It should be used in tile worlds. GridWorld doesnt use QuadTree and it doesnt
		use cl::Body class. It only needs a 2D array of tile IDs and a function GetCollisionType
		which tells it whether a tile is CollisionType::Solid, CollisionType::Cross, etc...
	*/
	class GridWorld
	{
	public:
		// create a grid world with given width and height and cell size
		void Create(int width, int height, int cellW, int cellH);

		// set/get object on a given position
		inline void SetObject(int x, int y, int id) { m_grid[y][x] = id; }
		inline int GetObject(int x, int y) { return m_grid[y][x]; }

		// get world size
		inline int GetWidth() { return m_w; }
		inline int GetHeight() { return m_h; }

		// Check for collision between player and the grid world.
		// NOTE: read World::Check() comment to read about the "steps" parameter
		Point Check(int steps, Rect body, Point goal, std::function<void(int, int, int, bool, GridWorld*)> func = nullptr);

		// a "filter" which tells us certain CollisionType for each tile ID
		std::function<CollisionType(int)> GetCollisionType; 

	private:
		int m_w, m_h, m_cellW, m_cellH;
		std::vector<std::vector<int>> m_grid;
	};
}

#endif