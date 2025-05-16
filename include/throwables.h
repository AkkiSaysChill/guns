#pragma once
#include "particle.h"
#include "raylib.h"
#include <vector>

struct throwables {
  int id;
  int type;
  float x;
  float y;
  float vx;
  float vy;
  int damage;
  int lifetime;
  Texture2D texture;
};

struct explosion_t {
  float x, y;
  float scale = 1.0f;
  int lifetime = 30;
  int age = 0;
  bool didDamage = false;
  float damageRadius = 140.0f;
  int damage = 100;
};

enum throwables_type {
  THROWABLE_TYPE_GRENADE,
  THROWABLE_TYPE_MOLLY,
};

class throwables_manager {
public:
  Texture2D explosion;
  std::vector<explosion_t> Explosions;

  std::vector<Rectangle> explosion_rect;

  throwables_manager();
  ~throwables_manager();

  Texture2D grenadeTexture;
  Texture2D molotovTexture;

  std::vector<throwables> Activethrowables;

  void add_throwable(int id, int type, Vector2 start, Vector2 target,
                     int damage, int lifetime);
  void remove_throwable(int id);
  void update_throwables(Particle *pm);
  void draw_throwables();

  void draw_explosions();

private:
  particle_t *particle;
};
