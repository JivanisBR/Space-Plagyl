#pragma once
#include "raylib.h"
#include <vector>

struct DustParticle { Vector2 position; Color color; };
struct Star { Vector2 position; int size; int speed_delay; int move_counter; bool growing; int anim_timer; int anim_limit; int base_size; int growth_range; };
struct NebulaParticle { Vector2 offset; float width; float height; Color color; int star_size; bool star_growing; int star_timer; int star_limit; };

struct Nebula {
    Vector2 position; Color base_color; int speed_delay; int move_counter;
    std::vector<NebulaParticle> particles;
};

class Background {
private:
    std::vector<DustParticle> dust;
    std::vector<Star> bgStars;
    std::vector<Star> fgStars;
    std::vector<Nebula> nebulas;
    RenderTexture2D staticDustTexture;
    
    // Configurações de densidade
    int dustCount, bgStarCount, fgStarCount, nebulaCount;

    // Métodos auxiliares privados
    void DrawStarShape(Vector2 center, float size, Color color);
    void GenerateNebulaShape(Nebula& nebula, int particleCount, int starParticleCount);

public:
    Background();
    ~Background();

    void Init(std::string densityLevel); // Baixa, Media, Alta
    void Update(bool turboMode);
    void Draw();
};