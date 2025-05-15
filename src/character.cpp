#include "characters.h"
#include "guns.h"
#include "particle.h"
#include "throwables.h"
#include <algorithm>
#include <raylib.h>
#include <raymath.h>

CharacterManager::CharacterManager() {
  // Initialize player
  player.health = 1000;
  player.armor = 150;
  player.score = 0;
  player.position = {500, 500};
  player.velocity = {0, 0};
  enemy_spawned = false; // Initialize enemy_spawned flag
  enemy_count = 5;       // Set default enemy count
}

CharacterManager::~CharacterManager() {
  // Destructor
}

void CharacterManager::UpdatePlayer(GunManager *gun_manager) {
  // Get mouse position and compute normalized direction vector
  Vector2 mouse_pos = GetMousePosition();
  Vector2 direction =
      Vector2Normalize(Vector2Subtract(mouse_pos, player.position));

  // Apply directional input (WASD)
  if (IsKeyDown(KEY_W))
    player.velocity.y -= 5;
  if (IsKeyDown(KEY_S))
    player.velocity.y += 5;
  if (IsKeyDown(KEY_A))
    player.velocity.x -= 5;
  if (IsKeyDown(KEY_D))
    player.velocity.x += 5;

  // Update player position based on velocity
  player.position.x += player.velocity.x;
  player.position.y += player.velocity.y;

  // Keep player within screen bounds
  if (player.position.x < 0)
    player.position.x = 0;
  if (player.position.x > GetScreenWidth() - 50)
    player.position.x = GetScreenWidth() - 50;
  if (player.position.y < 0)
    player.position.y = 0;
  if (player.position.y > GetScreenHeight() - 50)
    player.position.y = GetScreenHeight() - 50;

  // thow throwables
  // this shit is hard ngl
  if (IsKeyPressed(KEY_T)) {
    thm.add_throwable(next_id++, THROWABLE_TYPE_GRENADE, player.position,
                      GetMousePosition(), 50, 120);
  }

  // Shoot bullet if space is pressed and cooldown has passed
  static float shootCooldown = 0.0f;
  if (shootCooldown > 0.0f)
    shootCooldown -= GetFrameTime();

  particle.UpdateParticles(GetFrameTime());
  thm.update_throwables();
  thm.draw_throwables();

  if (IsKeyDown(KEY_SPACE) && shootCooldown <= 0.0f &&
      gun_manager->guns[gun_manager->selected_gun_index].ammo > 0 &&
      !isReloading) {

    gun_manager->ShootFromPlayer(player.position, direction);
    shootCooldown =
        gun_manager->guns[gun_manager->selected_gun_index].fire_rate;
    gun_manager->guns[gun_manager->selected_gun_index].ammo--;

    particle.spawnParticle(player.position.x, player.position.y,
                           GetRandomValue(10, 15), 100, SMOKE);
  }

  if (gun_manager->guns[gun_manager->selected_gun_index].ammo <= 0 &&
      !isReloading) {
    isReloading = true;        // Start reloading
    currentReloadTimer = 0.0f; // Reset the timer
  }

  // Check if the player WANTS to reload (pressed 'R')
  if (IsKeyPressed(KEY_R) && !isReloading &&
      gun_manager->guns[gun_manager->selected_gun_index].ammo <
          gun_manager->guns[gun_manager->selected_gun_index].ammo_capacity) {
    isReloading = true;        // Start reloading
    currentReloadTimer = 0.0f; // Reset the timer
  }

  if (isReloading) {
    // Display reloading message
    DrawText("Reloading...", GetScreenWidth() / 2 - 20, GetScreenHeight() / 2,
             20,
             DARKGRAY); // Increment the reload timer by the time elapsed since
                        // the last frame
    currentReloadTimer += GetFrameTime();

    // Check if the reload duration has been met
    if (currentReloadTimer >=
        gun_manager->guns[gun_manager->selected_gun_index].reload_time) {
      gun_manager->guns[gun_manager->selected_gun_index].ammo =
          gun_manager->guns[gun_manager->selected_gun_index]
              .ammo_capacity;    // Refill ammo
      isReloading = false;       // Stop reloading
      currentReloadTimer = 0.0f; // Reset timer for next time
    }
  }

  // Reset velocity after movement
  player.velocity = {0, 0};
}

void CharacterManager::CheckBulletEnemyCollision(GunManager *gun_manager) {
  std::vector<character> killedEnemies; // Store enemies killed this frame

  for (auto &bullet : gun_manager->bullets) {
    if (!bullet.active || !bullet.isPlayerBullet)
      continue;

    for (auto &enemy : enemies) {
      Rectangle bulletRect = {bullet.Position.x - 5, bullet.Position.y - 5, 10,
                              10};
      Rectangle enemyRect = {enemy.position.x, enemy.position.y, 50, 50};

      if (CheckCollisionRecs(bulletRect, enemyRect)) {
        enemy.health -= bullet.damage;
        bullet.active = false;

        if (enemy.health <= 0) {
          killedEnemies.push_back(enemy);
        }

        break;
      }
    }
  }

  player.score += static_cast<int>(killedEnemies.size());

  for (const auto &e : killedEnemies) {
    int count = GetRandomValue(3, 6);
    for (int i = 0; i < count; ++i) {
      particle.spawnParticle(e.position.x, e.position.y, GetRandomValue(10, 15),
                             100, BLOOD);

      if (gun_manager->selected_gun_index == 8) {
        particle.spawnParticle(e.position.x, e.position.y,
                               GetRandomValue(10, 15), 100, EXPLOSION);
      }
    }
  }

  // Clean up dead enemies
  enemies.erase(
      std::remove_if(enemies.begin(), enemies.end(),
                     [](const character &e) { return e.health <= 0; }),
      enemies.end());
}

void CharacterManager::UpdateEnemy(GunManager *gun_manager) {
  // Only spawn once
  if (!enemy_spawned) {
    spawnEnemies();
    enemy_spawned = true;
  }

  for (auto &e : enemies) {
    Vector2 direction =
        Vector2Normalize(Vector2Subtract(player.position, e.position));
    float distance = Vector2Distance(player.position, e.position);
    // Move towards player
    if (distance >= 250) {
      e.velocity = Vector2Scale(direction, 2.0f);
      e.position = Vector2Add(e.position, e.velocity);
      // Clamp to screen
      e.position.x = Clamp(e.position.x, 0, GetScreenWidth() - 50);
      e.position.y = Clamp(e.position.y, 0, GetScreenHeight() - 50);
    }

    // Shooting logic - each enemy has its own cooldown
    if (e.shootCooldown > 0.0f)
      e.shootCooldown -= GetFrameTime();

    if (distance < 300 && e.ammo > 0 && e.shootCooldown <= 0.0f) {
      // Temporarily change the selected gun to the enemy's gun
      int previousGunIndex = gun_manager->selected_gun_index;
      gun_manager->selected_gun_index = e.current_gun_id;

      // Shoot with the enemy's gun
      gun_manager->ShootGun(e.position, direction);

      // Restore previous selected gun
      gun_manager->selected_gun_index = previousGunIndex;

      e.ammo--;
      e.shootCooldown = gun_manager->guns[e.current_gun_id].fire_rate;
    }

    // check for collision of bullets with player
    for (auto &bullet : gun_manager->bullets) {
      if (!bullet.active || bullet.isPlayerBullet)
        continue;

      Rectangle bulletRect = {bullet.Position.x - 5, bullet.Position.y - 5, 10,
                              10};
      Rectangle playerRect = {player.position.x, player.position.y, 50, 50};

      if (CheckCollisionRecs(bulletRect, playerRect)) {
        if (player.armor > 0) {
          player.armor -= bullet.damage; // Damage armor first
        } else {
          player.health -= bullet.damage; // Damage health if no armor left
        }
        bullet.active = false;
        break;
      }
    }
  }

  // check for collision of player with enemies
  for (auto &e : enemies) {
    Rectangle enemy_rect = {e.position.x, e.position.y, 50, 50};
    Rectangle player_rect = {player.position.x, player.position.y, 50, 50};

    if (CheckCollisionRecs(enemy_rect, player_rect)) {
      Vector2 pushDir =
          Vector2Normalize(Vector2Subtract(e.position, player.position));
      e.position = Vector2Add(e.position,
                              Vector2Scale(pushDir, 5.0f)); // Push enemy away
      if (player.armor > 0) {
        player.armor -= 1; // Damage armor first
      } else {
        player.health -= 1; // Damage health if no armor left
      }
      if (e.armor > 0) {
        e.armor -= 1; // Damage armor first
      } else {
        e.health -= 1; // Damage health if no armor left
      }
    }
  }

  // remove enemy
  enemies.erase(
      std::remove_if(enemies.begin(), enemies.end(),
                     [](const character &e) { return e.health <= 0; }),
      enemies.end());

  // prevent enemies from overlapping
  for (size_t i = 0; i < enemies.size(); ++i) {
    for (size_t j = i + 1; j < enemies.size(); ++j) {
      Rectangle a = {enemies[i].position.x, enemies[i].position.y, 50, 50};
      Rectangle b = {enemies[j].position.x, enemies[j].position.y, 50, 50};

      if (CheckCollisionRecs(a, b)) {
        Vector2 dir = Vector2Normalize(
            Vector2Subtract(enemies[j].position, enemies[i].position));
        enemies[i].position =
            Vector2Add(enemies[i].position, Vector2Scale(dir, -2.5f));
        enemies[j].position =
            Vector2Add(enemies[j].position, Vector2Scale(dir, 2.5f));
      }
    }
  }
}

void CharacterManager::DrawPlayer() {
  // Draw player
  DrawRectangle(player.position.x, player.position.y, 50, 50, BLUE);
}

void CharacterManager::DrawEnemy() {
  for (const auto &e : enemies) {
    DrawRectangle(e.position.x, e.position.y, 50, 50, RED);
  }
}

void CharacterManager::spawnEnemies() {
  for (int i = 0; i < enemy_count; i++) {
    character new_enemy;
    new_enemy.health = 100;
    new_enemy.armor = 50;
    new_enemy.ammo = 99999999;
    new_enemy.velocity = {0, 0};
    new_enemy.score = 0;
    new_enemy.shootCooldown = 0.0f; // Initialize cooldown

    // Assign a random gun ID (0 to 8 inclusive)
    new_enemy.current_gun_id = static_cast<gun_id>(GetRandomValue(0, 6));

    // Avoid spawning too close to player
    do {
      new_enemy.position.x = GetRandomValue(0, GetScreenWidth() - 50);
      new_enemy.position.y = GetRandomValue(0, GetScreenHeight() - 50);
    } while (Vector2Distance(new_enemy.position, player.position) < 150);

    enemies.push_back(new_enemy);
  }
}
