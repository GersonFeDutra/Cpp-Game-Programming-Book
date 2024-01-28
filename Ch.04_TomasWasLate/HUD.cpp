#include "HUD.h"

#define ROBOTO_FONT "fonts/Roboto-Light.ttf"
#define START_FONT_SIZE 100

#define TIME_FONT_SIZE 75
#define TIME_FONT_OFFSET 150

#define LEVEL_FONT_SIZE 75
#define LEVEL_TEXT_OFFSET 25

Hud::Hud() {
	Vector2u resolution;
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;

	// Load the font
	m_Font.loadFromFile(ROBOTO_FONT);
	// When paused
	m_StartText.setFont(m_Font);
	m_StartText.setCharacterSize(START_FONT_SIZE);
	m_StartText.setFillColor(Color::White);
	m_StartText.setString("Press Enter when ready!");

	// Position the text
	FloatRect textRect = m_StartText.getLocalBounds();

	m_StartText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

	m_StartText.setPosition(resolution.x / 2.0f, resolution.y / 2.0f);

	// Time
	m_TimeText.setFont(m_Font);
	m_TimeText.setCharacterSize(TIME_FONT_SIZE);
	m_TimeText.setFillColor(Color::White);
	m_TimeText.setPosition(resolution.x - TIME_FONT_OFFSET, 0);
	m_TimeText.setString("------");

	// Level
	m_LevelText.setFont(m_Font);
	m_LevelText.setCharacterSize(LEVEL_FONT_SIZE);
	m_LevelText.setFillColor(Color::White);
	m_LevelText.setPosition(LEVEL_TEXT_OFFSET, 0);
	m_LevelText.setString("1");
}

Text Hud::getMessage() {
	return m_StartText;
}

Text Hud::getLevel() {
	return m_LevelText;
}

Text Hud::getTime() {
	return m_TimeText;
}

void Hud::setLevel(String text) {
	m_LevelText.setString(text);
}

void Hud::setTime(String text) {
	m_TimeText.setString(text);
}
