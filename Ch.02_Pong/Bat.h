#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Bat
{
  private:
	Vector2f m_Position;

	// A rectangle shape object
	RectangleShape m_Shape;

	float m_Speed = 1000.0f;
	bool m_IsMovingRight = false;
	bool m_IsMovingLeft = false;

  public:
	Bat(float startX, float startY);

	FloatRect getCoords(); // Positions of the rect

	RectangleShape getShape(); // Dimensions

	// Transforms

	void moveLeft();

	void moveRight();

	void stopLeft();

	void stopRight();

	void update(Time delta);
};
