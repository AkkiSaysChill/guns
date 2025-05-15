#include "guns.h"
#include <algorithm>
#include <cmath>
#include <raylib.h>
#include <raymath.h>
// #include <string>

GunManager::GunManager() {
  max_guns = 3;
  selected_gun_index = 0;

  // Load individual textures
  Texture2D pistolTex = LoadTexture("assets/Guns/Pistol.png");
  Texture2D rifleTex = LoadTexture("assets/Guns/rifle.png");
  Texture2D shotgunTex = LoadTexture("assets/Guns/shotgun.png");
  Texture2D sniperTex = LoadTexture("assets/Guns/sniper.png");
  Texture2D smgTex = LoadTexture("assets/Guns/SMG.png");
  Texture2D assaultRifleTex = LoadTexture("assets/Guns/AR.png");
  Texture2D lmgTex = LoadTexture("assets/Guns/lmg.png");
  Texture2D grenadeLauncherTex =
      LoadTexture("assets/Guns/grenade_launcher.png");
  Texture2D rocketLauncherTex = LoadTexture("assets/Guns/RocketLauncher.png");

  // Add guns with separate textures
  gun pistol = {GUN_PISTOL, pistolTex, "Pistol", 10, 15, 50, 1.5f, 0.5f, 15};
  gun rifle = {GUN_RIFLE, rifleTex, "Rifle", 20, 30, 100, 2.0f, 0.1f, 30};
  gun shotgun = {GUN_SHOTGUN, shotgunTex, "Shotgun", 50, 8, 20, 1.0f, 0.5f, 8};
  gun sniper = {GUN_SNIPER, sniperTex, "Sniper", 50, 5, 200, 3.0f, 0.2f, 5};
  gun submachinegun = {
      GUN_SUBMACHINEGUN, smgTex, "Submachine Gun", 15, 30, 50, 0.5f, 0.1f, 30};
  gun assault_rifle = {GUN_ASSAULT_RIFLE,
                       assaultRifleTex,
                       "Assault Rifle",
                       25,
                       30,
                       100,
                       2.0f,
                       0.1f,
                       30};
  gun lmg = {GUN_LMG, lmgTex, "Light Machine Gun", 20, 100, 200, 3.0f,
             0.1f,    100};
  gun grenade_launcher = {GUN_GRENADE_LAUNCHER,
                          grenadeLauncherTex,
                          "Grenade Launcher",
                          40,
                          5,
                          50,
                          2.0f,
                          0.5f,
                          5};
  gun rocket_launcher = {GUN_ROCKET_LAUNCHER,
                         rocketLauncherTex,
                         "Rocket Launcher",
                         100,
                         1,
                         500,
                         5.0f,
                         1.0f,
                         1};

  // Add all to the list
  guns.push_back(pistol);
  guns.push_back(rifle);
  guns.push_back(shotgun);
  guns.push_back(sniper);
  guns.push_back(submachinegun);
  guns.push_back(assault_rifle);
  guns.push_back(lmg);
  guns.push_back(grenade_launcher);
  guns.push_back(rocket_launcher);
}

GunManager::~GunManager() {
  // Destructor
  guns.clear();
}

void GunManager::DrawGun(Vector2 playerPos, std::vector<Vector2> enemyPositions,
                         std::vector<int> enemy_gun_ids) {
  if (guns.empty())
    return;

  gun &current = guns[selected_gun_index];

  Vector2 gunOffset = {20, 10}; // tweak based on character sprite size
  Vector2 gunPos = {playerPos.x + gunOffset.x, playerPos.y + gunOffset.y};

  Vector2 aimDir = Vector2Subtract(GetMousePosition(), playerPos);
  float angle = atan2f(aimDir.y, aimDir.x) * RAD2DEG;

  DrawTextureEx(current.texture, gunPos, angle, 1.0f, WHITE);

  // DrawText(std::to_string(selected_gun_index).c_str(), 100, 100, 32, RED);

  // draw gun for enemies

  for (size_t i = 0; i < enemyPositions.size(); i++) {
    Vector2 enemyGunPos = {enemyPositions[i].x + gunOffset.x,
                           enemyPositions[i].y + gunOffset.y};

    // Calculate direction from enemy to player
    Vector2 dirToPlayer = Vector2Subtract(playerPos, enemyPositions[i]);
    float enemyAngle = atan2f(dirToPlayer.y, dirToPlayer.x) * RAD2DEG;

    int gunId = enemy_gun_ids[i];
    if (gunId >= 0 && gunId < guns.size()) {
      DrawTextureEx(guns[gunId].texture, enemyGunPos, enemyAngle, 1.0f, WHITE);
    }
  }
  DrawText(current.name, 10, GetScreenHeight() - 30, 20, DARKGRAY);
}

void GunManager::ShootGun(Vector2 position, Vector2 direction) {
  if (bullets.size() >= guns[selected_gun_index].ammo_capacity * 2) {
    return;
  }

  Bullet bullet;
  bullet.Position = position;

  float speedMultiplier = 10;

  bullet.speed = {direction.x * speedMultiplier, direction.y * speedMultiplier};
  bullet.damage = guns[selected_gun_index].damage;
  bullet.active = true;
  bullet.isPlayerBullet = false;
  bullet.gun_id = selected_gun_index;

  bullets.push_back(bullet);
}

void GunManager::ShootFromPlayer(Vector2 position, Vector2 direction) {
  if (bullets.size() >= guns[selected_gun_index].ammo_capacity * 2) {
    return;
  }

  Vector2 gunOffset = {20, 10};
  Vector2 gunPos = Vector2Add(position, gunOffset);

  Bullet bullet;
  bullet.Position = gunPos;

  float speedMultiplier = 10;

  bullet.speed = {direction.x * speedMultiplier, direction.y * speedMultiplier};
  bullet.damage = guns[selected_gun_index].damage;
  bullet.active = true;
  bullet.isPlayerBullet = true;
  bullet.gun_id = selected_gun_index;

  bullets.push_back(bullet);
}

void GunManager::UpdateBullets() {
  float screenWidth = (float)GetScreenWidth();
  float screenHeight = (float)GetScreenHeight();

  for (auto &bullet : bullets) {
    if (!bullet.active)
      continue;

    bullet.Position = Vector2Add(bullet.Position, bullet.speed);

    // Deactivate if out of screen bounds (plus radius buffer)
    if (bullet.Position.x < -bullet.radius ||
        bullet.Position.x > screenWidth + bullet.radius ||
        bullet.Position.y < -bullet.radius ||
        bullet.Position.y > screenHeight + bullet.radius) {
      bullet.active = false;
    }
  }

  // Remove inactive bullets
  bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                               [](const Bullet &b) { return !b.active; }),
                bullets.end());
}

void GunManager::DrawBullets() {
  for (const auto &bullet : bullets) {
    if (!bullet.active)
      continue;

    Color bulletColor = bullet.isPlayerBullet ? BLUE : RED;

    DrawCircleV(bullet.Position, 5, bulletColor);
  }
}
