#pragma once
#include <SFML/Graphics.hpp>

class ParticleSystem : public sf::Drawable, public sf::Transformable {
private:
	struct Particle {
		sf::Vector2f velocity;
		sf::Time lifetime;
	};

	// default initial count of particles
	const int COUNT = 1000;
	// count of particles
	unsigned int m_count;
	// gravity
	float m_gravity;
	// emit angle
	unsigned int m_emitAngle;
	// emit start angle
	float m_emitStart;
	// min speed
	float m_speed;

	// Particle STD vector storing the velocity and life time
	std::vector<Particle> m_particles;
	// SFML Vertex Array storing point shape
	sf::VertexArray m_vertices;
	// Life time of particles
	sf::Time m_lifetime;
	// Emitter position
	sf::Vector2f m_emitter;

	// Move the particle after respawning each time by the initial velocity and angle
	void resetParticle (std::size_t index) {
		// give a random velocity and lifetime to the particle
		float angle = (std::rand () % m_emitAngle) * 3.14f / 180.f + m_emitStart;
		float speed = (std::rand () % 50) + m_speed;
		m_particles[index].velocity = sf::Vector2f (std::cos (angle) * speed, std::sin (angle) * speed);
		m_particles[index].lifetime = sf::milliseconds ((std::rand () % (m_lifetime.asMilliseconds () / 2)) + m_lifetime.asMilliseconds () / 2);

		// reset the position of the corresponding vertex
		m_vertices[index].position = m_emitter;
		m_vertices[index].color = sf::Color::Color (std::rand () % 255, std::rand () % 255, std::rand () % 255, std::rand () % 255);
	}

	// Apply the transform and draw the vertext array
	virtual void draw (sf::RenderTarget& target, sf::RenderStates states) const {
		// apply the transform
		states.transform *= getTransform ();

		// our particles don't use a texture
		states.texture = NULL;

		// draw the vertex array
		target.draw (m_vertices, states);
	}

public:
	ParticleSystem () :
		m_particles (COUNT),
		m_vertices (sf::Points, COUNT),
		m_lifetime (sf::seconds (3)),
		m_emitter (0, 0),
		m_count (1000),
		m_gravity (0.f),
		m_emitAngle (360),
		m_emitStart (0.f),
		m_speed (50.f) {
	}

	// Set the max life time of the particles
	void setLifetime (int lifetime) {
		m_lifetime = sf::seconds (lifetime);
	}

	// Set the location of the emitter
	void setEmitter (sf::Vector2f position) {
		m_emitter = position;
	}

	// Set the gravity of particles
	void setGravity (float gravity) {
		m_gravity = gravity;
	}

	// Set the emit start angle of particles
	void setEmitStart (float emitStart) {
		m_emitStart = emitStart;
	}

	// Set the range of emit angle of particles
	void setEmitAngle (int emitAngle) {
		m_emitAngle = emitAngle;
	}

	// Set the min speed of particles
	void setSpeed (float speed) {
		m_speed = speed;
	}

	// Update the lifetime, position, color, etc of the particles
	void update (sf::Time elapsed) {
		for (std::size_t i = 0; i < m_particles.size (); ++i) {
			// update the particle lifetime
			Particle& p = m_particles[i];
			p.lifetime -= elapsed;

			// if the particle is dead, respawn it
			if (p.lifetime <= sf::Time::Zero)
				resetParticle (i);

			// update the position of the corresponding vertex
			m_vertices[i].position += p.velocity * elapsed.asSeconds ();

			// update the alpha (transparency) of the particle according to its lifetime
			float ratio = p.lifetime.asSeconds () / m_lifetime.asSeconds ();
			m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
		}
	}

	// get the count of particles
	int getCount () {
		return m_count;
	}

	// add the count of particles by 100 each time
	void addCount () {
		int diff = 100;

		if (m_count >= UINT_MAX - 1) {
			diff = 0;
		} else if (m_count + 100 >= UINT_MAX) {
			diff = UINT_MAX - 1 - m_count;
		}

		// update particles and vertice
		m_vertices.resize (m_count + diff);
		m_particles.resize (m_count + diff);
		m_count = m_vertices.getVertexCount ();
	}

	// reduce the count of particles by 100 each time
	void reduceCount () {
		int diff = 100;

		if (m_count <= 0) {
			diff = 0;
		} else if (m_count - 100 <= 0) {
			diff = m_count;
		}

		// update particles and vertice
		m_vertices.resize (m_count - diff);
		m_particles.resize (m_count - diff);
		m_count = m_vertices.getVertexCount ();
	}
};