#pragma once
#include <SFML/Graphics.hpp>
#include <cassert>
#include <vector>
#include <memory>
#include <iostream>

struct Particle {
	sf::Vector2f velocity;
	sf::Time lifetime;
	sf::VertexArray vertices;    // particle origin center vertex and surrounding 4 vertices as a sprite to load texture
	sf::VertexArray history;     // trail vertices
	std::vector<sf::Time> h_time;    // history life time of trail vertices
	Particle () : lifetime (sf::seconds (3)), vertices (sf::Points, 5) {}
};
typedef std::shared_ptr<Particle> ParticlePtr;

class ParticleSystem : public sf::Drawable, public sf::Transformable {
private:
	std::vector<ParticlePtr> m_particles;
	std::map<int, sf::Texture> tex_map;

	// default initial count of particles
	const int COUNT = 100;
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
	// Life time of particles
	sf::Time m_lifetime;
	// Emitter position
	sf::Vector2f m_emitter;
	// render states to load texture
	sf::Texture m_texture;
	// pixel width of texture image
	float t_width;
	// pixel height of texture image
	float t_height;

	// Move the particle after respawning each time by the initial velocity and angle
	void resetParticle (std::size_t index) {
		// give a random velocity and lifetime to the particle
		float angle = (std::rand () % m_emitAngle) * 3.14f / 180.f + m_emitStart;
		float speed = (std::rand () % 50) + m_speed;
		m_particles[index]->velocity = sf::Vector2f (std::cos (angle) * speed, std::sin (angle) * speed);
		m_particles[index]->lifetime = sf::milliseconds ((std::rand () % (m_lifetime.asMilliseconds () / 2)) + m_lifetime.asMilliseconds () / 2);
		m_particles[index]->history.resize (0);
		m_particles[index]->h_time.clear ();

		// reset the position of the corresponding vertex
		m_particles[index]->vertices[0].position = m_emitter;
		m_particles[index]->vertices[1].position = m_emitter + sf::Vector2f (-m_size, -m_size);
		m_particles[index]->vertices[2].position = m_emitter + sf::Vector2f (m_size, -m_size);
		m_particles[index]->vertices[3].position = m_emitter + sf::Vector2f (-m_size, m_size);
		m_particles[index]->vertices[4].position = m_emitter + sf::Vector2f (m_size, m_size);		
	}

	// Move the particle after respawning each time by the initial velocity and angle
	void resetParticle (Particle* particle) {
		// give a random velocity and lifetime to the particle
		float angle = (std::rand () % m_emitAngle) * 3.14f / 180.f + m_emitStart;
		float speed = (std::rand () % 50) + m_speed;
		particle->velocity = sf::Vector2f (std::cos (angle) * speed, std::sin (angle) * speed);
		particle->lifetime = sf::milliseconds ((std::rand () % (m_lifetime.asMilliseconds () / 2)) + m_lifetime.asMilliseconds () / 2);
		particle->history.resize (0);

		// reset the position of the corresponding vertex
		particle->vertices[0].position = m_emitter;
		particle->vertices[1].position = m_emitter + sf::Vector2f (-m_size, -m_size);
		particle->vertices[2].position = m_emitter + sf::Vector2f (m_size, -m_size);
		particle->vertices[3].position = m_emitter + sf::Vector2f (-m_size, m_size);
		particle->vertices[4].position = m_emitter + sf::Vector2f (m_size, m_size);
	}

	// initialize texture of a particle
	void initTex (Particle* p) {
		p->vertices.setPrimitiveType (sf::PrimitiveType::Quads);
		p->vertices.resize (5);
		p->vertices[1].texCoords = sf::Vector2f (0, 0);
		p->vertices[2].texCoords = sf::Vector2f (t_width, 0);
		p->vertices[3].texCoords = sf::Vector2f (t_width, t_height);
		p->vertices[4].texCoords = sf::Vector2f (0, t_height);
	}	

	// Apply the transform and draw the vertext array
	virtual void draw (sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= getTransform ();
		states.texture = NULL;
		
		for (std::size_t i = 0; i < m_particles.size (); ++i) {
			target.draw (m_particles[i]->vertices, &m_texture);
			target.draw (m_particles[i]->history, states);
		}
	}

public:
	ParticleSystem () :
		m_lifetime (sf::seconds (3)),
		m_emitter (0, 0),
		m_count (COUNT),
		m_gravity (0.f),
		m_emitAngle (360),
		m_emitStart (0.f),
		m_speed (50.f),
		m_size (0.f) {		
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
			m_particles[i]->lifetime -= elapsed;

			// if the particle is dead, respawn it
			if (m_particles[i]->lifetime <= sf::Time::Zero) {
				resetParticle (i);
				return;
			}

			// store the history positions and life time of a particle
			float prevVelocity = m_particles[i]->velocity.y;
			sf::Vertex *vertex = new sf::Vertex ();
			vertex->color = sf::Color::Color (std::rand () % 255, std::rand () % 255, std::rand () % 255, std::rand () % 255);
			vertex->position = m_particles[i]->vertices[0].position;
			m_particles[i]->history.append (*vertex);
			m_particles[i]->h_time.push_back (m_particles[i]->lifetime);

			// update the position of the corresponding vertex
			m_particles[i]->velocity.y += m_gravity * elapsed.asSeconds ();
			m_particles[i]->vertices[0].position.x += m_particles[i]->velocity.x * elapsed.asSeconds ();
			m_particles[i]->vertices[0].position.y += (m_particles[i]->velocity.y + prevVelocity) / 2 * elapsed.asSeconds ();
			
			// update surrending vertices of shape
			sf::Vector2f m_pos = m_particles[i]->vertices[0].position;
			m_particles[i]->vertices[1].position = m_pos + sf::Vector2f (-m_size, -m_size);
			m_particles[i]->vertices[2].position = m_pos + sf::Vector2f (m_size, -m_size);
			m_particles[i]->vertices[3].position = m_pos + sf::Vector2f (-m_size, m_size);
			m_particles[i]->vertices[4].position = m_pos + sf::Vector2f (m_size, m_size);
		}
	}

	// get the count of particles
	int getCount () {
		return m_count;
	}

	// get the emitter positin of particles
	sf::Vector2f getEmitter () {
		return m_emitter;
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
		while (diff > 0) {
			Particle* particle = new Particle ();

			if (particle != nullptr) {
				initTex (particle);
				resetParticle (particle);
				m_particles.push_back (ParticlePtr(particle));
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
		m_count = m_particles.size ();
	}

	// initialize texture map and build the sprite/quad vertices around the particle
	void init (int id) {		
		sf::Texture temp;
		assert (temp.loadFromFile ("static/image/smokeparticle.png"));
		tex_map[1] = temp;
		assert (temp.loadFromFile ("static/image/dollar.png"));
		tex_map[2] = temp;
		m_texture = tex_map[id];
		m_texture.setSmooth (true);
		t_width = m_texture.getSize ().x;
		t_height = m_texture.getSize ().y;

		for (int i = 0; i < m_count; ++i) {
			Particle* particle = new Particle ();
			m_particles.push_back (ParticlePtr (particle));
			initTex (particle);
		}
	}

	// apply force to particles
	void applyForce (float force, sf::Time elapsed) {
		std::cout << "Force: " << force * elapsed.asSeconds () << std::endl;
		for (size_t i = 0; i < m_particles.size (); i++) {
			m_particles[i]->velocity.x += force * elapsed.asSeconds ();
			m_particles[i]->velocity.y += force * elapsed.asSeconds ();
		}
	}
};