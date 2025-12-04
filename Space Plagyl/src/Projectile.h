#pragma once
#include "GameObject.h"

class Projectile : public GameObject {
private:
    float speed;
    Color color;

public:
    Projectile(float x, float y, float spd);
    
    void Update() override;
    void Draw() override;
    Rectangle GetBounds() const override;
};