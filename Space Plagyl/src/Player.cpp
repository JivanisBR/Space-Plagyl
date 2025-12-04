#include "Player.h"

Player::Player(float x, float y, Texture2D tex, Texture2D fireTex, Sound shotSfx)
    : GameObject(x, y), texture(tex), fireTexture(fireTex), shootSound(shotSfx) {
    speed = 3.0f;
    health = 1;
    score = 0;
    fireFrameCounter = 0;
    shootCooldown = 0;
}

void Player::Update() {
    // Movimentação
    if ((IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) && position.x > 0) position.x -= speed;
    if ((IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) && position.x < 1115) position.x += speed;
    if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) && position.y > 300) position.y -= speed;
    if ((IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) && position.y < 600) position.y += speed;

    // Animação do fogo
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D) || IsKeyDown(KEY_W) || IsKeyDown(KEY_S)) {
        fireFrameCounter++;
        if (fireFrameCounter > 90) fireFrameCounter = 0;
    }

    // Cooldown
    if (shootCooldown > 0) shootCooldown--;
}

void Player::Shoot(std::vector<std::unique_ptr<Projectile>>& projectiles) {
    if (IsKeyPressed(KEY_SPACE) && shootCooldown <= 0) {
        // Cria um novo projétil e add ao vetor
        projectiles.push_back(std::make_unique<Projectile>(position.x + 48, position.y, 5.0f));
        PlaySound(shootSound);
        shootCooldown = 10; // delay pra não spammar
    }
}

void Player::Draw() {
    DrawTexture(texture, (int)position.x, (int)position.y, WHITE);

    // Lógica de desenho do fogo baseada no contador
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D) || IsKeyDown(KEY_W) || IsKeyDown(KEY_S)) {
        Rectangle sourceRec;
        if (fireFrameCounter <= 30) sourceRec = {0, 0, 18, 23};
        else if (fireFrameCounter <= 60) sourceRec = {19, 0, 16, 23};
        else sourceRec = {35, 0, 17, 23};

        Vector2 firePos = { position.x + 41, position.y + 84 };
        DrawTextureRec(fireTexture, sourceRec, firePos, WHITE);
    }
}

Rectangle Player::GetBounds() const {
    // Hitbox
    return { position.x + 10, position.y + 10, (float)texture.width - 20, (float)texture.height - 20 };
}

void Player::TakeDamage(int damage) {
    health -= damage;
    if (health <= 0) active = false;
}