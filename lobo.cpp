#include "lobo.h"
#include <cmath>

// =========================================================================
// CONSTRUTORES DA CLASSE
// =========================================================================

// Construtor padrão inicializando as variáveis base (usado na instanciação global)
Lobo::Lobo() {
    posicao     = { -50.0f, -50.0f };
    velocidade  = 1.5f;
    raioColisao = 18.0f;
    ativo       = false;
    direcaoX    = 1.0f;
}

// Construtor sobrecarregado com coordenadas iniciais parametrizadas
Lobo::Lobo(float x, float y) {
    posicao.x   = x;
    posicao.y   = y;
    velocidade  = 1.5f;
    raioColisao = 18.0f;
    ativo       = false;
    direcaoX    = 1.0f;
}

// =========================================================================
// PIPELINE GRÁFICO (RENDERIZAÇÃO E CUSTOMIZAÇÃO AMBIENTAL)
// =========================================================================
void Lobo::Desenhar(Estacao estacao) {
    if (!ativo) return;

    // 1. Sombra projetada no terreno para percepção de profundidade
    DrawEllipse((int)posicao.x, (int)posicao.y + 12, 18, 5, { 0, 0, 0, 50 });

    // Fator de espelhamento baseado na direção horizontal atual (Esquerda vs Direita)
    int inverter = (direcaoX < 0) ? -1 : 1;

    // 2. Paleta de Cores Padrão (Verão / Primavera)
    Color PELO_BASE   = { 95, 100, 105, 255 };
    Color PELO_ESC    = { 65, 68, 75, 255 };
    Color PELO_CLARO  = { 145, 150, 155, 255 };
    Color OLHO_LOBO   = { 255, 215, 0, 255 };

    // Adaptação biológica/visual dependendo do ciclo sazonal
    if (estacao == INVERNO) {
        PELO_BASE  = { 200, 205, 210, 255 };
        PELO_ESC   = { 140, 145, 150, 255 };
        PELO_CLARO = { 240, 245, 250, 255 };
    } else if (estacao == OUTONO) {
        PELO_BASE  = { 110, 100, 90, 255 };
        PELO_ESC   = { 75, 68, 60, 255 };
    }

    // 3. Rabo com micro-animação procedural aleatória
    DrawTriangle(
        { posicao.x - (12 * inverter), posicao.y + 2 },
        { posicao.x - (26 * inverter), posicao.y + (float)GetRandomValue(3, 7) },
        { posicao.x - (14 * inverter), posicao.y - 6 },
        PELO_ESC
        );

    // 4. Membros Inferiores Traseiros (Renderizados sob a camada do tronco)
    DrawRectangle((int)posicao.x - (8 * inverter), (int)posicao.y + 4, 3, 9, PELO_ESC);
    DrawRectangle((int)posicao.x + (2 * inverter), (int)posicao.y + 4, 3, 9, PELO_ESC);

    // 5. Corpo Principal (Tronco e peitoral)
    DrawEllipse((int)posicao.x, (int)posicao.y, 14, 9, PELO_BASE);
    DrawEllipse((int)posicao.x, (int)posicao.y - 2, 11, 5, PELO_CLARO);

    // 6. Membros Inferiores Dianteiros (Renderizados sobrepondo o tronco)
    DrawRectangle((int)posicao.x - (3 * inverter), (int)posicao.y + 5, 3, 8, PELO_BASE);
    DrawRectangle((int)posicao.x + (6 * inverter), (int)posicao.y + 5, 3, 8, PELO_BASE);

    // 7. Pescoço e Base da Cabeça
    DrawCircle((int)posicao.x + (8 * inverter), (int)posicao.y - 4, 7, PELO_ESC);
    DrawCircle((int)posicao.x + (12 * inverter), (int)posicao.y - 9, 6, PELO_BASE);

    // 8. Orelhas Alertas
    DrawTriangle(
        { posicao.x + (8 * inverter),  posicao.y - 12 },
        { posicao.x + (10 * inverter), posicao.y - 18 },
        { posicao.x + (13 * inverter), posicao.y - 13 },
        PELO_ESC
        );

    // 9. Estrutura Craniana e Focinho Predatório
    DrawTriangle(
        { posicao.x + (12 * inverter), posicao.y - 6 },
        { posicao.x + (21 * inverter), posicao.y - 8 },
        { posicao.x + (14 * inverter), posicao.y - 11 },
        PELO_BASE
        );

    // Olhos brilhantes e anatomia facial detalhada
    DrawCircle((int)posicao.x + (21 * inverter), (int)posicao.y - 8, 1.5f, BLACK);
    DrawCircle((int)posicao.x + (14 * inverter), (int)posicao.y - 10, 1.5f, OLHO_LOBO);
    DrawCircle((int)posicao.x + (14 * inverter), (int)posicao.y - 10, 0.5f, BLACK);

    // 10. Interface de Usuário (Alerta de Perigo iminente na tela)
    DrawText("ALERTA LOBO!", (int)posicao.x - 30, (int)posicao.y - 28, 11, RED);
}

// =========================================================================
// MÉTODOS DE CONTROLE DE FLUXO E ESTADO
// =========================================================================
void Lobo::Atualizar() {
    // Nota de Desenvolvimento: Movimentação delegada para PerseguirRebanho para IA ativa.
}

void Lobo::Spawnar(float x, float y, float vel) {
    posicao.x  = x;
    posicao.y  = y;
    velocidade = vel;
    ativo      = true;
}

void Lobo::Desativar() {
    ativo = false;
}

void Lobo::SetPosicao(float x, float y) {
    posicao.x = x;
    posicao.y = y;
}

// =========================================================================
// INTELIGÊNCIA ARTIFICIAL (ALGORITMO DE CAÇA)
// =========================================================================
bool Lobo::PerseguirRebanho(std::vector<Vaca>& rebanho) {
    if (!ativo || rebanho.empty()) return false;

    int indiceAlvo       = -1;
    float menorDistancia = 999999.0f;

    // 1. Escaneamento do perímetro: Busca pela unidade de rebanho viva mais próxima
    for (size_t i = 0; i < rebanho.size(); i++) {
        float dx   = rebanho[i].GetPosicao().x - posicao.x;
        float dy   = rebanho[i].GetPosicao().y - posicao.y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < menorDistancia) {
            menorDistancia = dist;
            indiceAlvo     = i;
        }
    }

    // 2. Processamento do Vetor Direcional de Perseguição
    if (indiceAlvo != -1) {
        float dx = rebanho[indiceAlvo].GetPosicao().x - posicao.x;
        float dy = rebanho[indiceAlvo].GetPosicao().y - posicao.y;

        if (menorDistancia > 0) {
            posicao.x += (dx / menorDistancia) * velocidade;
            posicao.y += (dy / menorDistancia) * velocidade;

            // Atualiza a orientação do sprite baseado na direção horizontal do vetor velocidade
            direcaoX = (dx > 0) ? 1.0f : -1.0f;
        }

        // 3. Resolução de Colisão Dinâmica (Alcance do Ataque)
        if (menorDistancia < (raioColisao + rebanho[indiceAlvo].GetRaioColisao())) {
            // Aplica punição mecânica: o susto faz a vaca perder o leite acumulado
            rebanho[indiceAlvo].SetNivelLeite(0.0f);

            // Retorna o lobo ao estado inativo/retirada após consumir o alvo
            ativo = false;
            return true;
        }
    }

    return false;
}

// =========================================================================
// MÉTODOS DE CONSULTA (GETTERS CONST)
// =========================================================================
Vector2 Lobo::GetPosicao() const     { return posicao; }
bool    Lobo::IsAtivo() const        { return ativo; }
float   Lobo::GetRaioColisao() const { return raioColisao; }