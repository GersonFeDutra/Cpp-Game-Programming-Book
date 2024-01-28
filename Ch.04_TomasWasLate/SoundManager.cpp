#include "SoundManager.h"
#include <SFML/Audio.hpp>

using namespace sf;

#define _SOUND "sound/"
#define FIRE_WAV(N) _SOUND "fire" #N ".wav"
#define FALL_IN_FIRE_WAV _SOUND "fallinfire.wav"
#define FALL_IN_WATER_WAV _SOUND "fallinwater.wav"
#define JUMP_WAV _SOUND "jump.wav"
#define REACH_GOAL_WAV _SOUND "reachgoal.wav"

#define MIN_DISTANCE 150
#define DEFAULT_ATTENUATION 15

SoundManager::SoundManager() {
	// Load the sound in to the buffers
	m_FireBuffer.loadFromFile(FIRE_WAV(1));
	m_FallInFireBuffer.loadFromFile(FALL_IN_FIRE_WAV);
	m_FallInWaterBuffer.loadFromFile(FALL_IN_WATER_WAV);
	m_JumpBuffer.loadFromFile(JUMP_WAV);
	m_ReachGoalBuffer.loadFromFile(REACH_GOAL_WAV);

	// Associate the sounds with the buffers
	for (unsigned short i = 0; i < FIRE_SOUNDS; i++)
		m_FireSounds[i].setBuffer(m_FireBuffer);

	m_FallInFireSound.setBuffer(m_FallInFireBuffer);
	m_FallInWaterSound.setBuffer(m_FallInWaterBuffer);
	m_JumpSound.setBuffer(m_JumpBuffer);
	m_ReachGoalSound.setBuffer(m_ReachGoalBuffer);

	// When the player is next to emmiter, sound is full volume
	float minDistance = MIN_DISTANCE;
	// The sound reduces steadily as the player moves further away
	float attenuation = DEFAULT_ATTENUATION;

	for (unsigned short i = 0; i < FIRE_SOUNDS; i++) {
		// Set all the attenuation levels
		m_FireSounds[i].setAttenuation(attenuation);
		// Set all the minimum distance levels
		m_FireSounds[i].setMinDistance(minDistance);
		// Loop all the fire sounds when they are played
		m_FireSounds[i].setLoop(true);
	}
}

void SoundManager::playFire(Vector2f emitterLocation, Vector2f listenerLocation) {
	// Where is the listener? Thomas
	Listener::setPosition(listenerLocation.x, listenerLocation.y, 0.0f);

	// Locate / move the source of the sound
	m_FireSounds[m_NextSound].setPosition(emitterLocation.x, emitterLocation.y, 0.0f);

	if (m_FireSounds[m_NextSound].getStatus() == Sound::Status::Stopped)
		m_FireSounds[m_NextSound].play();

	// Increment to the next fire sound
	// Go back to 1° when the third sound has been started
	m_NextSound = (++m_NextSound) % FIRE_SOUNDS;
}

void SoundManager::playFallInFire() {
	m_FallInFireSound.setRelativeToListener(true);
	m_FallInFireSound.play();
}

void SoundManager::playFallInWater() {
	m_FallInWaterSound.setRelativeToListener(true);
	m_FallInWaterSound.play();
}

void SoundManager::playJump() {
	m_JumpSound.setRelativeToListener(true);
	m_JumpSound.play();
}

void SoundManager::playReachGoal() {
	m_ReachGoalSound.setRelativeToListener(true);
	m_ReachGoalSound.play();
}
