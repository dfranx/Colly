#include <Colly.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#define VELOCITY 256 // pixels per second
#define TILE_WIDTH 64
#define TILE_HEIGHT 64
#define MAP_WIDTH 12
#define MAP_HEIGHT 8

int main()
{
	sf::RenderWindow wnd;
	wnd.create(sf::VideoMode(800, 600), "Colly");

	// declare a world and add some bodies to it
	cl::GridWorld world;
	world.Create(MAP_WIDTH, MAP_HEIGHT, TILE_WIDTH, TILE_HEIGHT);

	// player
	sf::RectangleShape player;
	player.setPosition(32, 32);
	player.setSize(sf::Vector2f(TILE_WIDTH-1, TILE_HEIGHT-1));
	player.setFillColor(sf::Color::Blue);

	sf::Event event;

	sf::Clock clk;
	float clkTime;

	while (wnd.isOpen()) {
		while (wnd.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				wnd.close();
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			sf::Vector2i mp = sf::Mouse::getPosition(wnd);

			world.SetObject(mp.x / TILE_WIDTH, mp.y / TILE_HEIGHT, sf::Mouse::isButtonPressed(sf::Mouse::Left));
		}

		clkTime = clk.restart().asSeconds();

		/// MOVEMENT
		int dirX = sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A);
		int dirY = sf::Keyboard::isKeyPressed(sf::Keyboard::S) - sf::Keyboard::isKeyPressed(sf::Keyboard::W);

		sf::Vector2f pos = player.getPosition();
		sf::Vector2f goal(pos.x + dirX * clkTime * VELOCITY, pos.y + dirY * clkTime * VELOCITY);

		/// COLLISION CHECK
		sf::FloatRect pBounds = player.getGlobalBounds();
		cl::Point res = world.Check(1, { pBounds.left, pBounds.top, pBounds.width, pBounds.height }, { goal.x, goal.y });
		player.setPosition(res.X, res.Y);


		wnd.clear();
		for (int x = 0; x <MAP_WIDTH; x++) {
			for (int y = 0; y < MAP_HEIGHT; y++) {
				int id = world.GetObject(x, y);
				
				if (id == 0)
					continue;

				sf::RectangleShape rs;
				rs.setPosition(x*TILE_WIDTH, y*TILE_HEIGHT);
				rs.setSize(sf::Vector2f(TILE_WIDTH, TILE_HEIGHT));
				wnd.draw(rs);
			}
		}
		wnd.draw(player);
		wnd.display();
	}

	return 0;
}

