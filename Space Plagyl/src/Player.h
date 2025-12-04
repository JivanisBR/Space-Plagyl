#pragma once
#include "GameObject.h"
#include "Projectile.h"
#include <vector>
#include <memory> 

class Player : public GameObject {
private:
    Texture2D texture;
    Texture2D fireTexture; 
    float speed;
    int health;
    int score;
    
    // animação do fogo
    int fireFrameCounter; 
    
    // tiro
    float shootCooldown;
    Sound shootSound;

public:
    Player(float x, float y, Texture2D tex, Texture2D fireTex, Sound shotSfx);
    
    void Update() override;
    void Draw() override;
    Rectangle GetBounds() const override;

    // Ações do Player
    void Shoot(std::vector<std::unique_ptr<Projectile>>& projectiles);
    void TakeDamage(int damage);
    
    int GetHealth() const { return health; }
    int GetScore() const { return score; }
    void AddScore(int value) { score += value; }
};