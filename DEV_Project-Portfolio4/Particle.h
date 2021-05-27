#pragma once
#include "Definitions.h"

class Particle
{
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 prev_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT4 color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	float lifespan;

	Particle() = default;
	Particle(XMFLOAT3 pos, XMFLOAT3 prev_pos, XMFLOAT4 vel, XMFLOAT4 col, float life);
	~Particle() = default;
	Particle& operator=(const Particle&) = default;
};

