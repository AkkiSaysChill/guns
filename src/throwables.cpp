#include "throwables.h"
#include <algorithm>
#include <raylib.h>
#include <raymath.h>

throwables_manager::throwables_manager() {
  explosion = LoadTexture("assets/Bullets&Ammo/explosion.png");
  grenadeTexture = LoadTexture("assets/Guns/nade.png");
  molotovTexture = LoadTexture("assets/throwables/molotov.png");
}

throwables_manager::~throwables_manager() {
  UnloadTexture(explosion);
  UnloadTexture(Activethrowables[0].texture);
  UnloadTexture(grenadeTexture);
  UnloadTexture(molotovTexture);
}

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
    t.texture = grenadeTexture;
  } else if (type == THROWABLE_TYPE_MOLLY) {
    t.texture = molotovTexture;
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

void throwables_manager::update_throwables(Particle *pm) {
  for (auto &t : Activethrowables) {
    t.x += t.vx;
    t.y += t.vy;
    t.lifetime--;

    float resistance = 0.04f;

    if (t.vx > 0)
      t.vx = std::max(t.vx - resistance, 0.0f);
    else if (t.vx < 0)
      t.vx = std::min(t.vx + resistance, 0.0f);

    if (t.vy > 0)
      t.vy = std::max(t.vy - resistance, 0.0f);
    else if (t.vy < 0)
      t.vy = std::min(t.vy + resistance, 0.0f);
    if (t.lifetime == 0) {

      for (int i = 0; i < 4; ++i) {
        pm->spawnParticle(t.x, t.y, 20, 100, EXPLOSION);
        pm->spawnParticle(t.x, t.y, 15, 100, SMOKE);
      }

      Explosions.push_back({t.x, t.y});
    }
  }

  // Remove expired throwables
  Activethrowables.erase(
      std::remove_if(Activethrowables.begin(), Activethrowables.end(),
                     [](const throwables &t) { return t.lifetime <= 0; }),
      Activethrowables.end());
}

void throwables_manager::draw_explosions() {

  for (auto &e : Explosions) {

    // --- Scale Growth ---
    float maxScale = 3.0f;
    float minScale = 1.0f;
    float growthRate = (maxScale - minScale) / e.lifetime;
    float smoothScale = minScale + growthRate * e.age;

    // --- Add Jitter ---
    float jitter = GetRandomValue(-10, 10) / 100.0f; // -0.10 to +0.10
    e.scale = smoothScale + jitter;

    // --- Pixelated center alignment ---
    float scaledWidth = explosion.width * e.scale;
    float scaledHeight = explosion.height * e.scale;

    // Snap position to integer pixels
    float drawX = floor(e.x - scaledWidth / 2.0f);
    float drawY = floor(e.y - scaledHeight / 2.0f);

    explosion_rect.push_back({drawX, drawY, e.damageRadius, e.damageRadius});

    // for (const auto &rect : explosion_rect) {
    //   DrawRectangleLines((int)rect.x, (int)rect.y, (int)rect.width,
    //                      (int)rect.height, RED);
    // }

    DrawTextureEx(explosion, {drawX, drawY}, 0.0f, e.scale, WHITE);

    e.age++;
  }

  // --- Remove finished explosions ---
  Explosions.erase(
      std::remove_if(Explosions.begin(), Explosions.end(),
                     [](const explosion_t &e) { return e.age >= e.lifetime; }),
      Explosions.end());
  explosion_rect.erase(explosion_rect.begin(),
                       explosion_rect.end()); // Clear the vector
}

void throwables_manager::draw_throwables() {
  for (const auto &t : Activethrowables) {

    DrawTexture(t.texture, t.x, t.y, WHITE);
  }
}
