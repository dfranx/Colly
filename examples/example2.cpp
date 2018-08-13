#include "Colly.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#define VELOCITY 256 // pixels per second
#define TILE_WIDTH 64
#define TILE_HEIGHT 64
#define MAP_WIDTH 12
#define MAP_HEIGHT 8
#define PHYSICS	1/60.0f	// do a physics tick every PHYSICS seconds

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
	float totalTime = 0;

	while (wnd.isOpen()) {
		while (wnd.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				wnd.close();
		}

		// place the tile with left mouse button and delete it with the right mouse button
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			sf::Vector2i mp = sf::Mouse::getPosition(wnd);
			mp.x /= TILE_WIDTH;
			mp.y /= TILE_HEIGHT;

			if (mp.x >= 0 && mp.x < MAP_WIDTH && mp.y >= 0 && mp.y < MAP_HEIGHT)
				world.SetObject(mp.x, mp.y, sf::Mouse::isButtonPressed(sf::Mouse::Left));
		}

		totalTime += clk.restart().asSeconds();

		while (totalTime >= PHYSICS) {
			// movement
			int dirX = sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A);
			int dirY = sf::Keyboard::isKeyPressed(sf::Keyboard::S) - sf::Keyboard::isKeyPressed(sf::Keyboard::W);

			sf::Vector2f pos = player.getPosition();
			sf::Vector2f goal(pos.x + dirX * PHYSICS * VELOCITY, pos.y + dirY * PHYSICS * VELOCITY);

			// check for the collision
			sf::FloatRect pBounds = player.getGlobalBounds();
			cl::Point res = world.Check(6, { pBounds.left, pBounds.top, pBounds.width, pBounds.height }, { goal.x, goal.y });
			player.setPosition(res.X, res.Y);

			// decrease time
			totalTime -= PHYSICS;
		}

		wnd.clear();
		// simple way of rendering a small map
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

