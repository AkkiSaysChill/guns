#include "characters.h"
#include "guns.h"
#include "powerUp.h"
#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>
#include <string>

// helper to convert gun id to gun name
inline const char *GunIDToString(gun_id id) {
  switch (id) {
  case GUN_PISTOL:
    return "Pistol";
  case GUN_RIFLE:
    return "Rifle";
  case GUN_SHOTGUN:
    return "Shotgun";
  case GUN_SNIPER:
    return "Sniper";
  case GUN_SUBMACHINEGUN:
    return "SMG";
  case GUN_ASSAULT_RIFLE:
    return "Assault Rifle";
  case GUN_LMG:
    return "LMG";
  case GUN_GRENADE_LAUNCHER:
    return "Grenade Launcher";
  case GUN_ROCKET_LAUNCHER:
    return "Rocket Launcher";
  default:
    return "Unknown";
  }
}

int main() {
  InitWindow(GetScreenWidth(), GetScreenHeight(), "Raylib Project");
  SetTargetFPS(60);

  ToggleFullscreen();

  CharacterManager character_manager;

  // power up
  powerUp powerup;

  // gun related stuff
  GunManager gun_manager;
  int next_id = 0;

  std::vector<gun> guns = gun_manager.guns;

  rlImGuiSetup(true);

  powerup.spawnPowerUp(GetRandomValue(0, GetScreenWidth() - 50),
                       GetRandomValue(0, GetScreenHeight() - 50), 50,
                       GetRandomValue(0, 1)); // Random powerup type

  float gameTick = 0.0f;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    character_manager.UpdatePlayer(&gun_manager);
    character_manager.DrawPlayer();
    character_manager.UpdateEnemy(&gun_manager);
    character_manager.DrawEnemy();

    character_manager.CheckBulletEnemyCollision(&gun_manager);

    // gun_manager.DrawGun(character_manager.player.position,
    // character_manager.enemies.begin()->position);

    if (!character_manager.enemies.empty()) {
      Vector2 playerPos = character_manager.player.position;

      std::vector<Vector2> enemyPositions;
      std::vector<int> enemy_gun_ids;
      for (const auto &e : character_manager.enemies) {
        enemyPositions.push_back(e.position);
        enemy_gun_ids.push_back(e.current_gun_id);
      }

      gun_manager.DrawGun(playerPos, enemyPositions, enemy_gun_ids);
    }

    gun_manager.UpdateBullets();
    gun_manager.DrawBullets();
    powerup.DrawPowerUps();
    powerup.UpdatePowerUps();
    powerup.CkeckCollisionWithPlayer(character_manager.player.position, 50,
                                     &character_manager);

    // make powerup spawn at random intervals

    float nextSpawnTime = GetRandomValue(15, 30);

    gameTick += GetFrameTime();

    if (gameTick >= nextSpawnTime) {
      powerup.spawnPowerUp(GetRandomValue(0, GetScreenWidth() - 50),
                           GetRandomValue(0, GetScreenHeight() - 50), 50,
                           GetRandomValue(0, 2)); // Random powerup type

      gameTick = 0.0f;
      nextSpawnTime = GetRandomValue(3, 7); // Set a new random interval
    }

    Vector2 start = character_manager.player.position;
    Vector2 target = GetMousePosition();

    std::string score = std::to_string(character_manager.player.score);

    DrawText(score.c_str(), 10, 70, 20, BLACK);

    if (character_manager.enemies.size() <= 2) {
      character_manager.spawnEnemies();
    }

    // Increase difficulty based on score
    character_manager.enemy_count =
        std::min(50, character_manager.player.score / 5 + 5);

    int barWidth = (character_manager.player.health * 200) / 1000;
    int armorWidth =
        (character_manager.player.armor * 200) / 1000; // 1000 is max armor
    DrawRectangle(10, 0, barWidth, 20, RED);
    DrawRectangle(10, 20, armorWidth, 20, BLUE);

    // === ImGui stuff ===
    rlImGuiBegin();
    ImGui::Begin("Enemy Stats");

    // draw the stats of the enemies
    ImGui::Text("Enemies:");
    for (const auto &e : character_manager.enemies) {
      ImGui::Text("Health: %d", e.health);
      ImGui::Text("Armor: %d", e.armor);
      ImGui::Text("Ammo: %d", e.ammo);
      ImGui::Text("Score: %d", e.score);
      ImGui::Text("gun %s", GunIDToString(e.current_gun_id));
      ImGui::Separator();
    }

    ImGui::End();

    // Player stats
    ImGui::Begin("Player Stats");
    ImGui::Text("Player:");

    if (!gun_manager.guns.empty()) {
      std::vector<const char *> gun_names;
      for (const auto &g : gun_manager.guns) {
        gun_names.push_back(g.name);
      }

      // Clamp index to avoid invalid access
      if (gun_manager.selected_gun_index >= gun_manager.guns.size())
        gun_manager.selected_gun_index = 0;

      ImGui::Text("Current Gun: %s",
                  gun_manager.guns[gun_manager.selected_gun_index].name);

      ImGui::Combo("Guns", &gun_manager.selected_gun_index, gun_names.data(),
                   gun_names.size());
      ImGui::Text("Damage: %d",
                  gun_manager.guns[gun_manager.selected_gun_index].damage);
      ImGui::Text(
          "Ammo Capacity: %d",
          gun_manager.guns[gun_manager.selected_gun_index].ammo_capacity);
      ImGui::Text("Range: %d",
                  gun_manager.guns[gun_manager.selected_gun_index].range);
      ImGui::Text("Reload Time: %.2f",
                  gun_manager.guns[gun_manager.selected_gun_index].reload_time);
      ImGui::Text("Fire Rate: %.2f",
                  gun_manager.guns[gun_manager.selected_gun_index].fire_rate);
      ImGui::Text("Current Ammo: %d",
                  gun_manager.guns[gun_manager.selected_gun_index].ammo);
      ImGui::Text("Health: %d", character_manager.player.health);
      ImGui::Text("Armor: %d", character_manager.player.armor);

      ImGui::SliderInt("Armor", &character_manager.player.armor, 0, 1000);
      ImGui::SliderInt("Health", &character_manager.player.health, 0, 1000);

      if (ImGui::Button("Spawn Enemy")) {
        character_manager.spawnEnemies();
      }

    } else {
      ImGui::Text("No guns available!");
    }

    ImGui::End();
    rlImGuiEnd();

    EndDrawing();
  }

  rlImGuiShutdown();

  CloseWindow();
  return 0;
}
