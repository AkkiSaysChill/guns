// guns.h
#pragma once
#include <raylib.h>
#include <vector>

enum gun_id {
  GUN_PISTOL,
  GUN_RIFLE,
  GUN_SHOTGUN,
  GUN_SNIPER,
  GUN_SUBMACHINEGUN,
  GUN_ASSAULT_RIFLE,
  GUN_LMG,
  GUN_GRENADE_LAUNCHER,
  GUN_ROCKET_LAUNCHER,
};

struct gun {
  gun_id id;
  Texture2D texture; // Texture for the gun
  char name[32];
  int damage;
  int ammo_capacity;
  int range;
  float reload_time;
  float fire_rate;
  int ammo;
};

struct Bullet {
  Vector2 Position; // Position of the bullet
  Vector2 speed;    // Speed of the bullet
  int damage;       // Damage dealt by the bullet
  bool active;      // Is the bullet currently active?
  bool isPlayerBullet;
  Texture2D bullet_texture; // Texture for the bullet
  int gun_id;
  float radius; // Radius of the bullet
};

class GunManager {
public:
  GunManager();
  ~GunManager();

  std::vector<gun> guns;
  std::vector<Bullet> bullets;
  int max_guns;
  int selected_gun_index = 0;

  void DrawGun(Vector2 playerPos, std::vector<Vector2> enemyPositions,
               std::vector<int> enemy_gun_ids);
  void ShootGun(Vector2 position, Vector2 direction);
  void ShootFromPlayer(Vector2 position, Vector2 direction);
  void DrawBullets();
  void UpdateBullets();
};
