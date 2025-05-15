#include "throwables.h"
#include <algorithm>
#include <raylib.h>
#include <raymath.h>

// TODO: add explosion effect for grenades
// TODO: add molotovs and their effects also
throwables_manager::throwables_manager() {}

throwables_manager::~throwables_manager() {}

void throwables_manager::add_throwable(int id, int type, Vector2 start,
                                       Vector2 target, int damage,
                                       int lifetime) {
  throwables t;
  t.id = id;
  t.type = type;
  t.x = start.x;
  t.y = start.y;
  t.damage = damage;
  t.lifetime = lifetime;

  if (type == THROWABLE_TYPE_GRENADE) {
    t.texture = LoadTexture("assets/Guns/nade.png");
  } else if (type == THROWABLE_TYPE_MOLLY) {
    t.texture = LoadTexture("assets/throwables/molotov.png");
  }

  Vector2 dir = Vector2Subtract(target, start);
  dir = Vector2Normalize(dir);
  dir = Vector2Scale(dir, 5.0f); // speed of the throwable

  t.vx = dir.x;
  t.vy = dir.y;

  Activethrowables.push_back(t);
}

void throwables_manager::remove_throwable(int id) {

  Activethrowables.erase(
      std::remove_if(Activethrowables.begin(), Activethrowables.end(),
                     [id](const throwables &t) { return t.id == id; }),
      Activethrowables.end());
}

void throwables_manager::update_throwables() {
  for (auto &t : Activethrowables) {
    t.x += t.vx;
    t.y += t.vy;
    t.lifetime--;
  }

  // Remove expired throwables
  Activethrowables.erase(
      std::remove_if(Activethrowables.begin(), Activethrowables.end(),
                     [](const throwables &t) { return t.lifetime <= 0; }),
      Activethrowables.end());
}

void throwables_manager::draw_throwables() {
  for (const auto &t : Activethrowables) {

    DrawTexture(t.texture, t.x, t.y, WHITE);
  }
}
