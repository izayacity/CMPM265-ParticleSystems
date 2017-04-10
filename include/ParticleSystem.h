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
	unsigned int count;
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
		float angle = (std::rand () % 360) * 3.14f / 180.f;
		float speed = (std::rand () % 50) + 50.f;
		m_particles[index].velocity = sf::Vector2f (std::cos (angle) * speed, std::sin (angle) * speed);
		m_particles[index].lifetime = sf::milliseconds ((std::rand () % 2000) + 1000);

		// reset the position of the corresponding vertex
		m_vertices[index].position = m_emitter;
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
		m_emitter (0, 0) {
		count = 1000;
	}

	// Set the location of the emitter
	void setEmitter (sf::Vector2f position) {
		m_emitter = position;
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
		return count;
	}

	// add the count of particles by 100 each time
	void addCount () {
		int diff = 100;

		if (count >= UINT_MAX - 1) {
			diff = 0;
		} else if (count + 100 >= UINT_MAX) {
			diff = UINT_MAX - 1 - count;
		}

		// update particles and vertice
		m_vertices.resize (count + diff);
		m_particles.resize (count + diff);
		count = m_vertices.getVertexCount ();

		/*while (diff > 0) {
			Particle* particle = new Particle;
			sf::Vertex* point = new sf::Vertex;

			if (particle != nullptr && point != nullptr) {
				m_particles.push_back (*particle);
				m_vertices.append (*point);
				count++;
				diff--;
			} else {
				throw std::runtime_error ("Failed to add particle");
			}
		}*/
	}

	// reduce the count of particles by 100 each time
	void reduceCount () {
		int diff = 100;

		if (count <= 0) {
			diff = 0;
		} else if (count - 100 <= 0) {
			diff = count;
		}

		// update particles and vertice
		m_vertices.resize (count - diff);
		m_particles.resize (count - diff);
		count = m_vertices.getVertexCount ();
	}
};