#include "Bob.h"
#include "TextureHolder.h"

#define DEFAULT_JUMP .25
#define BOB_PNG "graphics/bob.png"

Bob::Bob() {
	// Associate a texture with the sprite
	m_Sprite = Sprite(TextureHolder::GetTexture(BOB_PNG));

	m_JumpDuration = DEFAULT_JUMP;
}

// A virtual function implementation
bool Bob::handleInput() {
	m_JustJumped = false;

	if (Keyboard::isKeyPressed(Keyboard::Up)) {
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

	m_LeftPressed = Keyboard::isKeyPressed(Keyboard::Left);
	m_RightPressed = Keyboard::isKeyPressed(Keyboard::Right);

	return m_JustJumped;
}
