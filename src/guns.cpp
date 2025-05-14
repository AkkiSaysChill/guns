#include "guns.h"
#include <algorithm>
#include <raylib.h>

// BUG: sniper grenade_launcher rocket_launcher bullets not drawing
// TODO: make GUN_GRENADE_LAUNCHER and rocket_launcher bullets explode and
// damage enemy in radius of 15
//

GunManager::GunManager() {
  // Initialize the gun list with some default guns
  max_guns = 3;
  selected_gun_index = 0; // Initialize selected gun index

  gun pistol = {GUN_PISTOL, "Pistol", 10, 15, 50, 1.5f, 0.5f, 15};
  gun rifle = {GUN_RIFLE, "Rifle", 20, 30, 100, 2.0f, 0.1f, 30};
  gun shotgun = {GUN_SHOTGUN, "Shotgun", 50, 8, 20, 1.0f, 0.5f, 8};
  gun sniper = {GUN_SNIPER, "Sniper", 50, 5, 200, 3.0f, 0.2f, 5};
  gun submachinegun = {
      GUN_SUBMACHINEGUN, "Submachine Gun", 15, 30, 50, 0.5f, 0.1f, 30};
  gun assault_rifle = {
      GUN_ASSAULT_RIFLE, "Assault Rifle", 25, 30, 100, 2.0f, 0.1f, 30};
  gun lmg = {GUN_LMG, "Light Machine Gun", 20, 100, 200, 3.0f, 0.1f, 100};
  gun grenade_launcher = {
      GUN_GRENADE_LAUNCHER, "Grenade Launcher", 40, 5, 50, 2.0f, 0.5f, 5};
  gun rocket_launcher = {
      GUN_ROCKET_LAUNCHER, "Rocket Launcher", 100, 1, 500, 5.0f, 1.0f, 1};

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

void GunManager::DrawGun() {
  // Draw the currently selected gun
  if (guns.empty())
    return;

  gun &current = guns[selected_gun_index];
  DrawText(current.name, 10, GetScreenHeight() - 30, 20, DARKGRAY);
}

void GunManager::ShootGun(Vector2 position, Vector2 direction) {
  // Check if there are too many bullets on screen already
  if (bullets.size() >= guns[selected_gun_index].ammo_capacity *
                            2) { // Increased capacity for enemy bullets
    return;
  }

  Bullet bullet;
  bullet.Position = position;
  bullet.speed = {direction.x * 10, direction.y * 10};
  bullet.damage = guns[selected_gun_index].damage;
  bullet.active = true;
  bullet.isPlayerBullet = false; // Mark as enemy bullet
  bullets.push_back(bullet);
}

void GunManager::ShootFromPlayer(Vector2 position, Vector2 direction) {
  if (bullets.size() >= guns[selected_gun_index].ammo_capacity *
                            2) { // Increased capacity for consistency
    return;
  }

  Bullet bullet;
  bullet.Position = position;
  bullet.speed = {direction.x * 10, direction.y * 10};
  bullet.damage = guns[selected_gun_index].damage;
  bullet.active = true;
  bullet.isPlayerBullet = true; // Mark as player bullet
  bullets.push_back(bullet);
}

// TODO: add collision detection for bullets
// the logic should be if the bullet is a player bullet and it hits an enemy,
// enemy gets damage if the bullets are of enemy and it hits the player, player
// gets damage and bullet is deactivated

void GunManager::UpdateBullets() {
  for (auto &bullet : bullets) {
    if (!bullet.active)
      continue;

    bullet.Position.x += bullet.speed.x;
    bullet.Position.y += bullet.speed.y;

    // Deactivate bullets if they go off-screen
    if (bullet.Position.x < 0 || bullet.Position.x > GetScreenWidth() ||
        bullet.Position.y < 0 || bullet.Position.y > GetScreenHeight()) {
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

    // Draw player bullets in blue, enemy bullets in red
    Color bulletColor = bullet.isPlayerBullet ? BLUE : RED;
    DrawCircleV(bullet.Position, 5, bulletColor);
  }
}
