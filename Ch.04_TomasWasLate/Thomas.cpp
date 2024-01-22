#include "Thomas.h"
#include "TextureHolder.h"

#define DEFAULT_JUMP .45
#define THOMAS_PNG "graphics/thomas.png"

Thomas::Thomas() {
	// Associate a texture with the sprite
	m_Sprite = Sprite(TextureHolder::GetTexture(THOMAS_PNG));

	m_JumpDuration = DEFAULT_JUMP;
}

// A virtual function implementation
bool Thomas::handleInput() {
	m_JustJumped = false;

	if (Keyboard::isKeyPressed(Keyboard::W)) {
		// Start a jump if not already jumping
		// but only if standing on a block (not falling)
		if (!m_IsJumping && !m_IsFalling) {
			m_JustJumped = m_IsJumping = true;
			m_TimeThisJump = 0;
		}
	} else {
		m_IsJumping = false;
		m_IsFalling = true;
	}

	m_LeftPressed = Keyboard::isKeyPressed(Keyboard::A);
	m_RightPressed = Keyboard::isKeyPressed(Keyboard::D);

	return m_JustJumped;
}
