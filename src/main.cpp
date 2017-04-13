#include <SFML/Graphics.hpp>
#include <string>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include "../include/ParticleSystem.h"

int main () {
	const int gameWidth = 800;
	const int gameHeight = 600;
	// create the window
	sf::RenderWindow window1 (sf::VideoMode (gameWidth, gameHeight), "ParticleSys");	
	window1.setFramerateLimit (60); // call it once, after creating the window
	std::srand (static_cast<unsigned int>(std::time (NULL)));

	// create the particle system
	ParticleSystem particles1;    // particle sys 1: emitter changed by mouse cursor
	ParticleSystem particles2;    // particle sys 2: emitter at [gameWidth / 2.f, 0.f], angle [120, 240], gravity = 100, textured

	// create a clock to track the elapsed time
	sf::Clock clock;

	// load fonts
	sf::Font fontHNL;
	assert (fontHNL.loadFromFile ("static/font/HelveticaNeue Light.ttf"));

	// draw the background of text field
	sf::RectangleShape textField;
	textField.setFillColor (sf::Color::Black);
	textField.setSize (sf::Vector2f (gameWidth + 0.f, 30.f));
	textField.setPosition (0, gameHeight - 30);

	// TODO: Simulate text box input by text cursor
	// display the text
	sf::Text countText;
	countText.setFont (fontHNL);
	countText.setCharacterSize (20);
	countText.setFillColor (sf::Color::Green);
	std::string countStr = std::to_string (particles1.getCount ());
	countText.setString ("Particle count (Use UP/DOWN key to change): " + countStr);
	countText.setPosition (20, gameHeight - 30);
	
	// init particle sys 2
	particles2.setEmitter (sf::Vector2f (gameWidth / 2.f, 10.f));
	particles2.setEmitAngle (120);
	particles2.setEmitStart (120.f);
	particles2.setLifetime (10);
	particles2.setSpeed (50.f);
	particles2.setGravity (100.f);
	particles2.setSize (2.f);
	particles2.init ();
	particles1.setSize (2.f);
	particles1.init ();

	// run the main loop
	while (window1.isOpen ()) {
		// handle events
		sf::Event event;
		while (window1.pollEvent (event)) {
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
				window1.close ();
			} else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Up)) {
				particles1.addCount ();
				countStr = std::to_string (particles1.getCount ());
				countText.setString ("Particle count (Use UP/DOWN key to change): " + countStr);
			} else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Down)) {
				particles1.reduceCount ();
				countStr = std::to_string (particles1.getCount ());
				countText.setString ("Particle count (Use UP/DOWN key to change): " + countStr);
			}
		}

		// set the emitter of particle sys 1 by mouse cursors
		sf::Vector2i mouse = sf::Mouse::getPosition (window1);
		particles1.setEmitter (window1.mapPixelToCoords (mouse));				

		// update the particle system by time
		sf::Time elapsed = clock.restart ();
		particles1.update (elapsed);
		particles2.update (elapsed);

		// render it
		window1.clear ();
		window1.draw (particles2);
		window1.draw (particles1);
		window1.draw (textField);
		window1.draw (countText);
		window1.display ();
	}

	return 0;
}