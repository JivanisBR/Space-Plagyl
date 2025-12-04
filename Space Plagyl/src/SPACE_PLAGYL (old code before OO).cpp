#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <cmath>
Texture2D menui, credits, nav, win_tex, mort, explo, fire1, meteoro_tex, iboss;
Sound sob[5], bomb, ai, oh;
Music background_music;
//----------------------------------------------------------------------------------
// ESTRUTURAS DE DADOS PARA O FUNDO DE ESTRELAS
//----------------------------------------------------------------------------------
struct DustParticle {
    Vector2 position;
    Color color;
};

struct Star {
    Vector2 position;
    int size;
    int speed_delay;
    int move_counter;
    bool growing;
    int anim_timer;
    int anim_limit;
    int base_size;
    int growth_range;
};

struct NebulaParticle {
    Vector2 offset;
    float width;
    float height;
    Color color;
    int star_size;
    bool star_growing;
    int star_timer;
    int star_limit;
};

struct Nebula {
    Vector2 position;
    Color base_color;
    int speed_delay;
    int move_counter;
    std::vector<NebulaParticle> particles;
};

int DUST_COUNT, BG_STAR_COUNT, FG_STAR_COUNT, NEBULA_COUNT, PARTICLES_PER_NEBULA, STAR_PARTICLE_COUNT;
std::vector<DustParticle> dust;
std::vector<Star> background_stars;
std::vector<Star> foreground_stars;
std::vector<Nebula> nebulas;
RenderTexture2D static_dust_texture;
int opt = 1, sob_index = 0;
bool menu = true, creds = false, quit = false, boss = false, winn = false, pause = false;
int fall, bum = 0, bump = 0, vmin = 1, vmax = 4, pisc = 0;
int mort_x = 250, mort_y = 70;
int pontos = 0, storo1 = 0, storo2 = 0, storo3 = 0, wait = 0, ng = 0;
int nave_x = 390, nave_y = 600, velo = 3, tvel = 5;
int exp_x, exp_y;
int death_x, death_y, vida = 1;
int fire_x, fire_y;
int projetil_x, projetil_y;
int foga = 0, fogd = 0, projetil = 0, cdpause = 0;
int sol_x, sol_y;
int bot_x, bot_y;
int expm_x, expm_y;
int meteoro_x, meteoro_y;
bool meter = true;
int hitbox1x, hitbox1y, hitbox2x, hitbox2y;
bool hit = false;
int death_sound_delay = 0;
int boss_death_sound_delay = 0;
int boss_bomb_count = 0;
int bossx = 400, bossy = -220, vidaboss = 50, tirobossx, tirobossy, d, t, v = 1;

//----------------------------------------------------------------------------------
// FUNÇÕES AUXILIARES PARA O FUNDO DE ESTRELAS
//----------------------------------------------------------------------------------
void DrawStarShape(Vector2 center, float size, Color color)
{
    if (size < 1.0f) return;

    // Caso especial para estrelas minúsculas 
    if (size <= 2) {
        DrawRectangle(center.x - size, center.y, size * 2 + 1, 1, color);
        DrawRectangle(center.x, center.y - size, 1, size * 2 + 1, color);
        return;
    }

    // Lógica de desenho para estrelas maiores
    float main_length = size;
    float width = (size / 3.0f) + 1.0f;

    // Raio Vertical
    DrawTriangle({center.x, center.y - main_length}, {center.x - width, center.y}, {center.x + width, center.y}, color);
    DrawTriangle({center.x, center.y + main_length}, {center.x + width, center.y}, {center.x - width, center.y}, color);
    
    // Raio Horizontal
    DrawTriangle({center.x + main_length, center.y}, {center.x, center.y - width}, {center.x, center.y + width}, color);
    DrawTriangle({center.x - main_length, center.y}, {center.x, center.y + width}, {center.x, center.y - width}, color);
}

void GenerateNebulaShape(Nebula& nebula, int particle_count, int star_particle_count) {
    nebula.particles.clear();
    nebula.particles.resize(particle_count);

    int particles_drawn = 0;
    int num_arms = 3;
    int particles_per_arm = particle_count / num_arms;
    if (particles_per_arm == 0) particles_per_arm = 1;

    for (int b = 0; b < num_arms; ++b) {
        Vector2 walker = {0, 0};
        Vector2 direction = {0, 0};
        int steps_left = 0;

        for (int p = 0; p < particles_per_arm; ++p) {
            if (particles_drawn >= particle_count) break;

            if (steps_left <= 0) {
                steps_left = GetRandomValue(10, 20);
                int dir_choice = GetRandomValue(0, 7);
                switch (dir_choice) {
                    case 0: direction = {0, -1}; break; case 1: direction = {1, -1}; break;
                    case 2: direction = {1, 0}; break;  case 3: direction = {1, 1}; break;
                    case 4: direction = {0, 1}; break;  case 5: direction = {-1, 1}; break;
                    case 6: direction = {-1, 0}; break; case 7: direction = {-1, -1}; break;
                }
            }

            walker.x += direction.x * GetRandomValue(4, 8);
            walker.y += direction.y * GetRandomValue(4, 8);
            steps_left--;

            int idx = particles_drawn++;
            
            nebula.particles[idx].offset.x = walker.x + GetRandomValue(-20, 20);
            nebula.particles[idx].offset.y = walker.y + GetRandomValue(-20, 20);
            
            if (idx >= star_particle_count) {
                nebula.particles[idx].width = GetRandomValue(20, 60);
                nebula.particles[idx].height = GetRandomValue(20, 60);
                unsigned char opacity = GetRandomValue(2, 8);
                nebula.particles[idx].color = {nebula.base_color.r, nebula.base_color.g, nebula.base_color.b, opacity};
            } else {
                nebula.particles[idx].width = 0;
                nebula.particles[idx].height = 0;
                nebula.particles[idx].color = nebula.base_color;
                nebula.particles[idx].star_size = GetRandomValue(1, 3);
                nebula.particles[idx].star_growing = GetRandomValue(0, 1);
                nebula.particles[idx].star_limit = GetRandomValue(5, 15);
                nebula.particles[idx].star_timer = 0;
            }
        }
    }
}

void controle() {
    if (menu) {
        cdpause++;
        if (IsKeyPressed(KEY_S) && cdpause > 50 || IsKeyPressed(KEY_DOWN) && cdpause > 50) {
            opt++;
            cdpause = 0;
        }
        if (opt > 3) opt = 1;
        if (IsKeyPressed(KEY_W) && cdpause > 50 || IsKeyPressed(KEY_UP) && cdpause > 50) {
            opt--;
            cdpause = 0;
        }
        if (opt < 1) opt = 3;
        if (opt == 1 && IsKeyPressed(KEY_SPACE)) {
            menu = false;
        }
        if (opt == 2 && IsKeyPressed(KEY_SPACE)) {
            creds = true;
        }
        if (creds && IsKeyPressed(KEY_ENTER)) {
            creds = false;
        }
        if (opt == 3 && IsKeyPressed(KEY_SPACE)) {
            quit = true;
        }
    } else {
        if (IsKeyPressed(KEY_SPACE) && projetil == 0) {
            projetil = 1;
            PlaySound(sob[sob_index]);
            sob_index = (sob_index + 1) % 5;
        }
        if (IsKeyDown(KEY_A) && vida > 0 || IsKeyDown(KEY_LEFT) && vida > 0) {
            nave_x -= velo;
            death_x = nave_x - velo;
            fire_x = nave_x + 41;
            exp_x -= velo;
            hitbox1x = nave_x + 10;
            hitbox2x = nave_x + 38;
            foga++;
            if (foga > 90) foga = 0;
        }
        if (IsKeyDown(KEY_D) && vida > 0 || IsKeyDown(KEY_RIGHT) && vida > 0) {
            nave_x += velo;
            death_x = nave_x + velo;
            fire_x = nave_x + 41;
            exp_x += velo;
            hitbox1x = nave_x + 10;
            hitbox2x = nave_x + 38;
            fogd++;
            if (fogd > 90) fogd = 0;
        }
        if (IsKeyDown(KEY_W) && vida > 0 || IsKeyDown(KEY_UP) && vida > 0) {
            nave_y -= velo;
            death_y = nave_y - velo;
            fire_y = nave_y + 84;
            exp_y -= velo;
            hitbox1y = nave_y + 40;
            hitbox2y = nave_y + 10;
            fogd++;
            if (fogd > 90) fogd = 0;
        }
        if (IsKeyDown(KEY_S) && vida > 0 || IsKeyDown(KEY_DOWN) && vida > 0) {
            nave_y += velo;
            death_y = nave_y + velo;
            fire_y = nave_y + 84;
            exp_y += velo;
            hitbox1y = nave_y + 40;
            hitbox2y = nave_y + 10;
            fogd++;
            if (fogd > 90) fogd = 0;
        }
    }
    if (IsKeyPressed(KEY_P) && !menu && cdpause > 100) {
        menu = true;
        cdpause = 0;
    }
    if (IsKeyPressed(KEY_P) && menu && cdpause > 100) {
        menu = false;
        cdpause = 0;
    }
    cdpause++;
}

void desenhar() {
    BeginDrawing();
    if (menu==true) {
        DrawTexture(menui, 0, 0, WHITE);
        if (opt == 1) {
            DrawRectangle(430, 380, 50, 10, WHITE);
            DrawRectangle(630, 380, 50, 10, WHITE);
        }
        if (opt == 2) {
            DrawRectangle(410, 460, 50, 10, WHITE);
            DrawRectangle(670, 460, 50, 10, WHITE);
        }
        if (opt == 3) {
            DrawRectangle(450, 550, 50, 10, WHITE);
            DrawRectangle(630, 550, 50, 10, WHITE);
        }
        if (creds) {
            DrawTexture(credits, 0, 0, WHITE);
        }
    } else {
        ClearBackground(BLACK);
        
        // --- ATUALIZAÇÃO E DESENHO DO FUNDO DE ESTRELAS ---

    // Verifica se o modo turbo está ativo
    bool turbo_on = IsKeyDown(KEY_LEFT_SHIFT);

    // ATUALIZAÇÃO das camadas (exceto poeira estática)
    for (auto& s : background_stars) {
        if (turbo_on) { s.position.y += 30.0f / s.speed_delay; } 
        else { s.move_counter++; if(s.move_counter >= s.speed_delay){ s.position.y++; s.move_counter = 0; }}
        if(s.position.y > 710) { s.position = {(float)GetRandomValue(0, 1200), -10.f}; }
        s.anim_timer++;
        if (s.anim_timer >= s.anim_limit) {
            s.anim_timer = 0;
            if (s.growing) { if (s.size < s.base_size + s.growth_range) s.size++; else s.growing = false; } 
            else { if (s.size > s.base_size) s.size--; else s.growing = true; }
        }
    }
    for (auto& s : foreground_stars) {
        if (turbo_on) { s.position.y += 30.0f / s.speed_delay; } 
        else { s.move_counter++; if(s.move_counter >= s.speed_delay){ s.position.y++; s.move_counter = 0; }}
        if(s.position.y > 710) { s.position = {(float)GetRandomValue(0, 1200), -10.f}; }
        s.anim_timer++;
        if (s.anim_timer >= s.anim_limit) {
            s.anim_timer = 0;
            if (s.growing) { if (s.size < s.base_size + s.growth_range) s.size++; else s.growing = false; } 
            else { if (s.size > s.base_size) s.size--; else s.growing = true; }
        }
    }
    for(auto& n : nebulas){
        if (turbo_on) { n.position.y += 30.0f / n.speed_delay; } 
        else { n.move_counter++; if(n.move_counter >= n.speed_delay){ n.position.y++; n.move_counter = 0; }}
       if(n.position.y > 700 + 200) { 
            n.position = {(float)GetRandomValue(0, 1200), (float)GetRandomValue(-400, -200)};
            Color COLOR_ROXO = {128, 0, 128, 255};
            std::vector<Color> nebula_colors = {BLUE, COLOR_ROXO, RED, GREEN, YELLOW, WHITE};
            n.base_color = nebula_colors[GetRandomValue(0, nebula_colors.size() - 1)];
            GenerateNebulaShape(n, PARTICLES_PER_NEBULA, STAR_PARTICLE_COUNT);
        }
        for (int i=0; i < STAR_PARTICLE_COUNT; ++i){
            if (i >= n.particles.size()) break;
            auto& p = n.particles[i];
            p.star_timer++;
            if (p.star_timer >= p.star_limit) { p.star_growing = !p.star_growing; p.star_timer = 0; }
            if (p.star_growing) { p.star_size++; } else { if (p.star_size > 1) p.star_size--; }
        }
    }

    // DESENHO das camadas do fundo
    DrawTexture(static_dust_texture.texture, 0, 0, WHITE);
    
    for (const auto& s : background_stars) {
        DrawRectangle(s.position.x - s.size, s.position.y, s.size * 2 + 1, 1, {128, 128, 128, 255});
        DrawRectangle(s.position.x, s.position.y - s.size, 1, s.size * 2 + 1, {128, 128, 128, 255});
    }
    
    for (const auto& n : nebulas) {
        for (int i=0; i < n.particles.size(); ++i){
            const auto& p = n.particles[i];
            Vector2 particle_pos = {n.position.x + p.offset.x, n.position.y + p.offset.y};
            if (i < STAR_PARTICLE_COUNT) { DrawStarShape(particle_pos, p.star_size, p.color); } 
            else { DrawEllipse(particle_pos.x, particle_pos.y, p.width/2, p.height/2, p.color); }
        }
    }
    
    for (const auto& s : foreground_stars) { DrawStarShape(s.position, s.size, WHITE); }
    // --- FIM DO CÓDIGO DO FUNDO ---

        DrawText("Points: ", 520, 50, 20, WHITE);
        DrawText("Move: WASD/ARROW KEYS", 20, 30, 20, WHITE);
        DrawText("Shoot: ESPACE", 20, 50, 20, WHITE);
        DrawText("Pause: P", 20, 70, 20, WHITE);
        if (projetil == 1 && vida > 0) {
            DrawEllipse((float)projetil_x, (float)projetil_y, 1.5f, 10.0f, YELLOW);
            projetil_y -= tvel;
        }
        if (vida > 0) {
            DrawTexture(nav, nave_x, nave_y, WHITE);
        }
        if ((foga > 0 && foga <= 30 && IsKeyDown(KEY_A) && vida > 0) ||
            (fogd > 0 && fogd <= 30 && IsKeyDown(KEY_D) && vida > 0) ||
            (fogd > 0 && fogd <= 30 && IsKeyDown(KEY_W) && vida > 0) ||
            (fogd > 0 && fogd <= 30 && IsKeyDown(KEY_S) && vida > 0)) {
            Rectangle sourceRec = {0, 0, 18, 23};
            Vector2 position = {static_cast<float>(fire_x), static_cast<float>(fire_y)};
            DrawTextureRec(fire1, sourceRec, position, WHITE);
        }
        if ((foga > 30 && foga <= 60 && IsKeyDown(KEY_A) && vida > 0) ||
            (fogd > 30 && fogd <= 60 && IsKeyDown(KEY_D) && vida > 0) ||
            (fogd > 0 && fogd <= 30 && IsKeyDown(KEY_W) && vida > 0) ||
            (fogd > 0 && fogd <= 30 && IsKeyDown(KEY_S) && vida > 0)) {
            Rectangle sourceRec = {19, 0, 16, 23};
            Vector2 position = {static_cast<float>(fire_x), static_cast<float>(fire_y)};
            DrawTextureRec(fire1, sourceRec, position, WHITE);
        }
        if ((foga > 60 && foga <= 90 && IsKeyDown(KEY_A) && vida > 0) ||
            (fogd > 60 && fogd <= 90 && IsKeyDown(KEY_D) && vida > 0) ||
            (fogd > 0 && fogd <= 30 && IsKeyDown(KEY_W) && vida > 0) ||
            (fogd > 0 && fogd <= 30 && IsKeyDown(KEY_S) && vida > 0)) {
            Rectangle sourceRec = {35, 0, 17, 23};
            Vector2 position = {static_cast<float>(fire_x), static_cast<float>(fire_y)};
            DrawTextureRec(fire1, sourceRec, position, WHITE);
        }
        if (projetil_x >= 1200 || projetil_y >= 600 || projetil_x < 0 || projetil_y < 0) {
            projetil = 0;
            projetil_x = nave_x + 48;
            projetil_y = nave_y;
        }
        if (projetil == 0) {
            projetil_x = nave_x + 48;
            projetil_y = nave_y;
        }
        char pontos_str[10];
        sprintf(pontos_str, "%d       ", pontos);
        DrawText(pontos_str, 600, 50, 20, WHITE);
        if (meter && !winn) {
            DrawTexture(meteoro_tex, meteoro_x, meteoro_y, WHITE);
            bot_y += fall;
            meteoro_y = bot_y;
            meteoro_x = bot_x;
        }
        if (bot_y >= 750 && vida > 0) {
            bot_x = GetRandomValue(30, 1150);
            bot_y = GetRandomValue(-800, -10);
            fall = GetRandomValue(vmin, vmax);
            pontos--;
        }
        if (nave_y > bot_y && projetil_y <= bot_y && projetil_x >= bot_x && projetil_x <= bot_x + 60 && vida > 0) {
            PlaySound(bomb);
            pontos++;
            meter = false;
            bump = 0;
            projetil = 0;
            projetil_x = nave_x + 48;
            projetil_y = nave_y;
            expm_x = bot_x;
            expm_y = bot_y;
        }
        if (!meter && !winn) {
            wait++;
            bump++;
            if (bump > 0 && bump <= 5) {
                Rectangle sourceRec = {3, 80, 30, 30};
                Vector2 position = {static_cast<float>(expm_x + 20), static_cast<float>(expm_y + 25)};
                DrawTextureRec(explo, sourceRec, position, WHITE);
            }
            if (bump > 5 && bump <= 10) {
                Rectangle sourceRec = {48, 57, 64, 72};
                Vector2 position = {static_cast<float>(expm_x), static_cast<float>(expm_y)};
                DrawTextureRec(explo, sourceRec, position, WHITE);
            }
            if (bump > 10 && bump <= 15) {
                Rectangle sourceRec = {123, 39, 104, 130};
                Vector2 position = {static_cast<float>(expm_x - 20), static_cast<float>(expm_y - 10)};
                DrawTextureRec(explo, sourceRec, position, WHITE);
            }
            if (bump > 15 && bump <= 20) {
                Rectangle sourceRec = {231, 33, 145, 145};
                Vector2 position = {static_cast<float>(expm_x - 40), static_cast<float>(expm_y - 10)};
                DrawTextureRec(explo, sourceRec, position, WHITE);
            }
            if (bump > 20 && bump <= 25) {
                Rectangle sourceRec = {378, 0, 135, 180};
                Vector2 position = {static_cast<float>(expm_x - 40), static_cast<float>(expm_y - 55)};
                DrawTextureRec(explo, sourceRec, position, WHITE);
            }
            if (bump > 25 && bump <= 30) {
                Rectangle sourceRec = {525, 0, 155, 180};
                Vector2 position = {static_cast<float>(expm_x - 40), static_cast<float>(expm_y - 55)};
                DrawTextureRec(explo, sourceRec, position, WHITE);
            }
            if (wait > 30) {
                meter = true;
                wait = 0;
                bump = 0;
                bot_y = GetRandomValue(-800, -10);
                bot_x = GetRandomValue(30, 1150);
                fall = GetRandomValue(vmin, vmax);
            }
            if (pontos >= 3 && pontos <= 6) {
                vmin = 2;
                vmax = 5;
            }
            if (pontos >= 7 && pontos <= 9) {
                vmin = 3;
                vmax = 6;
            }
            if (boss) {
                vmax = 7;
            }
        }
        if (nave_x > 1115) {
            nave_x = 1115;
            exp_x = nave_x;
        }
        if (nave_x < -10) {
            nave_x = -10;
            exp_x = nave_x;
        }
        if (nave_y > 600) {
            nave_y = 600;
            exp_y = nave_y;
        }
        if (nave_y < 300) {
            nave_y = 300;
            exp_y = nave_y;
        }
        if (pontos > 9 && !winn) {
            boss = true;
        }
        if (boss && !winn) {
            DrawEllipse((float)tirobossx, (float)tirobossy, 2.5f, 12.5f, RED);
            DrawTexture(iboss, bossx, bossy, WHITE);
            if (bossy < 100) {
                bossy++;
            }
            char vidaboss_str[10];
            sprintf(vidaboss_str, "%d", vidaboss);
            DrawText(vidaboss_str, bossx + 90, bossy, 20, WHITE);
            if (vidaboss >= 40) {
                tirobossy += 2;
                v = 1;
            } else if (vidaboss >= 30) {
                tirobossy += 3;
            } else if (vidaboss >= 20) {
                tirobossy += 4;
                v = 2;
            } else if (vidaboss >= 10) {
                tirobossy += 5;
            } else {
                tirobossy += 6;
                v = 3;
            }
            if (tirobossy > 700) {
                tirobossx = GetRandomValue(bossx + 10, bossx + 90);
                tirobossy = bossy + 120;
                PlaySound(oh);
            }
            t -= v;
            if (d == 1 && t > 0) {
                bossx += v;
            } else if (d == 2 && t > 0) {
                bossx -= v;
            }
            if (bossx + 200 >= 1200) {
                d = 2;
                t = GetRandomValue(30, 300);
            }
            if (bossx <= 0) {
                d = 1;
                t = GetRandomValue(30, 300);
            }
            if (t < 1) {
                t = GetRandomValue(30, 300);
                d = GetRandomValue(1, 2);
            }
        }
        if (projetil_x >= bossx && projetil_x <= bossx + 200 && projetil_y <= bossy + 150 && boss) {
            vidaboss--;
            projetil = 0;
            projetil_x = nave_x + 48;
            projetil_y = nave_y;
            sob_index = (sob_index + 1) % 5;
        }
        if (vidaboss < 1) {
            if (boss_bomb_count < 5 && boss_death_sound_delay == 0) {
                PlaySound(bomb);
                boss_bomb_count++;
                boss_death_sound_delay = 30; // 166ms a 60 FPS
            }
            if (bump < 100) {
                bump++;
            }
            if (bump > 0 && bump <= 10) {
                Rectangle sourceRec = {3, 80, 30, 30};
                Vector2 pos1 = {static_cast<float>(bossx), static_cast<float>(bossy)};
                Vector2 pos2 = {static_cast<float>(bossx + 30), static_cast<float>(bossy + 10)};
                Vector2 pos3 = {static_cast<float>(bossx + 50), static_cast<float>(bossy + 30)};
                Vector2 pos4 = {static_cast<float>(bossx + 90), static_cast<float>(bossy - 10)};
                Vector2 pos5 = {static_cast<float>(bossx - 10), static_cast<float>(bossy + 50)};
                Vector2 pos6 = {static_cast<float>(bossx + 140), static_cast<float>(bossy + 90)};
                Vector2 pos7 = {static_cast<float>(bossx + 70), static_cast<float>(bossy)};
                Vector2 pos8 = {static_cast<float>(bossx + 60), static_cast<float>(bossy + 70)};
                DrawTextureRec(explo, sourceRec, pos1, WHITE);
                DrawTextureRec(explo, sourceRec, pos2, WHITE);
                DrawTextureRec(explo, sourceRec, pos3, WHITE);
                DrawTextureRec(explo, sourceRec, pos4, WHITE);
                DrawTextureRec(explo, sourceRec, pos5, WHITE);
                DrawTextureRec(explo, sourceRec, pos6, WHITE);
                DrawTextureRec(explo, sourceRec, pos7, WHITE);
                DrawTextureRec(explo, sourceRec, pos8, WHITE);
            }
            if (bump > 10 && bump <= 20) {
                Rectangle sourceRec = {48, 57, 64, 72};
                Vector2 pos1 = {static_cast<float>(bossx), static_cast<float>(bossy)};
                Vector2 pos2 = {static_cast<float>(bossx + 30), static_cast<float>(bossy + 10)};
                Vector2 pos3 = {static_cast<float>(bossx + 50), static_cast<float>(bossy + 30)};
                Vector2 pos4 = {static_cast<float>(bossx + 90), static_cast<float>(bossy - 10)};
                Vector2 pos5 = {static_cast<float>(bossx - 10), static_cast<float>(bossy + 50)};
                Vector2 pos6 = {static_cast<float>(bossx + 140), static_cast<float>(bossy + 90)};
                Vector2 pos7 = {static_cast<float>(bossx + 70), static_cast<float>(bossy)};
                Vector2 pos8 = {static_cast<float>(bossx + 60), static_cast<float>(bossy + 70)};
                DrawTextureRec(explo, sourceRec, pos1, WHITE);
                DrawTextureRec(explo, sourceRec, pos2, WHITE);
                DrawTextureRec(explo, sourceRec, pos3, WHITE);
                DrawTextureRec(explo, sourceRec, pos4, WHITE);
                DrawTextureRec(explo, sourceRec, pos5, WHITE);
                DrawTextureRec(explo, sourceRec, pos6, WHITE);
                DrawTextureRec(explo, sourceRec, pos7, WHITE);
                DrawTextureRec(explo, sourceRec, pos8, WHITE);
            }
            if (bump > 20 && bump <= 30) {
                Rectangle sourceRec = {123, 39, 104, 130};
                Vector2 pos1 = {static_cast<float>(bossx), static_cast<float>(bossy)};
                Vector2 pos2 = {static_cast<float>(bossx + 30), static_cast<float>(bossy + 10)};
                Vector2 pos3 = {static_cast<float>(bossx + 50), static_cast<float>(bossy + 30)};
                Vector2 pos4 = {static_cast<float>(bossx + 90), static_cast<float>(bossy - 10)};
                Vector2 pos5 = {static_cast<float>(bossx - 10), static_cast<float>(bossy + 50)};
                Vector2 pos6 = {static_cast<float>(bossx + 140), static_cast<float>(bossy + 90)};
                Vector2 pos7 = {static_cast<float>(bossx + 70), static_cast<float>(bossy)};
                Vector2 pos8 = {static_cast<float>(bossx + 60), static_cast<float>(bossy + 70)};
                DrawTextureRec(explo, sourceRec, pos1, WHITE);
                DrawTextureRec(explo, sourceRec, pos2, WHITE);
                DrawTextureRec(explo, sourceRec, pos3, WHITE);
                DrawTextureRec(explo, sourceRec, pos4, WHITE);
                DrawTextureRec(explo, sourceRec, pos5, WHITE);
                DrawTextureRec(explo, sourceRec, pos6, WHITE);
                DrawTextureRec(explo, sourceRec, pos7, WHITE);
                DrawTextureRec(explo, sourceRec, pos8, WHITE);
            }
            if (bump > 30 && bump <= 40) {
                Rectangle sourceRec = {231, 33, 145, 145};
                Vector2 pos1 = {static_cast<float>(bossx), static_cast<float>(bossy)};
                Vector2 pos2 = {static_cast<float>(bossx + 30), static_cast<float>(bossy + 10)};
                Vector2 pos3 = {static_cast<float>(bossx + 50), static_cast<float>(bossy + 30)};
                Vector2 pos4 = {static_cast<float>(bossx + 90), static_cast<float>(bossy - 10)};
                Vector2 pos5 = {static_cast<float>(bossx - 10), static_cast<float>(bossy + 50)};
                Vector2 pos6 = {static_cast<float>(bossx + 140), static_cast<float>(bossy + 90)};
                Vector2 pos7 = {static_cast<float>(bossx + 70), static_cast<float>(bossy)};
                Vector2 pos8 = {static_cast<float>(bossx + 60), static_cast<float>(bossy + 70)};
                DrawTextureRec(explo, sourceRec, pos1, WHITE);
                DrawTextureRec(explo, sourceRec, pos2, WHITE);
                DrawTextureRec(explo, sourceRec, pos3, WHITE);
                DrawTextureRec(explo, sourceRec, pos4, WHITE);
                DrawTextureRec(explo, sourceRec, pos5, WHITE);
                DrawTextureRec(explo, sourceRec, pos6, WHITE);
                DrawTextureRec(explo, sourceRec, pos7, WHITE);
                DrawTextureRec(explo, sourceRec, pos8, WHITE);
            }
            if (bump > 40 && bump <= 50) {
                Rectangle sourceRec = {378, 0, 135, 180};
                Vector2 pos1 = {static_cast<float>(bossx), static_cast<float>(bossy)};
                Vector2 pos2 = {static_cast<float>(bossx + 30), static_cast<float>(bossy + 10)};
                Vector2 pos3 = {static_cast<float>(bossx + 50), static_cast<float>(bossy + 30)};
                Vector2 pos4 = {static_cast<float>(bossx + 90), static_cast<float>(bossy - 10)};
                Vector2 pos5 = {static_cast<float>(bossx - 10), static_cast<float>(bossy + 50)};
                Vector2 pos6 = {static_cast<float>(bossx + 140), static_cast<float>(bossy + 90)};
                Vector2 pos7 = {static_cast<float>(bossx + 70), static_cast<float>(bossy)};
                Vector2 pos8 = {static_cast<float>(bossx + 60), static_cast<float>(bossy + 70)};
                DrawTextureRec(explo, sourceRec, pos1, WHITE);
                DrawTextureRec(explo, sourceRec, pos2, WHITE);
                DrawTextureRec(explo, sourceRec, pos3, WHITE);
                DrawTextureRec(explo, sourceRec, pos4, WHITE);
                DrawTextureRec(explo, sourceRec, pos5, WHITE);
                DrawTextureRec(explo, sourceRec, pos6, WHITE);
                DrawTextureRec(explo, sourceRec, pos7, WHITE);
                DrawTextureRec(explo, sourceRec, pos8, WHITE);
            }
            if (bump > 50 && bump <= 60) {
                Rectangle sourceRec = {525, 0, 155, 180};
                Vector2 pos1 = {static_cast<float>(bossx), static_cast<float>(bossy)};
                Vector2 pos2 = {static_cast<float>(bossx + 30), static_cast<float>(bossy + 10)};
                Vector2 pos3 = {static_cast<float>(bossx + 50), static_cast<float>(bossy + 30)};
                Vector2 pos4 = {static_cast<float>(bossx + 90), static_cast<float>(bossy - 10)};
                Vector2 pos5 = {static_cast<float>(bossx - 10), static_cast<float>(bossy + 50)};
                Vector2 pos6 = {static_cast<float>(bossx + 140), static_cast<float>(bossy + 90)};
                Vector2 pos7 = {static_cast<float>(bossx + 70), static_cast<float>(bossy)};
                Vector2 pos8 = {static_cast<float>(bossx + 60), static_cast<float>(bossy + 70)};
                DrawTextureRec(explo, sourceRec, pos1, WHITE);
                DrawTextureRec(explo, sourceRec, pos2, WHITE);
                DrawTextureRec(explo, sourceRec, pos3, WHITE);
                DrawTextureRec(explo, sourceRec, pos4, WHITE);
                DrawTextureRec(explo, sourceRec, pos5, WHITE);
                DrawTextureRec(explo, sourceRec, pos6, WHITE);
                DrawTextureRec(explo, sourceRec, pos7, WHITE);
                DrawTextureRec(explo, sourceRec, pos8, WHITE);
            }
            if(bump>=60){
                bossy = -220;
                tirobossy = bossy + 120;
            }
            boss = false;
            winn = true;
        }
        if (boss_death_sound_delay > 0) {
            boss_death_sound_delay--;
            if (boss_death_sound_delay == 0 && boss_bomb_count < 5) {
                PlaySound(bomb);
                boss_bomb_count++;
                boss_death_sound_delay = 20; // Reinicia o atraso
            }
        }
        if (winn) {
            pisc++;
            DrawTexture(win_tex, 100, 100, WHITE);
            if (pisc < 100) {
                DrawText("APERTE R PARA JOGAR DE NOVO", 440, 480, 20, WHITE);
            }
            if (pisc >= 200) {
                pisc = 0;
            }
            if (IsKeyPressed(KEY_R)) {
                winn = false;
                vida = 1;
                pontos = 1;
                bum = 0;
                boss = false;
                vidaboss = 50;
                tvel++;
                velo++;
                ng++;
            }
        }
        if (ng > 0) {
            char ng_str[20];
            sprintf(ng_str, "NewGame+ %d", ng);
            DrawText(ng_str, 1000, 50, 20, WHITE);
        }
        if ((bot_y >= death_y - 30 && bot_y <= death_y + 50 && bot_x >= death_x - 50 && bot_x <= death_x + 60) ||
            (tirobossy >= death_y+20 && tirobossy <= death_y+80 && tirobossx >= death_x+20 && tirobossx <= death_x + 70)) {
            if (death_sound_delay == 0) {
                PlaySound(bomb);
                death_sound_delay = 20; // Atraso de 10 frames
            }
            vida = 0;
        }
        if (death_sound_delay > 0) {
            death_sound_delay--;
            if (death_sound_delay == 0 && vida == 0) {
                PlaySound(ai);
            }
        }
        if (vida < 1) {
            bum++;
            pisc++;
            if (bum < 100) {
                if (bum > 0 && bum <= 5) {
                    Rectangle sourceRec = {3, 80, 30, 30};
                    Vector2 position = {static_cast<float>(exp_x + 30), static_cast<float>(exp_y + 25)};
                    DrawTextureRec(explo, sourceRec, position, WHITE);
                }
                if (bum > 5 && bum <= 10) {
                    Rectangle sourceRec = {48, 57, 64, 72};
                    Vector2 position = {static_cast<float>(exp_x + 10), static_cast<float>(exp_y)};
                    DrawTextureRec(explo, sourceRec, position, WHITE);
                }
                if (bum > 10 && bum <= 15) {
                    Rectangle sourceRec = {123, 39, 104, 130};
                    Vector2 position = {static_cast<float>(exp_x - 10), static_cast<float>(exp_y - 10)};
                    DrawTextureRec(explo, sourceRec, position, WHITE);
                }
                if (bum > 15 && bum <= 20) {
                    Rectangle sourceRec = {231, 33, 145, 145};
                    Vector2 position = {static_cast<float>(exp_x - 30), static_cast<float>(exp_y - 10)};
                    DrawTextureRec(explo, sourceRec, position, WHITE);
                }
                if (bum > 20 && bum <= 25) {
                    Rectangle sourceRec = {378, 0, 135, 180};
                    Vector2 position = {static_cast<float>(exp_x - 30), static_cast<float>(exp_y - 25)};
                    DrawTextureRec(explo, sourceRec, position, WHITE);
                }
                if (bum > 25 && bum <= 30) {
                    Rectangle sourceRec = {525, 38, 155, 133};
                    Vector2 position = {static_cast<float>(exp_x - 30), static_cast<float>(exp_y - 15)};
                    DrawTextureRec(explo, sourceRec, position, WHITE);
                }
            }
            DrawTexture(mort, mort_x, mort_y, WHITE);
            if (pisc < 100) {
                DrawText("APERTE R PARA TENTAR DE NOVO", 470, 420, 20, RED);
            }
            if (pisc >= 200) {
                pisc = 0;
            }
            if (IsKeyPressed(KEY_R)) {
                winn = false;
                vida = 1;
                pontos = 1;
                bum = 0;
                vidaboss = 50;
                bossy = -220;
            }
        }
    }
    EndDrawing();
}

int main() {
    // --- INICIALIZAÇÃO DA JANELA ---
    const int screenWidth = 1200;
    const int screenHeight = 700;
    InitWindow(screenWidth, screenHeight, "SPACE PLAGYL");
    InitAudioDevice();
    SetTargetFPS(120);

    // Variáveis para a seleção de densidade
    std::string density_choice = "";
    bool density_selected = false;

    // --- LOOP PRÉ-JOGO: TELA DE SELEÇÃO DE DENSIDADE ---
    while (!density_selected && !WindowShouldClose()) {
        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
            density_choice = "baixa";
            density_selected = true;
        }
        if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
            density_choice = "media";
            density_selected = true;
        }
        if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) {
            density_choice = "alta";
            density_selected = true;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("ESCOLHA A DENSIDADE DO CENARIO", 270, 250, 40, WHITE);
        DrawText("1 - BAIXA (Melhor Performance)", 400, 350, 20, GRAY);
        DrawText("2 - MEDIA (Equilibrado)", 400, 380, 20, GRAY);
        DrawText("3 - ALTA (Melhor Visual)", 400, 410, 20, GRAY);
        EndDrawing();
    }

    // Se o usuário fechou a janela na tela de seleção, sai do jogo
    if (WindowShouldClose()) {
        CloseAudioDevice();
        CloseWindow();
        return 0;
    }

    // --- CONFIGURAÇÃO DAS DENSIDADES (baseado na escolha) ---
    if (density_choice == "baixa") {
        // Estrelas em densidade Média, Nebulosas Desligadas
        DUST_COUNT = 500; BG_STAR_COUNT = 100; FG_STAR_COUNT = 50;
        NEBULA_COUNT = 0; PARTICLES_PER_NEBULA = 0; STAR_PARTICLE_COUNT = 0;
    } else if (density_choice == "alta") {
        // Estrelas em densidade Alta, Nebulosas em densidade Média
        DUST_COUNT = 800; BG_STAR_COUNT = 200; FG_STAR_COUNT = 100;
        NEBULA_COUNT = 10; PARTICLES_PER_NEBULA = 250; STAR_PARTICLE_COUNT = 10;
    } else { // Padrão: Média
        // Estrelas em densidade Média, Nebulosas em densidade Baixa
        DUST_COUNT = 500; BG_STAR_COUNT = 100; FG_STAR_COUNT = 50;
        NEBULA_COUNT = 5; PARTICLES_PER_NEBULA = 100; STAR_PARTICLE_COUNT = 10;
    }

    // --- REDIMENSIONAMENTO E INICIALIZAÇÃO DOS VETORES ---
    dust.resize(DUST_COUNT);
    background_stars.resize(BG_STAR_COUNT);
    foreground_stars.resize(FG_STAR_COUNT);
    nebulas.resize(NEBULA_COUNT);

    // --- CARREGAMENTO DE ASSETS ---
    srand(static_cast<unsigned>(time(0)));
    background_music = LoadMusicStream("C:/Users/giova/Desktop/Projetos/Space Plagyl Soundtrack.mp3");
    SetMusicVolume(background_music, 1.0f);
    PlayMusicStream(background_music);
    menui = LoadTexture("SPACE PLAGYL.png");
    credits = LoadTexture("CREDITS.png");
    sob[0] = LoadSound("sobio1.mp3"); sob[1] = LoadSound("sobio2.mp3"); sob[2] = LoadSound("sobio3.mp3"); sob[3] = LoadSound("sobio4.mp3"); sob[4] = LoadSound("sobio5.mp3");
    bomb = LoadSound("bomb.mp3"); ai = LoadSound("ai.mp3"); oh = LoadSound("tiroboss.wav");
    nav = LoadTexture("navee2.png"); win_tex = LoadTexture("winner.png"); mort = LoadTexture("mort.png");
    explo = LoadTexture("esplosoes.png"); fire1 = LoadTexture("fogos.png"); meteoro_tex = LoadTexture("meteoro.png"); iboss = LoadTexture("boss.png");

    // --- INICIALIZAÇÃO DAS VARIÁVEIS ---
    fall = GetRandomValue(1, 4); exp_x = nave_x; exp_y = nave_y; death_x = nave_x + 25; death_y = nave_y + 10; fire_x = nave_x + 30;
    fire_y = nave_y + 84; projetil_x = nave_x + 48; projetil_y = nave_y; sol_x = GetRandomValue(10, 1100); sol_y = GetRandomValue(10, 400);
    bot_x = GetRandomValue(50, 1100); bot_y = -10; expm_x = bot_x + 200; expm_y = bot_y + 100; meteoro_x = bot_x; meteoro_y = bot_y;
    hitbox1x = nave_x + 10; hitbox1y = nave_y + 40; hitbox2x = nave_x + 38; hitbox2y = nave_y + 10;
    tirobossx = GetRandomValue(bossx + 10, bossx + 90); tirobossy = bossy + 50; d = GetRandomValue(1, 2); t = GetRandomValue(30, 500);

    // --- INICIALIZAÇÃO DO FUNDO DE ESTRELAS ---
    Color COLOR_ROXO = {128, 0, 128, 255}; Color COLOR_CINZA_ESCURO = {80, 80, 80, 255}; Color COLOR_CINZA = {128, 128, 128, 255};
    std::vector<Color> nebula_colors = {BLUE, COLOR_ROXO, RED, GREEN, YELLOW, WHITE};
    static_dust_texture = LoadRenderTexture(screenWidth, screenHeight);
    BeginTextureMode(static_dust_texture);
    ClearBackground(BLANK);
    for (int i = 0; i < DUST_COUNT; ++i) {
        dust[i].position = {(float)GetRandomValue(0, screenWidth), (float)GetRandomValue(0, screenHeight)};
        dust[i].color = (GetRandomValue(0,1) == 0) ? COLOR_CINZA : COLOR_CINZA_ESCURO;
        DrawPixelV(dust[i].position, dust[i].color);
    }
    EndTextureMode();
    for (int i = 0; i < BG_STAR_COUNT; ++i) { background_stars[i].position = {(float)GetRandomValue(0, 1200), (float)GetRandomValue(0, 700)};
        background_stars[i].speed_delay = GetRandomValue(8, 20);
        background_stars[i].base_size = GetRandomValue(1, 2);
        background_stars[i].size = background_stars[i].base_size;
        background_stars[i].move_counter = GetRandomValue(0, background_stars[i].speed_delay);
        background_stars[i].growing = (GetRandomValue(0, 1) == 1);
        background_stars[i].anim_limit = GetRandomValue(5, 15);
        background_stars[i].anim_timer = 0;
        background_stars[i].growth_range = 3; }
    for (int i = 0; i < FG_STAR_COUNT; ++i) {  foreground_stars[i].position = {(float)GetRandomValue(0, 1200), (float)GetRandomValue(0, 700)};
        foreground_stars[i].speed_delay = GetRandomValue(1, 7);
        foreground_stars[i].base_size = (12 - foreground_stars[i].speed_delay) / 2;
        if(foreground_stars[i].base_size < 3) foreground_stars[i].base_size = 3;
        foreground_stars[i].size = foreground_stars[i].base_size;
        foreground_stars[i].move_counter = GetRandomValue(0, foreground_stars[i].speed_delay);
        foreground_stars[i].growing = (GetRandomValue(0, 1) == 1);
        foreground_stars[i].anim_limit = GetRandomValue(1, 6);
        foreground_stars[i].anim_timer = 0;
        foreground_stars[i].growth_range = (foreground_stars[i].base_size > 5) ? 9 : 6; }
    for (int i = 0; i < NEBULA_COUNT; ++i) { /* colocar a inicialização das nebulosas */ }

    // --- LOOP PRINCIPAL ---
    while (!WindowShouldClose() && !quit) {
        controle();
        UpdateMusicStream(background_music);
        desenhar();
    }

    // --- LIMPEZA FINAL ---
    UnloadRenderTexture(static_dust_texture);
    UnloadTexture(menui); UnloadTexture(credits); UnloadTexture(nav); UnloadTexture(win_tex); UnloadTexture(mort);
    UnloadTexture(explo); UnloadTexture(fire1); UnloadTexture(meteoro_tex); UnloadTexture(iboss);
    UnloadMusicStream(background_music);
    for (int i = 0; i < 5; i++) UnloadSound(sob[i]);
    UnloadSound(bomb); UnloadSound(ai); UnloadSound(oh);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}