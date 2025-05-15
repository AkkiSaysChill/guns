#include "powerUp.h"
#include <cmath>
#include <raylib.h>
#include <raymath.h>

void powerUp::spawnPowerUp(float x, float y, float size, int type) {
  powerUp_t newPowerUp;
  newPowerUp.position = {x, y};
  newPowerUp.size = size;
  newPowerUp.type = type;

  // select texture based on type

  switch (type) {
  case 0:
    newPowerUp.texture = LoadTexture("assets/health.png");
    break;
  case 1:
    newPowerUp.texture = LoadTexture("assets/armor.png");
    break;
    // case 2:
    //   newPowerUp.texture = LoadTexture("assets/ammo.png");
    //   break;
  }

  powerups.push_back(newPowerUp);
}

powerUp::~powerUp() {
  for (auto &p : powerups) {
    UnloadTexture(p.texture);
  }
}

void powerUp::DrawPowerUps() {
  for (const auto &p : powerups) {
    DrawTextureEx(p.texture, p.position,
                  0.0f,                     // Rotation
                  p.size / p.texture.width, // Scale
                  WHITE                     // Tint
    );
  }
}

void powerUp::CkeckCollisionWithPlayer(Vector2 playerPosition, float playerSize,
                                       CharacterManager *character_manager) {
  for (size_t i = 0; i < powerups.size();) {
    float dist = Vector2Distance(playerPosition, powerups[i].position);
    if (dist < (playerSize + powerups[i].size) / 2) {
      // Apply effect based on type
      if (powerups[i].type == 0) {
        character_manager->player.health += 100;
      } else if (powerups[i].type == 1) {
        character_manager->player.armor += 50;
      }
      // Cap values (optional)
      character_manager->player.health =
          std::min(character_manager->player.health, 1000);
      character_manager->player.armor =
          std::min(character_manager->player.armor, 1000);

      powerups.erase(powerups.begin() + i);
    } else {
      i++;
    }
  }
}

void powerUp::UpdatePowerUps() {
  float time = GetTime();

  for (size_t i = 0; i < powerups.size(); ++i) {
    float frequency = 2.0f;
    float amplitude = 10.0f;
    powerups[i].position.x += sinf(time * frequency + i) * 0.5f;
  }
}
