#include "Projectile.h"

Projectile::Projectile(float x, float y, float spd) 
    : GameObject(x, y), speed(spd), color(YELLOW) {
}

void Projectile::Update() {
    position.y -= speed; // Faz o tiro subir
    
    // Desativa se sai da tela (economiza memória e processamento)
    if (position.y < -10) {
        active = false;
    }
}

void Projectile::Draw() {
    DrawEllipse((int)position.x, (int)position.y, 1.5f, 10.0f, color);
}

Rectangle Projectile::GetBounds() const {
    return { position.x - 1.5f, position.y - 5.0f, 3.0f, 10.0f };
}