#include "Background.h"

Background::Background() {
    // Valores padrão
    dustCount = 500;
    bgStarCount = 100;
    fgStarCount = 50;
    nebulaCount = 5;
}

Background::~Background() {
    UnloadRenderTexture(staticDustTexture);
}

void Background::DrawStarShape(Vector2 center, float size, Color color) {
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

void Background::GenerateNebulaShape(Nebula& nebula, int particleCount, int starParticleCount) {
    nebula.particles.clear();
    nebula.particles.resize(particleCount);

    int particles_drawn = 0;
    int num_arms = 3;
    int particles_per_arm = particleCount / num_arms;
    if (particles_per_arm == 0) particles_per_arm = 1;

    for (int b = 0; b < num_arms; ++b) {
        Vector2 walker = {0, 0};
        Vector2 direction = {0, 0};
        int steps_left = 0;

        for (int p = 0; p < particles_per_arm; ++p) {
            if (particles_drawn >= particleCount) break;

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
            
            if (idx >= starParticleCount) {
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

void Background::Init(std::string densityLevel) {
    int particlesPerNebula = 100;
    int starParticleCount = 10;

    if (densityLevel == "baixa") {
        dustCount = 500; bgStarCount = 100; fgStarCount = 50;
        nebulaCount = 0; particlesPerNebula = 0; starParticleCount = 0;
    } else if (densityLevel == "alta") {
        dustCount = 800; bgStarCount = 200; fgStarCount = 100;
        nebulaCount = 10; particlesPerNebula = 250; starParticleCount = 10;
    } else { // Media
        dustCount = 500; bgStarCount = 100; fgStarCount = 50;
        nebulaCount = 5; particlesPerNebula = 100; starParticleCount = 10;
    }

    dust.resize(dustCount);
    bgStars.resize(bgStarCount);
    fgStars.resize(fgStarCount);
    nebulas.resize(nebulaCount);

    // Inicializa poeira estática (Textura)
    Color COLOR_CINZA_ESCURO = {80, 80, 80, 255}; 
    Color COLOR_CINZA = {128, 128, 128, 255};
    staticDustTexture = LoadRenderTexture(1200, 700); 
    
    BeginTextureMode(staticDustTexture);
    ClearBackground(BLANK);
    for (int i = 0; i < dustCount; ++i) {
        Vector2 pos = {(float)GetRandomValue(0, 1200), (float)GetRandomValue(0, 700)};
        Color col = (GetRandomValue(0,1) == 0) ? COLOR_CINZA : COLOR_CINZA_ESCURO;
        DrawPixelV(pos, col);
    }
    EndTextureMode();

    // Inicializa Background Stars
    for (auto& s : bgStars) {
        s.position = {(float)GetRandomValue(0, 1200), (float)GetRandomValue(0, 700)};
        s.speed_delay = GetRandomValue(8, 20);
        s.base_size = GetRandomValue(1, 2);
        s.size = s.base_size;
        s.move_counter = GetRandomValue(0, s.speed_delay);
        s.growing = (GetRandomValue(0, 1) == 1);
        s.anim_limit = GetRandomValue(5, 15);
        s.anim_timer = 0;
        s.growth_range = 3;
    }

    // Inicializa Foreground Stars
    for (auto& s : fgStars) {
        s.position = {(float)GetRandomValue(0, 1200), (float)GetRandomValue(0, 700)};
        s.speed_delay = GetRandomValue(1, 7);
        s.base_size = (12 - s.speed_delay) / 2;
        if(s.base_size < 3) s.base_size = 3;
        s.size = s.base_size;
        s.move_counter = GetRandomValue(0, s.speed_delay);
        s.growing = (GetRandomValue(0, 1) == 1);
        s.anim_limit = GetRandomValue(1, 6);
        s.anim_timer = 0;
        s.growth_range = (s.base_size > 5) ? 9 : 6;
    }

    // Inicializa Nebulosas
    Color COLOR_ROXO = {128, 0, 128, 255};
    std::vector<Color> nebula_colors = {BLUE, COLOR_ROXO, RED, GREEN, YELLOW, WHITE};
    
    for (auto& n : nebulas) {
        n.position = {(float)GetRandomValue(0, 1200), (float)GetRandomValue(-400, 600)};
        n.base_color = nebula_colors[GetRandomValue(0, nebula_colors.size() - 1)];
        n.speed_delay = GetRandomValue(10, 20); // Da pra ajustar deps
        GenerateNebulaShape(n, particlesPerNebula, starParticleCount);
    }
}

void Background::Update(bool turboMode) {
    // Atualiza Background Stars
    for (auto& s : bgStars) {
        if (turboMode) { 
            s.position.y += 30.0f / s.speed_delay; 
        } else { 
            s.move_counter++; 
            if(s.move_counter >= s.speed_delay){ 
                s.position.y++; 
                s.move_counter = 0; 
            }
        }
        
        if(s.position.y > 710) { 
            s.position = {(float)GetRandomValue(0, 1200), -10.f}; 
        }
        
        s.anim_timer++;
        if (s.anim_timer >= s.anim_limit) {
            s.anim_timer = 0;
            if (s.growing) { 
                if (s.size < s.base_size + s.growth_range) s.size++; 
                else s.growing = false; 
            } else { 
                if (s.size > s.base_size) s.size--; 
                else s.growing = true; 
            }
        }
    }

    // Atualiza Foreground Stars
    for (auto& s : fgStars) {
        if (turboMode) { 
            s.position.y += 30.0f / s.speed_delay; 
        } else { 
            s.move_counter++; 
            if(s.move_counter >= s.speed_delay){ 
                s.position.y++; 
                s.move_counter = 0; 
            }
        }
        
        if(s.position.y > 710) { 
            s.position = {(float)GetRandomValue(0, 1200), -10.f}; 
        }
        
        s.anim_timer++;
        if (s.anim_timer >= s.anim_limit) {
            s.anim_timer = 0;
            if (s.growing) { 
                if (s.size < s.base_size + s.growth_range) s.size++; 
                else s.growing = false; 
            } else { 
                if (s.size > s.base_size) s.size--; 
                else s.growing = true; 
            }
        }
    }

    // Atualiza Nebulosas
    int particlesPerNebula = (nebulas.empty() || nebulas[0].particles.empty()) ? 0 : nebulas[0].particles.size();
    // Assumindo um starParticleCount fixo ou detectado
    int starParticleCount = 10; 

    for(auto& n : nebulas){
        if (turboMode) { 
            n.position.y += 30.0f / n.speed_delay; 
        } else { 
            n.move_counter++; 
            if(n.move_counter >= n.speed_delay){ 
                n.position.y++; 
                n.move_counter = 0; 
            }
        }

        if(n.position.y > 900) { 
            n.position = {(float)GetRandomValue(0, 1200), (float)GetRandomValue(-400, -200)};
            Color COLOR_ROXO = {128, 0, 128, 255};
            std::vector<Color> nebula_colors = {BLUE, COLOR_ROXO, RED, GREEN, YELLOW, WHITE};
            n.base_color = nebula_colors[GetRandomValue(0, nebula_colors.size() - 1)];
            GenerateNebulaShape(n, particlesPerNebula, starParticleCount);
        }

        for (int i=0; i < starParticleCount; ++i){
            if (i >= n.particles.size()) break;
            auto& p = n.particles[i];
            p.star_timer++;
            if (p.star_timer >= p.star_limit) { 
                p.star_growing = !p.star_growing; 
                p.star_timer = 0; 
            }
            if (p.star_growing) { p.star_size++; } 
            else { if (p.star_size > 1) p.star_size--; }
        }
    }
}

void Background::Draw() {
    DrawTexture(staticDustTexture.texture, 0, 0, WHITE);
    
    for (const auto& s : bgStars) {
        DrawRectangle(s.position.x - s.size, s.position.y, s.size * 2 + 1, 1, {128, 128, 128, 255});
        DrawRectangle(s.position.x, s.position.y - s.size, 1, s.size * 2 + 1, {128, 128, 128, 255});
    }
    
    // Desenha Nebulosas e suas estrelas
    int starParticleCount = 10; 

    for (const auto& n : nebulas) {
        for (int i=0; i < n.particles.size(); ++i){
            const auto& p = n.particles[i];
            Vector2 particle_pos = {n.position.x + p.offset.x, n.position.y + p.offset.y};
            
            if (i < starParticleCount) { 
                DrawStarShape(particle_pos, p.star_size, p.color); 
            } else { 
                DrawEllipse(particle_pos.x, particle_pos.y, p.width/2, p.height/2, p.color); 
            }
        }
    }
    
    for (const auto& s : fgStars) { 
        DrawStarShape(s.position, s.size, WHITE); 
    }
}