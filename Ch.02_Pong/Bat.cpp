#include "Bat.h"
#include <SFML/Graphics/Rect.hpp>

#define _Bat_WIDTH 50
#define _Bat_HEIGHT 5


// This is the constructor and it is called when we create an object
Bat::Bat(float startX, float startY)
{
	m_Position.x = startX;
	m_Position.y = startY;

	m_Shape.setSize(sf::Vector2f(_Bat_WIDTH, _Bat_HEIGHT));
	m_Shape.setPosition(m_Position);
}


FloatRect Bat::getCoords()
{
	return m_Shape.getGlobalBounds();
}


RectangleShape Bat::getShape()
{
	return m_Shape;
}


void Bat::moveLeft()
{
	m_IsMovingLeft = true;
}


void Bat::moveRight()
{
	m_IsMovingRight = true;
}


void Bat::stopLeft()
{
	m_IsMovingLeft = false;
}


void Bat::stopRight()
{
	m_IsMovingRight = false;
}


void Bat::update(Time delta)
{
	if (m_IsMovingLeft)
		m_Position.x -= m_Speed * delta.asSeconds();

	if (m_IsMovingRight)
		m_Position.x += m_Speed * delta.asSeconds();

	m_Shape.setPosition(m_Position);
}
