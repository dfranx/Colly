#include <Colly.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#define VELOCITY 256 // pixels per second

int main()
{
	sf::RenderWindow wnd;
	wnd.create(sf::VideoMode(800, 600), "Colly");

	// declare a world and add some bodies to it
	cl::World world;
	world.AddObject(0, { 150, 150, 50, 50 }, cl::CollisionType::Solid);
	world.AddObject(0, { 250, 250, 100, 25 }, cl::CollisionType::Solid);
	world.AddObject(1, { 500, 200, 25, 100 }, cl::CollisionType::Solid);
	world.AddObject(2, { 150, 500, 125, 50 }, cl::CollisionType::Solid);
	world.AddObject(3, { 420, 340, 25, 100 }, cl::CollisionType::Cross);

	// player
	sf::RectangleShape player;
	player.setPosition(32, 32);
	player.setSize(sf::Vector2f(64, 64));
	player.setFillColor(sf::Color::Blue);

	sf::Event event;

	sf::Clock clk;
	float clkTime;

	while (wnd.isOpen()) {
		while (wnd.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				wnd.close();
		}


		clkTime = clk.restart().asSeconds();

		/// MOVEMENT
		int dirX = sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A);
		int dirY = sf::Keyboard::isKeyPressed(sf::Keyboard::S) - sf::Keyboard::isKeyPressed(sf::Keyboard::W);

		sf::Vector2f pos = player.getPosition();
		sf::Vector2f goal(pos.x + dirX * clkTime * VELOCITY, pos.y + dirY * clkTime * VELOCITY);

		/// COLLISION CHECK
		sf::FloatRect pBounds = player.getGlobalBounds();
		cl::Point res = world.Check(1, { pBounds.left, pBounds.top, pBounds.width, pBounds.height }, { goal.x, goal.y }, [](cl::Body& b, cl::World* world) {
			printf("Collided with: %d\n", b.Id);
		});
		player.setPosition(res.X, res.Y);


		wnd.clear();
		for each (cl::Body b in world.GetObjects()) {
			sf::RectangleShape rs;
			rs.setPosition(b.Bounds.X, b.Bounds.Y);
			rs.setSize(sf::Vector2f(b.Bounds.Width, b.Bounds.Height));
			wnd.draw(rs);
		}
		wnd.draw(player);
		wnd.display();
	}

	return 0;
}

