#include "Colly.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#define VELOCITY 256	// pixels per second
#define QUAD_COUNT 1000	// number of quads
#define QUAD_SIZE 8		// quad size in pixels
#define PHYSICS	1/60.0f	// do a physics tick every PHYSICS seconds

int main()
{
	srand(time(NULL));

	sf::RenderWindow wnd;
	wnd.create(sf::VideoMode(800, 600), "Colly");

	// create vertex buffer for our map
	sf::VertexBuffer level;
	level.setPrimitiveType(sf::PrimitiveType::Quads);
	level.setUsage(sf::VertexBuffer::Usage::Dynamic);
	level.create(4 * QUAD_COUNT);

	// vertices for our buffer
	sf::Vertex* verts = new sf::Vertex[QUAD_COUNT * 4];

	// declare a world and add some bodies to it
	cl::World world;
	for (int i = 0; i < QUAD_COUNT; i++) {
		cl::Rect quad((float)(rand() % 800), (float)(rand() % 600), QUAD_SIZE, QUAD_SIZE);
		verts[i * 4 + 0].position = sf::Vector2f(quad.X, quad.Y);
		verts[i * 4 + 1].position = sf::Vector2f(quad.X + quad.Width, quad.Y);
		verts[i * 4 + 2].position = sf::Vector2f(quad.X + quad.Width, quad.Y + quad.Height);
		verts[i * 4 + 3].position = sf::Vector2f(quad.X, quad.Y + quad.Height);
		world.AddObject(i, quad, cl::CollisionType::Solid);
	}
	level.update(verts);

	// we need to (re)build quadtree after updating the physics world
	world.UpdateQuadTree();

	// we dont need vertex data anymore
	delete verts;

	// player
	sf::RectangleShape player;
	player.setPosition(32, 32);
	player.setSize(sf::Vector2f(8, 8));
	player.setFillColor(sf::Color::Blue);

	sf::Event event;

	sf::Clock clk;
	float totalTime = 0;

	while (wnd.isOpen()) {
		while (wnd.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				wnd.close();
		}


		totalTime += clk.restart().asSeconds();

		// do movement every PHYSICS seconds
		while (totalTime >= PHYSICS) {
			// movement
			int dirX = sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A);
			int dirY = sf::Keyboard::isKeyPressed(sf::Keyboard::S) - sf::Keyboard::isKeyPressed(sf::Keyboard::W);

			sf::Vector2f pos = player.getPosition();
			sf::Vector2f goal(pos.x + dirX * PHYSICS * VELOCITY, pos.y + dirY * PHYSICS * VELOCITY);

			// collision check - NOTE: since we do this every PHYSICS seconds, we need to do more steps per collision check since we will have very large differences in positions (PHYSICS*VELOCITY)
			sf::FloatRect pBounds = player.getGlobalBounds();
			cl::Point res = world.Check(6, { pBounds.left, pBounds.top, pBounds.width, pBounds.height }, { goal.x, goal.y }, [](cl::Body& b, cl::World* world)
			{
				printf("Collided with: %d\n", b.Id);
			});
			player.setPosition(res.X, res.Y);

			// decrease time
			totalTime -= PHYSICS;
		}


		wnd.clear();

		wnd.draw(level);
		wnd.draw(player);

		wnd.display();
	}

	return 0;
}

