#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

class ParticleSystem : public sf::Drawable, public sf::Transformable {
private:
	struct Particle {
		sf::Vector2f velocity;
		sf::Time lifetime;
		sf::VertexArray vertices;    // quad shape array with 4 vertices around the particle as a sprite to load texture
		sf::Vector2f prev_pos[5];
	};
	typedef std::shared_ptr<Particle> ParticlePtr;
	std::vector<ParticlePtr> m_particles;
	typedef std::vector<ParticlePtr>::iterator ParticleIterator;
	
	// default initial count of particles
	const int COUNT = 1000;
	// SFML Vertex Array storing particle origin center position
	sf::VertexArray m_vertices;
	// count of particles
	unsigned int m_count;
	// gravity
	float m_gravity;
	// emit angle
	unsigned int m_emitAngle;
	// emit start angel
	float m_emitStart;
	// min speed
	float m_speed;
	// radius/size of particle/sprite
	float m_size;
	// to suggest whether texture is loaded
	bool textureLoaded;	
	// Life time of particles
	sf::Time m_lifetime;
	// Emitter position
	sf::Vector2f m_emitter;
	// render states to load texture
	sf::Texture m_texture;

	// Move the particle after respawning each time by the initial velocity and angle
	void resetParticle (std::size_t index) {
		// give a random velocity and lifetime to the particle
		float angle = (std::rand () % m_emitAngle) * 3.14f / 180.f + m_emitStart;
		float speed = (std::rand () % 50) + m_speed;
		m_particles[index]->velocity = sf::Vector2f (std::cos (angle) * speed, std::sin (angle) * speed);
		m_particles[index]->lifetime = sf::milliseconds ((std::rand () % (m_lifetime.asMilliseconds () / 2)) + m_lifetime.asMilliseconds () / 2);

		// reset the position of the corresponding vertex
		m_vertices[index].position = m_emitter;
		m_vertices[index].color = sf::Color::Color (std::rand () % 255, std::rand () % 255, std::rand () % 255, std::rand () % 255);

		if (textureLoaded) {
			m_particles[index]->vertices[0].position = m_emitter + sf::Vector2f (-m_size, -m_size);
			m_particles[index]->vertices[1].position = m_emitter + sf::Vector2f (m_size, -m_size);
			m_particles[index]->vertices[2].position = m_emitter + sf::Vector2f (-m_size, m_size);
			m_particles[index]->vertices[3].position = m_emitter + sf::Vector2f (m_size, m_size);
		}
	}

	// Apply the transform and draw the vertext array
	virtual void draw (sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= getTransform ();

		if (textureLoaded) {
			states.texture = &m_texture;
			for (std::size_t i = 0; i < m_particles.size (); ++i) {
				target.draw (m_particles[i]->vertices, states);
			}
		} else {
			states.texture = NULL;
			target.draw (m_vertices, states);
		}
	}

public:
	ParticleSystem () :
		m_vertices (sf::Points, COUNT),
		m_lifetime (sf::seconds (3)),
		m_emitter (0, 0),
		m_count (1000),
		m_gravity (0.f),
		m_emitAngle (360),
		m_emitStart (0.f),
		m_speed (50.f),
		m_size (0.f) {
		textureLoaded = m_texture.loadFromFile ("static/image/smokeparticle.png") ? true : false;
		//textureLoaded = false;
	}

	~ParticleSystem () {
		m_particles.clear ();
	}

	// Set the max life time of the particles
	void setLifetime (float lifetime) {
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

	// Set particle/sprite size
	void setSize (float size) {
		m_size = size;
	}

	// Update the lifetime, position, color, etc of the particles
	void update (sf::Time elapsed) {
		for (std::size_t i = 0; i < m_particles.size (); ++i) {
			// update the particle lifetime
			Particle& p = *m_particles[i];
			p.lifetime -= elapsed;

			// if the particle is dead, respawn it
			if (p.lifetime <= sf::Time::Zero)
				resetParticle (i);

			// update the position of the corresponding vertex
			float prevVelocity = p.velocity.y;
			p.velocity.y += m_gravity * elapsed.asSeconds ();
			m_vertices[i].position.x += p.velocity.x * elapsed.asSeconds ();
			m_vertices[i].position.y += (p.velocity.y + prevVelocity) / 2 * elapsed.asSeconds ();

			// update the alpha (transparency) of the particle according to its lifetime
			float ratio = p.lifetime.asSeconds () / m_lifetime.asSeconds ();
			m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);

			// update surrending vertices of shape
			if (textureLoaded) {
				sf::Vector2f m_pos = m_vertices[i].position;
				m_particles[i]->vertices[0].position = m_pos + sf::Vector2f (-m_size, -m_size);
				m_particles[i]->vertices[1].position = m_pos + sf::Vector2f (m_size, -m_size);
				m_particles[i]->vertices[2].position = m_pos + sf::Vector2f (-m_size, m_size);
				m_particles[i]->vertices[3].position = m_pos + sf::Vector2f (m_size, m_size);
			}			
		}
	}

	// get the count of particles
	int getCount () {
		return m_count;
	}

	// add the count of particles by 100 each time
	void addCount () {
		int diff = 100;
		Particle* particle;

		if (m_count >= UINT_MAX - 1) {
			diff = 0;
		} else if (m_count + 100 >= UINT_MAX) {
			diff = UINT_MAX - 1 - m_count;
		}

		// update particles and vertice
		while (diff > 0) {
			particle = new Particle ();
			sf::Vertex* point = new sf::Vertex;

			if (particle != nullptr && point != nullptr) {
				m_particles.push_back (ParticlePtr(particle));
				m_vertices.append (*point);
				m_count++;
				diff--;
			} else {
				throw std::runtime_error ("Failed to add particle");
			}
		}
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
		m_particles.resize (m_count - diff);
		m_vertices.resize (m_count - diff);
		m_count = m_particles.size ();
	}

	// initialize and build the sprite/quad vertices around the particle
	void init () {
		float width = m_texture.getSize ().x;
		float height = m_texture.getSize ().y;
		Particle* particle;

		for (int i = 0; i < COUNT; ++i) {
			particle = new Particle ();
			m_particles.push_back (ParticlePtr(particle));
			m_particles[i]->vertices.setPrimitiveType (sf::PrimitiveType::Quads);
			m_particles[i]->vertices.resize (4);
			m_particles[i]->vertices[0].texCoords = sf::Vector2f (0, 0);
			m_particles[i]->vertices[1].texCoords = sf::Vector2f (width, 0);
			m_particles[i]->vertices[2].texCoords = sf::Vector2f (width, height);
			m_particles[i]->vertices[3].texCoords = sf::Vector2f (0, height);
		}
	}
};