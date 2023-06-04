#include "Ball.h"

#define _Ball_WIDTH 10
#define _Ball_HEIGHT 10
#define _Ball_REBOUND_X 500


// The constructor function
Ball::Ball(float startX, float startY)
{
	m_Position.x = startX;
	m_Position.y = startY;

	m_Shape.setSize(sf::Vector2f(_Ball_WIDTH, _Ball_HEIGHT));
	m_Shape.setPosition(m_Position);
}


FloatRect Ball::getCoords()
{
	return m_Shape.getGlobalBounds();
}


RectangleShape Ball::getShape()
{
	return m_Shape;
}


float Ball::getXVelocity()
{
	return m_Direction.x;
}


void Ball::reboundSides()
{
	m_Direction.x = -m_Direction.x;
	m_Position.x -= m_LastStep.x;
}


void Ball::reboundBatOrTop()
{
	m_Direction.y = -m_Direction.y;

	if (m_Direction.y > 0)
		m_Position.y = .0f; // Force the ball off the top of the screen
	else
		m_Position.y -= m_LastStep.y; // Force the ball off the bat

	// Speed up a little bit on each hit
	m_Speed *= 1.2f;
}


void Ball::reboundBottom()
{
	m_Position.y = 0;
	m_Position.x = _Ball_REBOUND_X;
	m_Direction.y = -m_Direction.y;
}

void Ball::resetSpeed()
{
	m_Speed = m_DEFAULT_SPEED;
}


void Ball::update(Time delta)
{
	// Update the ball's position
	m_LastStep.x = m_Direction.x * m_Speed * delta.asSeconds();
	m_LastStep.y = m_Direction.y * m_Speed * delta.asSeconds();
	m_Position.x += m_LastStep.x;
	m_Position.y += m_LastStep.y;

	// Move the ball
	m_Shape.setPosition(m_Position);
}
