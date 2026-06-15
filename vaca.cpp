#include "vaca.h"
#include "tipos.h"
#include <cmath>

// =========================================================================
// CONSTRUTOR DA CLASSE
// =========================================================================
// Adaptado: 0.0f = Sem fome (Alimentada), 100.0f = Fome Máxima
Vaca::Vaca(float x, float y)
    : posicao({x, y}),
    velocidade(0.8f),
    raioColisao(22.0f),
    nivelFome(0.0f),
    nivelLeite(0.0f),
    tempoMudarDirecao(0),
    direcaoAtual({0, 0}) {}

// =========================================================================
// MODIFICADORES DE ESTADO (SETTERS)
// =========================================================================
void Vaca::SetPosicao(float x, float y) {
    posicao.x = x;
    posicao.y = y;
}

// =========================================================================
// MÉTODOS DE ACESSO (GETTERS - CONST)
// =========================================================================
Vector2 Vaca::GetPosicao() const     { return posicao; }
float   Vaca::GetRaioColisao() const { return raioColisao; }
float   Vaca::GetNivelLeite() const  { return nivelLeite; }
float   Vaca::GetNivelFome() const   { return nivelFome; }

// =========================================================================
// MÉTODOS DE REGRA DE NEGÓCIO (INTERAÇÕES)
// =========================================================================
void Vaca::Alimentar() {
    nivelFome = 0.0f; // Zera a fome (Vaca completamente alimentada)
}

float Vaca::Ordenhar() {
    float quantidadeColetada = nivelLeite;
    nivelLeite = 0.0f;
    return quantidadeColetada;
}

// =========================================================================
// CICLO DE ATUALIZAÇÃO (LÓGICA DO JOGO)
// =========================================================================
void Vaca::Atualizar(Estacao estacaoAtual) {
    // 1. Inteligência Artificial de movimentação aleatória por tempo
    tempoMudarDirecao--;
    if (tempoMudarDirecao <= 0) {
        direcaoAtual.x    = (float)GetRandomValue(-1, 1);
        direcaoAtual.y    = (float)GetRandomValue(-1, 1);
        tempoMudarDirecao = GetRandomValue(60, 180);
    }

    // Aplica o vetor de movimento
    posicao.x += direcaoAtual.x * velocidade;
    posicao.y += direcaoAtual.y * velocidade;

    // 2. SISTEMA DE COLISÃO COM AS CERCAS (Limites do Cenário)
    // Coordenadas baseadas nas cercas visuais: X [145 a 790], Y [65 a 595]
    if (posicao.x - raioColisao < 145.0f) {
        posicao.x = 145.0f + raioColisao;
        direcaoAtual.x *= -1; // Rebate horizontal
    }
    else if (posicao.x + raioColisao > 790.0f) {
        posicao.x = 790.0f - raioColisao;
        direcaoAtual.x *= -1;
    }

    if (posicao.y - raioColisao < 65.0f) {
        posicao.y = 65.0f + raioColisao;
        direcaoAtual.y *= -1; // Rebate vertical
    }
    else if (posicao.y + raioColisao > 595.0f) {
        posicao.y = 595.0f - raioColisao;
        direcaoAtual.y *= -1;
    }

    // 3. SISTEMA DE COLISÃO COM O LAGO CIRCULAR
    // Centro do Lago: (465, 325) | Raio do Lago: 55
    if (estacaoAtual != INVERNO) {
        Vector2 lagoCentro   = { 465.0f, 325.0f };
        float lagoRaio       = 55.0f;
        float dx             = posicao.x - lagoCentro.x;
        float dy             = posicao.y - lagoCentro.y;
        float distanciaCentro = sqrtf(dx * dx + dy * dy);
        float distanciaMinima = lagoRaio + raioColisao;

        if (distanciaCentro < distanciaMinima) {
            if (distanciaCentro > 0.0f) {
                float nx = dx / distanciaCentro;
                float ny = dy / distanciaCentro;

                // Empurra a vaca para fora da margem d'água líquida
                posicao.x = lagoCentro.x + nx * distanciaMinima;
                posicao.y = lagoCentro.y + ny * distanciaMinima;

                // Faz ela caminhar para a direção oposta do lago
                direcaoAtual.x    = nx;
                direcaoAtual.y    = ny;
                tempoMudarDirecao = 40;
            }
        }
    }

    // 4. CICLO DE FOME E PRODUÇÃO BIOLÓGICA
    if (nivelFome < 100.0f) {
        nivelFome += 0.02f; // A fome aumenta progressivamente com o tempo
    }

    // Se a vaca não estiver faminta (fome menor que 75), produz leite normalmente
    if (nivelFome < 75.0f && nivelFome < 90.0f  && nivelLeite < 100.0f) {
        nivelLeite += 0.05f;
    }
    else if (nivelFome >= 90.0f){
        nivelLeite += 0.0f;
    }
}

// =========================================================================
// PIPELINE GRÁFICO (RENDERIZAÇÃO)
// =========================================================================
void Vaca::Desenhar(Estacao estacao) {
    // Paleta de cores dinâmica baseada no estado de saúde (Fome > 75% = triste/faminta)
    Color corPele    = (nivelFome > 75.0f) ? LIGHTGRAY : RAYWHITE;
    Color corManchas = (nivelFome > 75.0f) ? DARKGRAY : BLACK;
    Color corFocinho = PINK;
    Color corChifres = LIGHTGRAY;
    Color corCascos  = DARKBROWN;

    float x = posicao.x;
    float y = posicao.y;

    // 1. DESENHO DAS PATAS E CASCOS
    DrawRectangle(x - 14, y + 10, 6, 15, corPele);
    DrawRectangle(x - 14, y + 22, 6, 4, corCascos);
    DrawRectangle(x + 8, y + 10, 6, 15, corPele);
    DrawRectangle(x + 8, y + 22, 6, 4, corCascos);

    DrawRectangle(x - 6, y + 12, 6, 15, corPele);
    DrawRectangle(x - 6, y + 24, 6, 4, corCascos);
    DrawRectangle(x + 1, y + 12, 6, 15, corPele);
    DrawRectangle(x + 1, y + 24, 6, 4, corCascos);

    // 2. DESENHO DO CORPO PRINCIPAL
    DrawCircleV(posicao, raioColisao, corPele);

    // 3. MANCHAS NATURAIS DE CONFIGURAÇÃO GEOMÉTRICA
    DrawCircle(x - 8, y - 6, 6, corManchas);
    DrawCircle(x + 6, y + 6, 7, corManchas);
    DrawCircle(x - 5, y + 10, 5, corManchas);
    DrawEllipse(x + 10, y - 5, 8, 5, corManchas);

    // 4. ÚBERE / TETAS (Aparece se tiver produzido leite mínimo)
    if (nivelLeite > 10.0f) {
        DrawCircle(x, y + 12, 7, PINK);
        DrawRectangle(x - 4, y + 15, 2, 4, PINK);
        DrawRectangle(x + 2, y + 15, 2, 4, PINK);
    }

    // 5. CABEÇA AND ANATOMIA FACIAL
    float cx = x + 14;
    float cy = y - 12;

    // Chifres e Orelhas laterais
    DrawTriangle({cx - 4, cy - 8}, {cx - 8, cy - 16}, {cx, cy - 8}, corChifres);
    DrawTriangle({cx + 4, cy - 8}, {cx, cy - 8}, {cx + 8, cy - 16}, corChifres);
    DrawEllipse(cx - 10, cy - 2, 6, 3, corPele);
    DrawEllipse(cx + 10, cy - 2, 6, 3, corPele);

    DrawCircle(cx, cy, 11, corPele);

    // [EXCLUSIVO DE INVERNO]: GORRO SAZONAL NATALINO
    if (estacao == INVERNO) {
        DrawEllipse((int)cx, (int)cy - 10, 9, 3, RAYWHITE); // Base fofinha
        DrawTriangle({cx - 7, cy - 10}, {cx + 7, cy - 10}, {cx + 4, cy - 20}, RED); // Cone do gorro
        DrawCircle((int)cx + 4, (int)cy - 20, 2.5f, RAYWHITE); // Pompom
    }

    // Expressão dos Olhos Baseada no Status de Fome
    if (nivelFome > 75.0f) { // Olho em X (Triste/Faminta)
        DrawLine(cx - 5, cy - 4, cx - 2, cy - 1, BLACK);
        DrawLine(cx - 2, cy - 4, cx - 5, cy - 1, BLACK);
        DrawLine(cx + 2, cy - 4, cx + 5, cy - 1, BLACK);
        DrawLine(cx + 5, cy - 4, cx + 2, cy - 1, BLACK);
    } else { // Olho Kawaii (Satisfeita)
        DrawCircle(cx - 4, cy - 3, 2.5f, BLACK);
        DrawCircle(cx + 4, cy - 3, 2.5f, BLACK);
        DrawCircle(cx - 4.5f, cy - 4, 0.8f, WHITE); // Brilho interno
        DrawCircle(cx + 3.5f, cy - 4, 0.8f, WHITE);
    }

    // Focinho e narinas
    DrawEllipse(cx, cy + 4, 9, 6, corFocinho);
    DrawCircle(cx - 3, cy + 4, 1.5f, DARKBROWN);
    DrawCircle(cx + 3, cy + 4, 1.5f, DARKBROWN);

    // 6. BARRAS DE STATUS DO ANIMAL
    float percentualFomeInvertido = (100.0f - nivelFome) / 100.0f;
    if (percentualFomeInvertido < 0) percentualFomeInvertido = 0;

    // Barra de Fome (Do Verde ao Vermelho)
    DrawRectangle(x - 20, y - 38, 40, 3, RED);
    DrawRectangle(x - 20, y - 38, (int)(40 * percentualFomeInvertido), 3, GREEN);

    // Barra de Leite (Branca / Preenchimento acumulativo)
    DrawRectangle(x - 20, y - 33, 40, 3, BLACK);
    DrawRectangle(x - 20, y - 33, (int)(40 * (nivelLeite / 100.0f)), 3, WHITE);
}