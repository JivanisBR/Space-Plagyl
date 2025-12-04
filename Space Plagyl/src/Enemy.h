#pragma once
#include "GameObject.h"

// Serve tanto para meteoros quanto para o Boss
class Enemy : public GameObject {
private:
    Texture2D texture;
    Vector2 velocity;
    int type; // 0 = meteoro, 1 = boss
    int health;

public:
    Enemy(float x, float y, Texture2D tex, int enemyType);
    
    void Update() override;
    void Draw() override;
    Rectangle GetBounds() const override;
    
    void TakeDamage(int amount);
    int GetHealth() const { return health; }
    int GetType() const { return type; }
};