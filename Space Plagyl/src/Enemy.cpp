#include "Enemy.h"

Enemy::Enemy(float x, float y, Texture2D tex, int enemyType)
    : GameObject(x, y), texture(tex), type(enemyType) {
    
    // Se for Boss (type 1), vida maior. Se meteoro (type 0), vida 1.
    health = (type == 1) ? 50 : 1; 
    
    // Velocidade aleatória para meteoros
    velocity.y = (float)GetRandomValue(1, 4);
    velocity.x = (type == 1) ? 2.0f : 0.0f; // Boss se move lateralmente
}

void Enemy::Update() {
    if (type == 0) { // Meteoro
        position.y += velocity.y;
        if (position.y > 750) {
            // Respawna la em cima (reciclagem)
            position.y = (float)GetRandomValue(-800, -10);
            position.x = (float)GetRandomValue(30, 1150);
            velocity.y = (float)GetRandomValue(1, 4);
        }
    } 
    else if (type == 1) { // Boss
        // Boss desce e se move
        if (position.y < 100) position.y++;
        
        position.x += velocity.x;
        if (position.x > 1000 || position.x < 0) velocity.x *= -1; // Bate e volta
    }
}

void Enemy::Draw() {
    DrawTexture(texture, (int)position.x, (int)position.y, WHITE);
    if (type == 1) {
        DrawText(TextFormat("%d", health), (int)position.x + 90, (int)position.y, 20, WHITE);
    }
}

Rectangle Enemy::GetBounds() const {
    return { position.x, position.y, (float)texture.width, (float)texture.height };
}

void Enemy::TakeDamage(int amount) {
    health -= amount;
    if (health <= 0) active = false;
}