#pragma once

#include "guns.h"
#include "particle.h"
#include "throwables.h"
#include <raylib.h>
#include <vector>

struct character {
  int health;
  int armor;
  int ammo;
  float shootCooldown;
  int score;
  Vector2 position;
  Vector2 velocity;
  gun_id current_gun_id;
};

class CharacterManager {
public:
  CharacterManager();
  ~CharacterManager();
  bool isReloading = false;
  float currentReloadTimer;

  Particle particle;
  throwables_manager thm;

  int next_id = 0;

  character player;
  character enemy;
  std::vector<character> enemies;
  int enemy_count = 10;
  bool enemy_spawned = false;

  void CheckBulletEnemyCollision(GunManager *gun_manager);
  void CheckExplosionDamage(throwables_manager *thm);

  void UpdatePlayer(GunManager *gun_manager);
  void UpdateEnemy(GunManager *gun_manager);
  void DrawPlayer();
  void DrawEnemy();
  void spawnEnemies();

  Texture2D mouseCursor;
};
