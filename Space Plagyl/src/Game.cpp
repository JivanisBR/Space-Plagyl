#include "Game.h"
#include <algorithm> // Para std::remove_if

Game::Game(int width, int height, const char* title) 
    : screenWidth(width), screenHeight(height), shouldQuit(false), state(MENU) {
    InitWindow(screenWidth, screenHeight, title);
    InitAudioDevice();
    SetTargetFPS(120);
    LoadAssets();
    
    // Inicializa o Background
    background = std::make_unique<Background>();
    
    // Música
    PlayMusicStream(musicBg);
    SetMusicVolume(musicBg, 1.0f);
}

Game::~Game() {
    UnloadAssets();
    CloseAudioDevice();
    CloseWindow();
}

void Game::LoadAssets() {
    // Assets de Texturas
    texMenu = LoadTexture("SPACE PLAGYL.png");
    texWin = LoadTexture("winner.png"); 
    texCredits = LoadTexture("CREDITS.png"); 
    texMort = LoadTexture("mort.png");
    
    texPlayer = LoadTexture("navee2.png");
    texFire = LoadTexture("fogos.png");
    texExplosion = LoadTexture("esplosoes.png");
    
    texEnemy = LoadTexture("meteoro.png");
    texBoss = LoadTexture("boss.png");
    
    // Sons
    sndShoot[0] = LoadSound("sobio1.mp3");
    sndShoot[1] = LoadSound("sobio2.mp3");
    sndShoot[2] = LoadSound("sobio3.mp3");
    sndShoot[3] = LoadSound("sobio4.mp3");
    sndShoot[4] = LoadSound("sobio5.mp3");
    
    sndExplosion = LoadSound("bomb.mp3");
    sndBossHit = LoadSound("tiroboss.wav"); // Ou outro som de hit
    
    musicBg = LoadMusicStream("Space Plagyl Soundtrack.mp3");
}

void Game::UnloadAssets() {
    UnloadTexture(texMenu);
    UnloadTexture(texWin);
    UnloadTexture(texPlayer);
    UnloadTexture(texFire);
    UnloadTexture(texExplosion);
    UnloadTexture(texEnemy);
    UnloadTexture(texBoss);
    
    for(int i=0; i<5; i++) UnloadSound(sndShoot[i]);
    UnloadSound(sndExplosion);
    UnloadSound(sndBossHit);
    UnloadMusicStream(musicBg);
}

void Game::ResetGame() {
    // Reinicia o jogo
    player = std::make_unique<Player>(390, 600, texPlayer, texFire, sndShoot[0]); // Começa com som 0
    projectiles.clear();
    enemies.clear();
    
    // Spawn inicial
    enemies.push_back(std::make_unique<Enemy>(GetRandomValue(50, 1100), -10, texEnemy, 0));
}

void Game::Run() {
    bool densitySelected = false;
    while (!densitySelected && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("ESCOLHA A DENSIDADE DO CENARIO", 270, 250, 40, WHITE);
        DrawText("1 - BAIXA", 400, 350, 20, GRAY);
        DrawText("2 - MEDIA", 400, 380, 20, GRAY);
        DrawText("3 - ALTA", 400, 410, 20, GRAY);
        EndDrawing();
        
        if (IsKeyPressed(KEY_ONE)) { background->Init("baixa"); densitySelected = true; }
        if (IsKeyPressed(KEY_TWO)) { background->Init("media"); densitySelected = true; }
        if (IsKeyPressed(KEY_THREE)) { background->Init("alta"); densitySelected = true; }
    }

    ResetGame();

    // Loop Principal
    while (!WindowShouldClose() && !shouldQuit) {
        UpdateMusicStream(musicBg);
        HandleInput();
        
        if (state == PLAYING) {
            UpdateGame();
        }
        
        DrawGame();
    }
}

void Game::HandleInput() {
    if (state == MENU) {
        if (IsKeyPressed(KEY_SPACE)) state = PLAYING;
    }
    else if (state == WIN || state == GAME_OVER) {
        if (IsKeyPressed(KEY_R)) {
            ResetGame();
            state = PLAYING;
        }
    }
    else if (state == PLAYING) {
        if (IsKeyPressed(KEY_P)) state = MENU; 
    }
}

void Game::UpdateGame() {
    bool turbo = IsKeyDown(KEY_LEFT_SHIFT);
    background->Update(turbo);
    
    if (player->IsActive()) {
        player->Update();
        player->Shoot(projectiles); 
    } else {
        state = GAME_OVER;
    }

    // Atualiza Projéteis
    for (auto& p : projectiles) p->Update();
    
    // Remove projéteis inativos
    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), 
        [](const std::unique_ptr<Projectile>& p) { return !p->IsActive(); }), projectiles.end());

    // Atualiza Inimigos
    for (auto& e : enemies) e->Update();

    // Remove inimigos inativos
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), 
        [](const std::unique_ptr<Enemy>& e) { return !e->IsActive(); }), enemies.end());

    // --- COLISÕES ---
    // 1. Projétil vs Inimigo
    for (auto& p : projectiles) {
        if (!p->IsActive()) continue;
        for (auto& e : enemies) {
            if (!e->IsActive()) continue;
            
            if (CheckCollisionRecs(p->GetBounds(), e->GetBounds())) {
                p->SetActive(false);
                e->TakeDamage(1);
                
                if (!e->IsActive()) {
                    PlaySound(sndExplosion);
                    player->AddScore(1);
                } else {
                    // Som de hit no boss
                    if(e->GetType() == 1) PlaySound(sndBossHit);
                }
            }
        }
    }

    // 2. Player vs Inimigo
    if (player->IsActive()) {
        for (auto& e : enemies) {
            if (!e->IsActive()) continue;
            // Hitbox do meteoro um pouco menor para ser justo
            Rectangle enemyRec = e->GetBounds();
            if (CheckCollisionRecs(player->GetBounds(), enemyRec)) {
                player->TakeDamage(1);
                PlaySound(sndExplosion);
            }
        }
    }

    // Spawn simples de meteoros se tiver poucos
    if (enemies.size() < 3 + (player->GetScore() / 5)) { // Dificuldade progressiva
        enemies.push_back(std::make_unique<Enemy>(GetRandomValue(50, 1100), -50, texEnemy, 0));
    }

    // Spawn do Boss
    bool bossExists = false;
    for(const auto& e : enemies) if(e->GetType() == 1) bossExists = true;
    
    if (!bossExists && player->GetScore() >= 10 && player->GetScore() % 10 == 0) {
        enemies.push_back(std::make_unique<Enemy>(400, -200, texBoss, 1));
    }
}

void Game::DrawGame() {
    BeginDrawing();
    ClearBackground(BLACK);

    background->Draw();

    if (state == MENU) {
        DrawTexture(texMenu, 0, 0, WHITE);
    }
    else if (state == PLAYING) {
        player->Draw();
        for (const auto& p : projectiles) p->Draw();
        for (const auto& e : enemies) e->Draw();
        
        DrawText(TextFormat("Points: %d", player->GetScore()), 520, 50, 20, WHITE);
    }
    else if (state == WIN) {
        DrawTexture(texWin, 100, 100, WHITE);
        DrawText("APERTE R PARA JOGAR DE NOVO", 440, 480, 20, WHITE);
    }
    else if (state == GAME_OVER) {
        // Desenhar tela de morte
        DrawText("GAME OVER", 500, 300, 40, RED);
        DrawText("APERTE R PARA TENTAR DE NOVO", 470, 420, 20, WHITE);
    }

    EndDrawing();
}