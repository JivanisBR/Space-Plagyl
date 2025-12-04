#include "Game.h"

int main() {
    // Instancia o jogo
    // O construtor chama InitWindow e InitAudioDevice
    Game game(1200, 700, "SPACE PLAGYL - Refactored");

    // Roda o loop principal
    game.Run();

    // O destrutor da classe Game fecha tudo
    return 0;
}