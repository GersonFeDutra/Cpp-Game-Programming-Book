#pragma once
#include "Particle.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Drawable.hpp>

using namespace sf;
using namespace std;

class ParticleSystem : public Drawable {
private:
	vector<Particle> m_Particles;
	VertexArray m_Vertices;
	float m_Duration;
	bool m_IsRunning = false;

public:
	virtual void draw(RenderTarget &target, RenderStates states) const;

	void init(int count);

	void emitParticles(Vector2f position);

	void update(float elapsed);

	bool running();
};
