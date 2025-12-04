#pragma once
#include "raylib.h"
#include "Player.h"
#include "Enemy.h"
#include "Background.h"
#include <vector>
#include <memory>

enum GameState { MENU, PLAYING, GAME_OVER, WIN };

class Game {
private:
    int screenWidth;
    int screenHeight;
    bool shouldQuit;
    GameState state;

    // Assets 
    Texture2D texPlayer, texFire, texEnemy, texBoss, texExplosion, texMenu, texWin;
    Sound sndShoot[5], sndExplosion, sndBossHit;
    Music musicBg;

    // Objetos
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Projectile>> projectiles;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::unique_ptr<Background> background;

    // Lógica interna
    void HandleInput();
    void UpdateGame();
    void DrawGame();
    void ResetGame();
    void LoadAssets();
    void UnloadAssets();

public:
    Game(int width, int height, const char* title);
    ~Game();

    void Run(); // loop principal
};