#include "ParticleSystem.h"
#include "math.h"
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

#define PI 3.14159265
#define SPEED_BASE 600.f
#define MIN_SPEED 600

void ParticleSystem::init(int numParticles) {
	m_Vertices.setPrimitiveType(Points);
	m_Vertices.resize(numParticles);

	// Create the particles
	for (int i = 0; i < numParticles; i++) {
		srand(time(0) + i);
		float angle = (rand() % 360) * PI / 180.f;
		float speed = (rand() % MIN_SPEED) + SPEED_BASE;

		Vector2f direction = Vector2f(cos(angle) * speed, sin(angle) * speed);
		m_Particles.push_back(Particle(direction));
	}
}

void ParticleSystem::update(float delta) {
	m_Duration -= delta;
	int currentVertex = 0;

	for (auto it : m_Particles) {
		// Move the particle
		it.update(delta);

		// Update the vertex array
		m_Vertices[currentVertex++].position = it.getPosition();
	}

	if (m_Duration < 0)
		m_IsRunning = false;
}

void ParticleSystem::emitParticles(Vector2f startPosition) {
	m_IsRunning = true;
	m_Duration = 2;

	int currentVertex = 0;

	for (auto it : m_Particles) {
		m_Vertices[currentVertex].position = startPosition;
		m_Vertices[currentVertex++].color = Color::Yellow;
		it.setPosition(startPosition);
	}
}

void ParticleSystem::draw(RenderTarget &target, RenderStates states) const {
	target.draw(m_Vertices, states);
}

bool ParticleSystem::running() {
	return m_IsRunning;
}
