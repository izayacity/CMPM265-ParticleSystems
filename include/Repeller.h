#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include "Particle.h"

class Repeller : public sf::Drawable, public sf::Transformable {
private:
	sf::CircleShape circle;
	sf::Vector2f r_position;
	float r_strength;

	virtual void draw (sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= getTransform ();
		states.texture = NULL;
		target.draw (circle, states);
	}
public:
	Repeller () {
		circle.setRadius (10.f);
		circle.setOrigin (sf::Vector2f (10.f, 10.f));
		circle.setFillColor (sf::Color::Red);		
	}

	void setPosition (sf::Vector2f position) {
		r_position = position;
		circle.setPosition (r_position);
	}

	void setStrength (float strength) {
		r_strength = strength;
	}

	sf::Vector2f getPosition () {
		return r_position;
	}

	float getStrength () {
		return r_strength;
	}

	sf::Vector2f repel (ParticlePtr particle) {
		sf::Vector2f dis_vec = particle->vertices[0].position - r_position;
		float distance = sqrt (dis_vec.x * dis_vec.x + dis_vec.y * dis_vec.y);
		return sf::Vector2f (r_strength / (distance * distance) * (dis_vec.x / distance),
			r_strength / (distance * distance) * (dis_vec.y / distance));
	}
};