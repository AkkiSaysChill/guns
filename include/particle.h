#pragma once

#include <raylib.h>
#include <vector>

enum ParticleType { EXPLOSION, SMOKE, FIRE };

struct particle_t {
  Vector2 position;
  Vector2 velocity;
  float size;
  float lifetime; // Seconds remaining
  Color color;
  ParticleType type;
};

class Particle {
public:
  std::vector<particle_t> particle;
  void spawnParticle(float x, float y, float size, float speed,
                     ParticleType type);
  void deleteParticle(int index);
  void UpdateParticles(float deltaTime);
  void DrawParticles();
};
