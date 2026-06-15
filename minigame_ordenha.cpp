#include "Minigame_Ordenha.h"

// =========================================================================
// CONSTRUTOR DA CLASSE
// =========================================================================
MinigameOrdenha::MinigameOrdenha() {
    ativo                  = false;
    barra                  = 0.0f;
    velocidade             = 2.5f; // Mantém a velocidade padrão definida no jogo
    subindo                = true;
    indexVacaSendoOrdenhada = 0;
}

// =========================================================================
// CONTROLE DE FLUXO E INICIALIZAÇÃO
// =========================================================================
void MinigameOrdenha::Iniciar(size_t indexVaca) {
    ativo                  = true;
    barra                  = 0.0f;
    subindo                = true;
    indexVacaSendoOrdenhada = indexVaca;
}

// =========================================================================
// CICLO DE VIDA DO JOGO (ATUALIZAÇÃO DA LÓGICA)
// =========================================================================
void MinigameOrdenha::Atualizar() {
    if (!ativo) return;

    // Lógica de oscilação da barra (Agulha vai e volta entre 0 e 100)
    if (subindo) {
        barra += velocidade;
        if (barra >= 100.0f) {
            barra   = 100.0f;
            subindo = false;
        }
    } else {
        barra -= velocidade;
        if (barra <= 0.0f) {
            barra   = 0.0f;
            subindo = true;
        }
    }
}

// =========================================================================
// PIPELINE GRÁFICO (RENDERIZAÇÃO DE INTERFACE)
// =========================================================================
void MinigameOrdenha::Desenhar(Vector2 posJogador) {
    if (!ativo) return;

    // Posição flutuante acima da cabeça do fazendeiro
    Vector2 posHUDVaca = { posJogador.x - 50, posJogador.y - 45 };

    // 1. Fundo da barra em cinza escuro com bordas pretas
    DrawRectangle(posHUDVaca.x, posHUDVaca.y, 100, 14, DARKGRAY);
    DrawRectangleLines(posHUDVaca.x, posHUDVaca.y, 100, 14, BLACK);

    // 2. Zona Perfeita Alvo (Dourada) entre os pixels 70 e 85
    DrawRectangle(posHUDVaca.x + 70, posHUDVaca.y + 1, 15, 12, GOLD);

    // 3. Agulha indicadora vermelha que oscila horizontalmente
    int marcadorX = (int)barra;
    DrawRectangle(posHUDVaca.x + marcadorX - 2, posHUDVaca.y - 3, 4, 20, RED);

    // 4. Feedback textual de comando para o jogador
    DrawText("RITMO [C]!", posHUDVaca.x + 15, posHUDVaca.y - 15, 10, ORANGE);
}