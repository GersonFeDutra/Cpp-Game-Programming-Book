#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Ball
{
  private:
	Vector2f m_Position;
	Vector2f m_LastStep; // Keeps track of the last step to reflect collisions

	RectangleShape m_Shape;

	const float m_DEFAULT_SPEED = 300.0f;
	float m_Speed = m_DEFAULT_SPEED;

	Vector2f m_Direction = Vector2f(.2f, .2f);

  public:
	Ball(float startX, float startY);

	FloatRect getCoords();

	RectangleShape getShape();

	float getXVelocity();

	void reboundSides();

	void reboundBatOrTop();

	void reboundBottom();

	void resetSpeed();

	void update(Time delta);
};
