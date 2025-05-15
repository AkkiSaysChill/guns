#pragma once
#include "particle.h"
#include "raylib.h"
#include <vector>

struct throwables {
  int id;
  int type;
  float x;
  float y;
  float vx; // velocity x
  float vy; // velocity y
  int damage;
  int lifetime;
  Texture2D texture;
};

enum throwables_type {
  THROWABLE_TYPE_GRENADE,
  THROWABLE_TYPE_MOLLY,
};

class throwables_manager {
public:
  throwables_manager();
  ~throwables_manager();
  std::vector<throwables> Activethrowables;

  void add_throwable(int id, int type, Vector2 start, Vector2 target,
                     int damage, int lifetime);
  void remove_throwable(int id);
  void update_throwables();
  void draw_throwables();

private:
  particle_t *particle;
};
