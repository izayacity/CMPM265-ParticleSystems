#include <SFML/Graphics.hpp>
#include <string>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include "../include/ParticleSystem.h"
#include "../include/Repeller.h"

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
	Repeller repeller;

	sf::Clock clock1;  // clock of particle sys 1
	sf::Clock clock2;  // clock of particle sys 2

	// load fonts
	sf::Font fontHNL;
	assert (fontHNL.loadFromFile ("static/font/HelveticaNeue Light.ttf"));

	// draw the background of text field
	sf::RectangleShape textField;
	textField.setFillColor (sf::Color::Black);
	textField.setSize (sf::Vector2f (gameWidth + 0.f, 60.f));
	textField.setPosition (0, gameHeight - 60);

	// TODO: Simulate text box input by text cursor
	// display the text
	sf::Text countText;
	countText.setFont (fontHNL);
	countText.setCharacterSize (20);
	countText.setFillColor (sf::Color::Green);
	std::string countStr = std::to_string (particles1.getCount ());
	countText.setString ("Particle count (Up/Down key to change): " + countStr);
	countText.setPosition (20, gameHeight - 30);

	sf::Text repellerText;
	repellerText.setFont (fontHNL);
	repellerText.setCharacterSize (20);
	repellerText.setFillColor (sf::Color::Green);
	repellerText.setString ("Repeller: WASD to change location, Left/Right key to change strength");
	repellerText.setPosition (20, gameHeight - 60);
	
	// init particle sys
	particles2.setEmitter (sf::Vector2f (gameWidth / 2.f, 10.f));
	particles2.setEmitAngle (120);
	particles2.setEmitStart (120.f);
	particles2.setLifetime (10);
	particles2.setSpeed (50.f);
	particles2.setGravity (100.f);
	particles2.setSize (10.f);
	particles2.init (2);
	particles1.setSize (10.f);
	particles1.init (1);

	// init repeller
	repeller.setPosition (sf::Vector2f(gameWidth / 2.f, gameHeight / 2.f));
	repeller.setStrength (1000000.f);

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
			} else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::W)) {
				repeller.setPosition (sf::Vector2f (repeller.getPosition ().x,
					repeller.getPosition().y >= 10 ? repeller.getPosition ().y - 10.f: 0.f));
			} else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::A)) {
				repeller.setPosition (sf::Vector2f (repeller.getPosition ().x >= 10 ? repeller.getPosition ().x - 10.f : 0.f,
					repeller.getPosition ().y));
			} else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::S)) {
				repeller.setPosition (sf::Vector2f (repeller.getPosition ().x,
					repeller.getPosition ().y <= gameHeight - 10 ? repeller.getPosition ().y + 10.f : gameHeight + 0.f));
			} else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::D)) {
				repeller.setPosition (sf::Vector2f (repeller.getPosition ().x <= gameWidth - 10 ? repeller.getPosition ().x + 10.f : gameWidth + 0.f,
					repeller.getPosition ().y));
			} else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Left)) {
				repeller.setStrength (repeller.getStrength() >= 100000 ? repeller.getStrength () - 100000.f : 0.f);
			} else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Right)) {
				repeller.setStrength (repeller.getStrength () + 100000.f);
			}
		}
	
		// set the emitter of particle sys 1 by mouse cursors
		sf::Vector2i mouse = sf::Mouse::getPosition (window1);
		particles1.setEmitter (window1.mapPixelToCoords (mouse));				

		// update the particle system by time and apply force from repeller to particle systems
		sf::Time elapsed1 = clock1.restart ();
		particles1.applyRepeller (repeller, elapsed1);
		particles1.update (elapsed1);
		
		sf::Time elapsed2 = clock2.restart ();
		particles2.applyRepeller (repeller, elapsed2);
		particles2.update (elapsed2);

		// render it
		window1.clear ();
		window1.draw (repeller);
		window1.draw (particles2);
		window1.draw (particles1);
		window1.draw (textField);
		window1.draw (repellerText);
		window1.draw (countText);
		window1.display ();
	}

	return 0;
}