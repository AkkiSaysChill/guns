#include "particle.h"
#include <cmath>
#include <raylib.h>

void Particle::spawnParticle(float x, float y, float size, float speed,
                             ParticleType type = EXPLOSION) {
  particle_t newParticle;
  newParticle.position = {x, y};

  // Random velocity direction
  float angle = GetRandomValue(0, 360) * DEG2RAD;
  newParticle.velocity = {cosf(angle) * speed, sinf(angle) * speed};

  newParticle.size = size;
  newParticle.lifetime = 5.0f; // Particle lives for 2 seconds
  if (type == EXPLOSION) {
    newParticle.color = ORANGE;
  } else if (type == SMOKE) {
    newParticle.color = GRAY;
  } else if (type == FIRE) {
    newParticle.color = RED;
  } else {
    newParticle.color = WHITE; // Default to white
  }
  newParticle.type = type;

  particle.push_back(newParticle);
}

void Particle::UpdateParticles(float deltaTime) {
  for (size_t i = 0; i < particle.size();) {
    particle[i].position.x += particle[i].velocity.x * deltaTime;
    particle[i].position.y += particle[i].velocity.y * deltaTime;

    // Optional gravity effect
    particle[i].velocity.y += 50.0f * deltaTime; // Gravity (pixels/sec²)

    particle[i].size *= 0.98f; // Shrink
    particle[i].color.a -= 2;  // Fade

    particle[i].lifetime -= deltaTime;

    // Remove dead particle
    if (particle[i].lifetime <= 0 || particle[i].size < 0.5f ||
        particle[i].color.a <= 1) {
      particle.erase(particle.begin() + i);
    } else {
      ++i;
    }
  }

  DrawParticles();
}

void Particle::DrawParticles() {
  for (const auto &p : particle) {
    DrawCircleV(p.position, p.size, p.color);
  }
}

void Particle::deleteParticle(int index) {
  if (index >= 0 && index < particle.size()) {
    particle.erase(particle.begin() + index);
  }
}
