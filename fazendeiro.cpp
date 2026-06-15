#include "fazendeiro.h"

// =========================================================================
// CONSTRUTOR DA CLASSE
// =========================================================================
Fazendeiro::Fazendeiro(float x, float y, Genero gen)
    : posicao({x, y}),
    velocidade(3.5f),
    raioColisao(16.0f),
    leiteNoBalde(0.0f),
    racaoNoInventario(10),
    generoAtual(gen) {}

// =========================================================================
// CICLO DE VIDA DO JOGO (CONTROLE E MOVIMENTAÇÃO)
// =========================================================================
void Fazendeiro::Atualizar() {
    // Input do jogador: Suporta setas direcionais ou o padrão WASD
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) posicao.x += velocidade;
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) posicao.x -= velocidade;
    if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) posicao.y += velocidade;
    if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) posicao.y -= velocidade;

    // Limites de movimentação na tela (Border Box invisível do pasto)
    if (posicao.x < 20)  posicao.x = 20;
    if (posicao.x > 780) posicao.x = 780;
    if (posicao.y < 80)  posicao.y = 80;
    if (posicao.y > 580) posicao.y = 580;
}

// =========================================================================
// PIPELINE GRÁFICO (RENDERIZAÇÃO E CUSTOMIZAÇÃO SAZONAL)
// =========================================================================
void Fazendeiro::Desenhar(Estacao estacao, float escala) {
    float x = posicao.x;
    float y = posicao.y;

    // -------------------------------------------------------------------------
    // 1. PALETA DE CORES DINÂMICA (POR GÊNERO E ESTAÇÃO)
    // -------------------------------------------------------------------------
    Color corCamisa      = (generoAtual == MASCULINO) ? BLUE : Color{ 245, 235, 220, 255 };
    Color corCalca       = DARKBLUE;
    Color corVestido     = Color{ 210, 50, 45, 255 };
    Color corAvental     = Color{ 255, 255, 255, 230 };
    Color corBotas       = (generoAtual == MASCULINO) ? DARKBROWN : Color{ 90, 55, 35, 255 };
    Color corPele        = BEIGE;
    Color corChapeu      = GOLD;
    Color corChapeuFaixa = (generoAtual == MASCULINO) ? MAROON : Color{ 210, 50, 45, 255 };
    Color corCabelo      = Color{ 220, 130, 40, 255 };

    bool temAvental    = (generoAtual == FEMININO);
    bool casacoInverno = false;

    // Switch de Guarda-Roupa Sazonal
    switch (estacao) {
    case PRIMAVERA:
        if (generoAtual == MASCULINO) {
            corCamisa = Color{ 135, 206, 235, 255 }; // Camisa azul céu
            corCalca  = Color{ 34, 139, 34, 255 };   // Jardineira verde grama
        } else {
            corCamisa  = Color{ 255, 240, 245, 255 }; // Blusa rosa bebê
            corVestido = Color{ 255, 105, 180, 255 }; // Saia rosa choque florida
            corAvental = Color{ 255, 255, 224, 240 }; // Avental amarelo claro
        }
        break;

    case VERAO:
        corChapeu = Color{ 245, 222, 179, 255 };      // Palha desbotada do sol
        if (generoAtual == MASCULINO) {
            corCamisa = Color{ 255, 215, 0, 255 };    // Amarelo girassol
            corCalca  = Color{ 120, 150, 180, 255 }; // Jeans lavado
        } else {
            corCamisa  = Color{ 255, 255, 255, 255 }; // Blusa branca leve
            corVestido = Color{ 255, 165, 0, 255 };   // Vestido laranja tropical
            temAvental = false;                       // Sem avental no calor
        }
        break;

    case OUTONO:
        if (generoAtual == MASCULINO) {
            corCamisa = Color{ 210, 105, 30, 255 };  // Xadrez marrom outonal
            corCalca  = Color{ 70, 70, 70, 255 };     // Cinza escuro
        } else {
            corCamisa  = Color{ 244, 164, 96, 255 }; // Blusa ocre
            corVestido = Color{ 139, 69, 19, 255 };   // Castanho terra
            corAvental = Color{ 222, 184, 135, 220 }; // Bege vintage
        }
        break;

    case INVERNO:
        // Especial de Natal: Roupas de Papai e Mamãe Noel
        casacoInverno = true;
        corChapeu      = RED;
        corChapeuFaixa = WHITE;
        corBotas       = Color{ 25, 25, 25, 255 };     // Botas pretas
        corCamisa      = RED;
        corVestido     = RED;
        corAvental     = Color{ 255, 255, 255, 255 }; // Branco neve
        if (generoAtual == FEMININO) temAvental = true;
        break;
    }

    // -------------------------------------------------------------------------
    // 2. BOTAS E SOMBRA PROJETADA
    // -------------------------------------------------------------------------
    DrawEllipse((int)x, (int)y + (15 * escala), 8 * escala, 3 * escala, Color{ 0, 0, 0, 50 });
    DrawCircle(x - (4 * escala), y + (14 * escala), 3.5f * escala, corBotas);
    DrawCircle(x + (4 * escala), y + (14 * escala), 3.5f * escala, corBotas);

    if (casacoInverno) { // Detalhe fofinho de algodão das botas natalinas
        DrawRectangle(x - (6.5f * escala), y + (11 * escala), 5 * escala, 2 * escala, WHITE);
        DrawRectangle(x + (1.5f * escala), y + (11 * escala), 5 * escala, 2 * escala, WHITE);
    }

    // -------------------------------------------------------------------------
    // 3. CABELO TRASEIRO (EXCLUSIVO FEMININO - "TOTÓS")
    // -------------------------------------------------------------------------
    if (generoAtual == FEMININO) {
        DrawCircle(x, y - (18 * escala), 11 * escala, corCabelo);
        Color corElastico = (estacao == INVERNO) ? GREEN : RED; // Elásticos natalinos

        // Totó Esquerdo
        DrawRectangle(x - (11 * escala), y - (14 * escala), 4 * escala, 14 * escala, corCabelo);
        DrawCircle(x - (9 * escala), y - (6 * escala), 4 * escala, corCabelo);
        DrawCircle(x - (9 * escala), y - (10 * escala), 2 * escala, corElastico);
        DrawCircle(x - (9 * escala), y - (2 * escala), 2.5f * escala, corElastico);

        // Totó Direito
        DrawRectangle(x + (7 * escala), y - (14 * escala), 4 * escala, 14 * escala, corCabelo);
        DrawCircle(x + (9 * escala), y - (6 * escala), 4 * escala, corCabelo);
        DrawCircle(x + (9 * escala), y - (10 * escala), 2 * escala, corElastico);
        DrawCircle(x + (9 * escala), y - (2 * escala), 2.5f * escala, corElastico);
    }

    // -------------------------------------------------------------------------
    // 4. TRONCO E VESTIMENTA (ALÇAS / CINTO)
    // -------------------------------------------------------------------------
    if (generoAtual == MASCULINO) {
        DrawRectangle(x - (10 * escala), y - (10 * escala), 20 * escala, 10 * escala, corCamisa);

        if (casacoInverno) { // Cinto clássico do Papai Noel
            DrawRectangle(x - (10 * escala), y - (3 * escala), 20 * escala, 3 * escala, Color{ 20, 20, 20, 255 });
            DrawRectangle(x - (2 * escala), y - (4 * escala), 4 * escala, 5 * escala, GOLD);
            DrawRectangle(x - (1 * escala), y - (3 * escala), 2 * escala, 3 * escala, Color{ 20, 20, 20, 255 });
            DrawRectangle(x - (1 * escala), y - (10 * escala), 2 * escala, 7 * escala, WHITE);
        } else { // Alças da Jardineira de fazendeiro
            DrawRectangle(x - (8 * escala), y - (10 * escala), 3 * escala, 10 * escala, corCalca);
            DrawRectangle(x + (5 * escala), y - (10 * escala), 3 * escala, 10 * escala, corCalca);
        }
    } else {
        DrawRectangle(x - (9 * escala), y - (10 * escala), 18 * escala, 10 * escala, corCamisa);

        if (casacoInverno) { // Corpete natalino da Mamãe Noel
            DrawRectangle(x - (9 * escala), y - (10 * escala), 18 * escala, 10 * escala, corVestido);
            DrawRectangle(x - (2 * escala), y - (10 * escala), 4 * escala, 10 * escala, WHITE);
            DrawRectangle(x - (9 * escala), y - (3 * escala), 18 * escala, 3 * escala, Color{ 20, 20, 20, 255 });
            DrawRectangle(x - (2 * escala), y - (4 * escala), 4 * escala, 5 * escala, GOLD);
        } else { // Corpete camponês folclórico
            DrawRectangle(x - (9 * escala), y - (10 * escala), 4 * escala, 10 * escala, corVestido);
            DrawRectangle(x + (5 * escala), y - (10 * escala), 4 * escala, 10 * escala, corVestido);
            DrawRectangle(x - (5 * escala), y - (4 * escala), 10 * escala, 4 * escala, corVestido);
        }
    }

    // -------------------------------------------------------------------------
    // 5. PARTE INFERIOR (CALÇAS VS SAIA COM AVENTAL)
    // -------------------------------------------------------------------------
    if (generoAtual == MASCULINO) {
        DrawRectangle(x - (10 * escala), y, 20 * escala, 12 * escala, corCalca);
    } else {
        // Saia armada por triângulos geométricos
        DrawTriangle({ x - (14 * escala), y + (13 * escala) }, { x + (14 * escala), y + (13 * escala) }, { x, y - (4 * escala) }, corVestido);
        Color corSombraSaia = (estacao == INVERNO) ? Color{ 140, 20, 20, 255 } : MAROON;
        DrawTriangle({ x - (14 * escala), y + (13 * escala) }, { x - (10 * escala), y + (13 * escala) }, { x, y - (4 * escala) }, corSombraSaia);
        DrawTriangle({ x + (10 * escala), y + (13 * escala) }, { x + (14 * escala), y + (13 * escala) }, { x, y - (4 * escala) }, corSombraSaia);

        if (temAvental) { // Desenho do avental protetor de ordenha
            DrawTriangle({ x - (8 * escala), y + (11 * escala) }, { x + (8 * escala), y + (11 * escala) }, { x, y - (2 * escala) }, corAvental);
            DrawRectangle(x - (5 * escala), y - (1 * escala), 10 * escala, 2 * escala, corAvental);

            if (estacao == INVERNO) { // Listras natalinas decorativas verdes
                DrawLineEx({x - (5 * escala), y + (3 * escala)}, {x - (7 * escala), y + (10 * escala)}, 1.5f * escala, GREEN);
                DrawLineEx({x + (5 * escala), y + (3 * escala)}, {x + (7 * escala), y + (10 * escala)}, 1.5f * escala, GREEN);
            }
        }
    }

    // -------------------------------------------------------------------------
    // 6. ROSTO E CABEÇA (OLHOS COM BRILHO/SORRISO)
    // -------------------------------------------------------------------------
    DrawCircle(x, y - (16 * escala), 8 * escala, corPele);

    if (generoAtual == FEMININO) { // Franja e mechas laterais femininas
        DrawRectangle(x - (7 * escala), y - (22 * escala), 14 * escala, 4 * escala, corCabelo);
        DrawRectangle(x - (8 * escala), y - (20 * escala), 2 * escala, 6 * escala, corCabelo);
        DrawRectangle(x + (6 * escala), y - (20 * escala), 2 * escala, 6 * escala, corCabelo);
    }

    DrawCircle(x - (3 * escala), y - (17 * escala), 1.5f * escala, BLACK);
    DrawCircle(x + (3 * escala), y - (17 * escala), 1.5f * escala, BLACK);
    DrawCircle(x - (2.5f * escala), y - (17.5f * escala), 0.5f * escala, RAYWHITE); // Brilho ocular
    DrawCircle(x + (3.5f * escala), y - (17.5f * escala), 0.5f * escala, RAYWHITE);
    DrawLineEx({x - (2 * escala), y - (13 * escala)}, {x + (2 * escala), y - (13 * escala)}, 1.2f * escala, MAROON);

    // -------------------------------------------------------------------------
    // 7. ACESSÓRIO DE CABEÇA (CHAPÉU DE PALHA VS GORRO DE NOEL)
    // -------------------------------------------------------------------------
    if (casacoInverno) {
        DrawEllipse(x, y - (22 * escala), 13 * escala, 3 * escala, WHITE); // Base de pelúcia
        DrawTriangle({x - (10 * escala), y - (22 * escala)}, {x + (10 * escala), y - (22 * escala)}, {x - (6 * escala), y - (34 * escala)}, RED);
        DrawCircle(x - (7 * escala), y - (34 * escala), 3 * escala, WHITE); // Pomponzinho caído
    } else {
        DrawEllipse(x, y - (22 * escala), 16 * escala, 4 * escala, corChapeu);
        DrawRectangle(x - (7 * escala), y - (25 * escala), 14 * escala, 3 * escala, corChapeuFaixa);
        DrawRectangle(x - (7 * escala), y - (30 * escala), 14 * escala, 5 * escala, corChapeu);
    }

    // -------------------------------------------------------------------------
    // 8. BRAÇOS E BALDE DE ORDENHA METÁLICO
    // -------------------------------------------------------------------------
    Color corMaoEsquerda = casacoInverno ? WHITE : corPele; // Luvas brancas no inverno
    DrawLineEx({x - (9 * escala), y - (8 * escala)}, {x - (15 * escala), y - (2 * escala)}, 3 * escala, corCamisa);
    DrawLineEx({x - (15 * escala), y - (2 * escala)}, {x - (11 * escala), y + (3 * escala)}, 3 * escala, corCamisa);
    DrawCircle(x - (11 * escala), y + (4 * escala), 2 * escala, corMaoEsquerda);

    Color corMaoDireita = casacoInverno ? WHITE : corPele;
    DrawLineEx({x + (9 * escala), y - (8 * escala)}, {x + (15 * escala), y - (1 * escala)}, 3 * escala, corCamisa);
    DrawLineEx({x + (15 * escala), y - (1 * escala)}, {x + (15 * escala), y + (6 * escala)}, 3 * escala, corCamisa);
    DrawCircle(x + (15 * escala), y + (7 * escala), 2.5f * escala, corMaoDireita);

    // Renderização Física do Balde na Mão Direita
    float bx = x + (15 * escala);
    float by = y + (11 * escala);
    DrawCircleLines(bx, by, 5 * escala, GRAY); // Alça circular

    Color corFundoBalde = (estacao == INVERNO) ? RED : LIGHTGRAY;
    Color corGradBalde  = (estacao == INVERNO) ? MAROON : GRAY;
    Color corBordaBalde = (estacao == INVERNO) ? GOLD : DARKGRAY;

    DrawRectangleGradientH((int)(bx - (5 * escala)), (int)(by + (4 * escala)), 10 * escala, 10 * escala, corFundoBalde, corGradBalde);
    DrawRectangleLines((int)(bx - (5 * escala)), (int)(by + (4 * escala)), 10 * escala, 10 * escala, corBordaBalde);

    // -------------------------------------------------------------------------
    // 9. INDICADOR VISUAL DE LEITE (MECÂNICA DE COMPORTAMENTO)
    // -------------------------------------------------------------------------
    if (leiteNoBalde > 0.0f) {
        float limiteLeite = (leiteNoBalde > 100.0f) ? 100.0f : leiteNoBalde;
        int alturaLeite   = (int)(8 * (limiteLeite / 100.0f) * escala);
        if (alturaLeite < 1) alturaLeite = 1;

        // Desenha a barra branca de preenchimento simulando o líquido subindo
        DrawRectangle(bx - (4 * escala), by + (13 * escala) - alturaLeite, 8 * escala, alturaLeite, RAYWHITE);
    }
}

// =========================================================================
// INTERAÇÕES E REGRAS DE NEGÓCIO (SETTERS E MODIFICADORES)
// =========================================================================
void Fazendeiro::SetLeiteBalde(float qtd) {
    leiteNoBalde = qtd; // Ajusta a quantidade líquida no balde
}

void Fazendeiro::SetGenero(Genero gen) {
    generoAtual = gen; // Permite trocar de personagem na seleção de perfil
}

void Fazendeiro::SetPosicao(float x, float y) {
    posicao.x = x;
    posicao.y = y;
}

void Fazendeiro::AdicionarLeite(float qtd) {
    leiteNoBalde += qtd; // Soma o leite coletado na ordenha atual
}

float Fazendeiro::EntregarLeite() {
    float total  = leiteNoBalde;
    leiteNoBalde = 0.0f; // Esvazia o balde após vender/armazenar e retorna o montante
    return total;
}

bool Fazendeiro::ReabastecerRacao() {
    if (racaoNoInventario < 10) {
        racaoNoInventario = 10; // Enche a sacola no Silo até o limite de 10 unidades
        return true;
    }
    return false;
}

void Fazendeiro::UsarRacao() {
    if (racaoNoInventario > 0) racaoNoInventario--; // Consome um saquinho ao alimentar a vaca
}

// =========================================================================
// MÉTODOS DE ACESSO (GETTERS - CONST)
// =========================================================================
Genero  Fazendeiro::GetGenero() const       { return generoAtual; }
Vector2 Fazendeiro::GetPosicao() const      { return posicao; }
float   Fazendeiro::GetRaioColisao() const   { return raioColisao; }
int     Fazendeiro::GetRacao() const        { return racaoNoInventario; }
float   Fazendeiro::GetLeiteBalde() const   { return leiteNoBalde; }