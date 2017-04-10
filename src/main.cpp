#include <SFML/Graphics.hpp>
#include <string>
#include <cassert>
#include "../include/ParticleSystem.h"

int main () {
	const int gameWidth = 800;
	const int gameHeight = 600;
	// create the window
	sf::RenderWindow window1 (sf::VideoMode (gameWidth, gameHeight), "ParticleSys1");
	//sf::RenderWindow window2 (sf::VideoMode (gameWidth, gameHeight), "ParticleSys2");

	// create the particle system
	ParticleSystem particles1;
	ParticleSystem particles2;

	// create a clock to track the elapsed time
	sf::Clock clock;
	sf::Clock clock2;

	sf::RectangleShape textField;
	textField.setFillColor (sf::Color::Black);
	textField.setSize (sf::Vector2f (800.f, 30.f));
	textField.setPosition (0, gameHeight - 30);

	// load fonts
	sf::Font fontHNL;	
	assert (fontHNL.loadFromFile ("static/font/HelveticaNeue Light.ttf"));

	sf::Text countText;
	countText.setFont (fontHNL);
	countText.setCharacterSize (20);
	countText.setFillColor (sf::Color::Green);
	std::string countStr = std::to_string (particles1.getCount ());
	countText.setString ("Particle count (Use UP/DOWN key to change): " + countStr);
	countText.setPosition (20, gameHeight - 30);
	
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

		// * make the particle system emitter follow the mouse
		sf::Vector2i mouse = sf::Mouse::getPosition (window1);
		particles1.setEmitter (window1.mapPixelToCoords (mouse));

		// update the particle system by time
		sf::Time elapsed = clock.restart ();
		particles1.update (elapsed);

		// render it
		window1.clear ();
		window1.draw (particles1);
		window1.draw (textField);
		window1.draw (countText);
		window1.display ();

		/*
		Window2
		*/
		// handle events
		//sf::Event event2;
		//while (window2.pollEvent (event2)) {
		//	switch (event.type) {
		//	case sf::Event::Closed:
		//		window1.close ();
		//		break;
		//	//case sf::Event::KeyPressed:

		//	}
		//}

		//// render it
		//window2.clear ();
		////window2.draw (terminal);
		//window2.display ();
	}

	return 0;
}