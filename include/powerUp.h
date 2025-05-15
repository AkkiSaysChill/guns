#pragma once

#include "characters.h"
#include <raylib.h>
#include <vector>

struct powerUp_t {
  Vector2 position;
  float size;
  Texture2D texture;
  int type; // 0: health, 1: armor, 2: ammo
};

class powerUp {
public:
  ~powerUp();
  std::vector<powerUp_t> powerups;
  void spawnPowerUp(float x, float y, float size, int type);
  void UpdatePowerUps();
  void DrawPowerUps();
  void CkeckCollisionWithPlayer(Vector2 playerPosition, float playerSize,
                                CharacterManager *character_manager);
};
