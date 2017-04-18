#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

struct Particle {
	sf::Vector2f velocity;
	sf::Time lifetime;
	sf::VertexArray vertices;    // particle origin center vertex and surrounding 4 vertices as a sprite to load texture
	sf::VertexArray history;     // trail vertices
	std::vector<sf::Time> h_time;    // history life time of trail vertices
	Particle () : lifetime (sf::seconds (3)), vertices (sf::Points, 5) {}
};
typedef Particle* ParticlePtr;