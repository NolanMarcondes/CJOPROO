#include "cenario.h"
#include <cmath>

// =========================================================================
// CONSTRUTOR DA CLASSE
// =========================================================================
Cenario::Cenario() {
    // Inicialização da Paleta de Cores (Formatos RGBA)
    VERDE_PASTO      = { 124, 215, 104, 255 };
    VERDE_GRAMA      = { 75, 165, 60, 255 };
    SOMBRA_REFRACAO  = { 0, 0, 0, 45 };
    CELEIRO_VERMELHO = { 190, 45, 35, 255 };
    CELEIRO_ESCURO   = { 125, 25, 20, 255 };
    CELEIRO_LUZ      = { 220, 65, 50, 255 };
    AMARELO_MILHO    = { 245, 210, 80, 255 };
    AMARELO_SOMBRA   = { 195, 155, 35, 255 };
    MADEIRA_CERCA    = { 120, 80, 50, 255 };
    TERRA_BATIDA     = { 185, 140, 100, 255 };
    TERRA_ESCURA     = { 150, 110, 75, 255 };
    LAMA_POCOA       = { 115, 80, 55, 255 };

    // Dimensões e Posicionamento do Lago do Pasto
    lagoCentro = { 465.0f, 325.0f };
    lagoRaio   = 55.0f;
}

// =========================================================================
// ELEMENTOS RASTROS E SOMBREAMENTO
// =========================================================================

// Desenha a elipse translúcida de sombra abaixo das entidades do mapa
void Cenario::DesenharSombraProjetada(float x, float y, float largura, float altura) {
    DrawEllipse((int)x, (int)y, largura / 2, altura / 2, SOMBRA_REFRACAO);
}

// Desenha o caminho de estradas internas da fazenda com bordas de relevo
void Cenario::DesenharCaminhoTerra(float x1, float y1, float x2, float y2, float largura) {
    DrawLineEx({x1, y1}, {x2, y2}, largura, TERRA_BATIDA);
    DrawLineEx({x1, y1}, {x2, y2}, largura - 4, TERRA_ESCURA);
}

// =========================================================================
// GEOMETRIA DA NATUREZA (VEGETAÇÃO, FLORES E ÁGUA)
// =========================================================================

// Renderiza o lago principal adaptando o visual entre água fluida ou congelada (Inverno)
void Cenario::DesenharLago(Vector2 centro, float raio, Estacao estacao) {
    if (estacao == INVERNO) {
        // Camadas de gelo texturizadas
        DrawCircleV(centro, raio + 6, { 140, 180, 200, 255 });
        DrawCircleV(centro, raio, { 180, 220, 235, 255 });
        DrawCircleV(centro, raio - 8, { 200, 235, 245, 255 });

        // Rachaduras estéticas no gelo
        DrawLineV({centro.x - 30, centro.y - 10}, {centro.x + 20, centro.y + 15}, { 255, 255, 255, 120 });
        DrawLineV({centro.x + 10, centro.y - 25}, {centro.x - 15, centro.y + 20}, { 255, 255, 255, 120 });
    } else {
        // Água fluida com borda de terra úmida e reflexo solar
        DrawCircleV(centro, raio + 6, TERRA_ESCURA);
        DrawCircleV(centro, raio, { 40, 105, 180, 255 });
        DrawCircleV(centro, raio - 8, { 65, 135, 215, 255 });
        DrawEllipse((int)centro.x - 15, (int)centro.y - 15, 20, 6, { 255, 255, 255, 60 });
    }
}

// Renderiza pequenos blocos de pixel art simulando tufos de grama silvestre
void Cenario::DesenharTufoGrama(float x, float y, Color corGrama) {
    DrawRectangle((int)x, (int)y - 6, 3, 6, corGrama);
    DrawRectangle((int)x - 3, (int)y - 4, 3, 4, corGrama);
    DrawRectangle((int)x + 3, (int)y - 5, 3, 5, corGrama);
}

// Desenha flores decorativas alternando as cores de acordo com as estações do ano
void Cenario::DesenharFlores(float x, float y, Estacao estacao) {
    if (estacao == INVERNO) return; // Flores somem no inverno

    Color corMiolo  = GOLD;
    Color corPetala = PURPLE;

    if (estacao == PRIMAVERA) { corPetala = PINK;   corMiolo = YELLOW; }
    if (estacao == VERAO)     { corPetala = ORANGE; corMiolo = RED;    }
    if (estacao == OUTONO)    { corPetala = MAROON; corMiolo = GOLD;   }

    // Desenho dos 4 quadrantes das pétalas e o núcleo central
    DrawCircle((int)x - 3, (int)y, 3, corPetala);
    DrawCircle((int)x + 3, (int)y, 3, corPetala);
    DrawCircle((int)x, (int)y - 3, 3, corPetala);
    DrawCircle((int)x, (int)y + 3, 3, corPetala);
    DrawCircle((int)x, (int)y, 2, corMiolo);
}

// Renderiza pedras com técnica de brilho superior e sombra projetada própria
void Cenario::DesenharPedra(float x, float y, float raio) {
    DrawEllipse((int)x + 2, (int)y + 2, raio, raio * 0.7f, SOMBRA_REFRACAO);
    DrawEllipse((int)x, (int)y, raio, raio * 0.7f, LIGHTGRAY);
    DrawEllipse((int)x - (raio * 0.2f), (int)y - (raio * 0.1f), raio * 0.7f, raio * 0.5f, RAYWHITE);
}

// Cria copas estilizadas dinâmicas que alteram frutos, folhas secas ou neve acumulada
void Cenario::DesenharArvoreEstilizada(float x, float y, Estacao estacao) {
    DesenharSombraProjetada(x, y + 35, 60, 15);
    DrawRectangle((int)x - 8, (int)y, 16, 35, DARKBROWN); // Tronco principal

    if (estacao == PRIMAVERA) {
        // Galhos secos aparentes com pequenos brotos de Cerejeira (Pink)
        DrawLineEx({x, y + 5}, {x - 20, y - 15}, 4, DARKBROWN);
        DrawLineEx({x, y + 10}, {x + 25, y - 10}, 4, DARKBROWN);
        DrawLineEx({x, y - 5}, {x, y - 25}, 5, DARKBROWN);
        DrawCircle((int)x - 15, (int)y - 10, 3, PINK);
        DrawCircle((int)x + 18, (int)y - 5, 3, PINK);
    }
    else if (estacao == VERAO) {
        // Árvore cheia, folhagem vívida e maçãs vermelhas
        DrawCircle((int)x, (int)y - 12, 30, DARKGREEN);
        DrawCircle((int)x - 12, (int)y - 22, 22, GREEN);
        DrawCircle((int)x + 12, (int)y - 22, 22, GREEN);
        DrawCircle((int)x - 10, (int)y - 15, 3, MAROON);
        DrawCircle((int)x + 12, (int)y - 25, 3, MAROON);
    }
    else if (estacao == OUTONO) {
        // Folhagens secas com paleta gradiente em tons quentes/alaranjados
        DrawCircle((int)x, (int)y - 12, 28, { 180, 90, 30, 255 });
        DrawCircle((int)x - 12, (int)y - 22, 20, { 210, 130, 40, 255 });
        DrawCircle((int)x + 12, (int)y - 22, 20, { 230, 175, 50, 255 });
    }
    else if (estacao == INVERNO) {
        // Galhos nus com blocos ovais simulando acúmulo de neve espessa
        DrawLineEx({x, y + 5}, {x - 20, y - 15}, 4, DARKBROWN);
        DrawLineEx({x, y + 10}, {x + 25, y - 10}, 4, DARKBROWN);
        DrawCircle((int)x - 10, (int)y - 12, 8, RAYWHITE);
        DrawCircle((int)x + 15, (int)y - 7, 8, RAYWHITE);
        DrawCircle((int)x, (int)y - 22, 10, RAYWHITE);
    }
}

// =========================================================================
// DELIMITADORES GEOMÉTRICOS (CERCAS DO PASTO)
// =========================================================================

// Cria cercas horizontais calculando e distribuindo as estacas de madeira automaticamente
void Cenario::DesenharCercaH(float x1, float x2, float y) {
    int passo = 40;
    int quantidadeEstacas = (int)((x2 - x1) / passo);

    for (int i = 0; i <= quantidadeEstacas; i++) {
        float posX = x1 + (i * passo);
        DrawRectangle((int)posX, (int)y - 12, 4, 16, MADEIRA_CERCA);
    }
    // Tábuas de conexão horizontal
    DrawRectangle((int)x1, (int)y - 8, (int)(x2 - x1), 3, MADEIRA_CERCA);
    DrawRectangle((int)x1, (int)y - 2, (int)(x2 - x1), 3, MADEIRA_CERCA);
}

// Cria cercas verticais calculando e distribuindo as estacas de madeira automaticamente
void Cenario::DesenharCercaV(float x, float y1, float y2) {
    int passo = 40;
    int quantidadeEstacas = (int)((y2 - y1) / passo);

    for (int i = 0; i <= quantidadeEstacas; i++) {
        float posY = y1 + (i * passo);
        DrawRectangle((int)x - 2, (int)posY, 16, 4, MADEIRA_CERCA);
    }
    // Tábuas de conexão vertical
    DrawRectangle((int)x + 2, (int)y1, 3, (int)(y2 - y1), MADEIRA_CERCA);
    DrawRectangle((int)x + 8, (int)y1, 3, (int)(y2 - y1), MADEIRA_CERCA);
}

// =========================================================================
// CONSTRUÇÕES URBANAS E EDIFÍCIOS INTERATIVOS
// =========================================================================

// Desenha o silo vertical de armazenamento de ração (Grãos amarelos e topo metálico)
void Cenario::DesenharSilo(float x, float y) {
    DrawEllipse((int)x + 40, (int)y + 95, 45, 12, SOMBRA_REFRACAO);

    // Suportes metálicos inferiores da base
    DrawLineEx({x + 15, y + 70}, {x + 10, y + 95}, 4, DARKGRAY);
    DrawLineEx({x + 65, y + 70}, {x + 70, y + 95}, 4, DARKGRAY);
    DrawLineEx({x + 40, y + 70}, {x + 40, y + 95}, 3, GRAY);

    // Funil cônico de escoamento
    DrawTriangle({x + 10, y + 60}, {x + 40, y + 80}, {x + 70, y + 60}, GRAY);
    DrawTriangle({x + 10, y + 60}, {x + 40, y + 80}, {x + 25, y + 60}, DARKGRAY);

    // Corpo cilíndrico de acrílico/vidro mostrando a silagem amarela
    DrawRectangleGradientH((int)x + 10, (int)y + 10, 60, 50, AMARELO_MILHO, AMARELO_SOMBRA);
    DrawRectangle((int)x + 10, (int)y + 22, 60, 2, { 160, 120, 30, 255 });
    DrawRectangle((int)x + 10, (int)y + 36, 60, 2, { 160, 120, 30, 255 });
    DrawRectangle((int)x + 10, (int)y + 50, 60, 2, { 160, 120, 30, 255 });

    // Cúpula superior de zinco com reflexo
    DrawCircleSector({x + 40, y + 12}, 30, 180, 360, 16, LIGHTGRAY);
    DrawCircleSector({x + 40, y + 12}, 30, 270, 360, 16, RAYWHITE);

    // Letreiro de identificação
    DrawRectangleLines((int)x + 20, (int)y + 25, 40, 16, DARKBROWN);
    DrawText("SILO", (int)x + 28, (int)y + 28, 12, DARKBROWN);
}

// Desenha o reservatório horizontal de resfriamento de leite industrial
void Cenario::DesenharTanque(float x, float y) {
    DrawRectangle((int)x - 5, (int)y + 65, 100, 15, SOMBRA_REFRACAO);
    DrawRectangle((int)x - 2, (int)y + 60, 94, 12, DARKGRAY);
    DrawRectangle((int)x - 2, (int)y + 60, 94, 3, LIGHTGRAY);

    // Tanque de Inox pressurizado com efeitos de chanfro e gradiente de iluminação
    DrawRectangleGradientH((int)x, (int)y + 15, 90, 45, LIGHTGRAY, GRAY);
    DrawRectangle((int)x + 15, (int)y + 15, 12, 45, RAYWHITE);
    DrawRectangleGradientH((int)x + 27, (int)y + 15, 20, 45, RAYWHITE, Fade(RAYWHITE, 0.0f));

    // Sistema hidráulico, medidores e painel frontal
    DrawRectangleGradientV((int)x + 70, (int)y - 2, 8, 17, GRAY, DARKGRAY);
    DrawLineEx({x + 74, y + 15}, {x + 74, y + 50}, 3, DARKGRAY);
    DrawCircle((int)x + 74, (int)y + 30, 5, SKYBLUE);
    DrawRectangle((int)x + 12, (int)y + 27, 50, 20, { 50, 60, 75, 255 });
    DrawRectangleLines((int)x + 12, (int)y + 27, 50, 20, RAYWHITE);
    DrawText("LEITE", (int)x + 24, (int)y + 32, 11, RAYWHITE);
}

// Desenha o grande Celeiro Americano vermelho (Estrutura principal de upgrades do jogo)
void Cenario::DesenharCeleiro(float x, float y) {
    DrawRectangle((int)x - 10, (int)y + 90, 160, 15, SOMBRA_REFRACAO);

    // Fachada e paredes principais em tábuas verticais sombreadas
    DrawRectangleGradientH((int)x, (int)y + 25, 140, 65, CELEIRO_LUZ, CELEIRO_VERMELHO);
    DrawRectangle((int)x + 125, (int)y + 25, 15, 65, CELEIRO_ESCURO);
    for (int i = 12; i < 125; i += 12) {
        DrawRectangle((int)(x + i), (int)y + 25, 1, 65, CELEIRO_ESCURO);
    }

    // Complexo arquitetônico do telhado estilo Mansarda (Branco e Vinho)
    DrawRectangle((int)x - 8, (int)y + 5, 156, 20, RAYWHITE);
    DrawRectangle((int)x - 5, (int)y + 8, 150, 14, { 150, 40, 35, 255 });
    DrawRectangleGradientH((int)x + 15, (int)y - 15, 110, 20, RAYWHITE, LIGHTGRAY);
    DrawRectangle((int)x + 18, (int)y - 12, 104, 17, MAROON);
    DrawTriangle({x - 8, y + 5}, {x + 15, y - 15}, {x + 15, y + 5}, RAYWHITE);
    DrawTriangle({x + 148, y + 5}, {x + 125, y + 5}, {x + 125, y - 15}, LIGHTGRAY);

    // Sótão superior do feno e janelas decorativas redondas com esquadrias
    DrawRectangle((int)x + 58, (int)y - 27, 24, 12, RAYWHITE);
    DrawTriangle({x + 55, y - 27}, {x + 70, y - 38}, {x + 85, y - 27}, CELEIRO_VERMELHO);
    DrawRectangle((int)x + 40, (int)y + 40, 60, 50, { 25, 20, 20, 255 });
    DrawRectangleLines((int)x + 39, (int)y + 39, 62, 51, RAYWHITE);

    // Portões duplos de correr com travessas em "X" decorativas brancas
    DrawRectangle((int)x + 12, (int)y + 43, 24, 47, RAYWHITE);
    DrawRectangleLines((int)x + 12, (int)y + 43, 24, 47, CELEIRO_ESCURO);
    DrawLineEx({x + 12, y + 43}, {x + 36, y + 90}, 2, CELEIRO_VERMELHO);
    DrawLineEx({x + 36, y + 43}, {x + 12, y + 90}, 2, CELEIRO_VERMELHO);

    DrawRectangle((int)x + 104, (int)y + 43, 24, 47, RAYWHITE);
    DrawRectangleLines((int)x + 104, (int)y + 43, 24, 47, CELEIRO_ESCURO);
    DrawLineEx({x + 104, y + 43}, {x + 128, y + 90}, 2, CELEIRO_VERMELHO);
    DrawLineEx({x + 128, y + 43}, {x + 104, y + 90}, 2, CELEIRO_VERMELHO);

    DrawCircle((int)x + 70, (int)y + 12, 12, RAYWHITE);
    DrawCircle((int)x + 70, (int)y + 12, 9, { 55, 65, 85, 255 });
    DrawLine((int)x + 70, (int)y, (int)x + 70, (int)y + 24, RAYWHITE);
    DrawLine((int)x + 58, (int)y + 12, (int)x + 82, (int)y + 12, RAYWHITE);
}

// Desenha a fábrica industrial de processamento de queijos ("Malu Queijos") com múltiplos efeitos visuais
void Cenario::DesenharFabricaQueijo(float x, float y) {
    // 1. Sombra Projetada Avançada (Profundidade no solo)
    DrawEllipse((int)x + 40, (int)y + 60, 55, 12, { 0, 0, 0, 40 });
    DrawEllipse((int)x + 40, (int)y + 60, 35, 7,  { 0, 0, 0, 65 });

    // 2. Tubulação Industrial Traseira com Relevo
    DrawRectangle((int)x - 8, (int)y + 18, 14, 32, DARKGRAY);
    DrawRectangle((int)x - 6, (int)y + 18, 4, 32, GRAY);
    DrawCircle((int)x - 1, (int)y + 18, 6, GRAY);

    // 3. Chaminé Industrial Compacta + Fumaça Metalizada
    DrawRectangle((int)x + 62, (int)y - 30, 14, 65, { 70, 75, 85, 255 });
    DrawRectangle((int)x + 64, (int)y - 30, 3, 65, { 110, 115, 125, 255 });
    DrawRectangle((int)x + 59, (int)y - 30, 20, 5, MAROON);
    DrawRectangle((int)x + 62, (int)y - 5, 14, 4, BLACK);
    DrawCircle((int)x + 69, (int)y - 38, 6, Fade(LIGHTGRAY, 0.4f));
    DrawCircle((int)x + 77, (int)y - 48, 10, Fade(LIGHTGRAY, 0.3f));
    DrawCircle((int)x + 88, (int)y - 58, 14, Fade(LIGHTGRAY, 0.2f));

    // 4. Parede Principal (Com Textura de Linhas de Tijolo)
    DrawRectangleGradientH((int)x, (int)y + 5, 80, 55, { 190, 190, 195, 255 }, { 135, 135, 140, 255 });
    for (int i = 12; i < 55; i += 10) {
        DrawLine((int)x, (int)y + 5 + i, (int)x + 80, (int)y + 5 + i, { 115, 115, 120, 80 });
    }
    DrawRectangle((int)x, (int)y + 5, 80, 2, WHITE);

    // 5. Telhado em "Dente de Serra" Duplo Recalculado
    DrawRectangleGradientH((int)x, (int)y + 5, 80, 4, { 0, 0, 0, 70 }, { 0, 0, 0, 0 });
    DrawTriangle({ x, y + 5 }, { x + 40, y - 12 }, { x + 40, y + 5 }, { 215, 70, 55, 255 });
    DrawTriangle({ x + 40, y + 5 }, { x + 80, y - 12 }, { x + 80, y + 5 }, { 145, 35, 30, 255 });
    DrawLineEx({ x, y + 5 }, { x + 40, y - 12 }, 2.5f, { 245, 105, 90, 255 });
    DrawLineEx({ x + 40, y + 5 }, { x + 80, y - 12 }, 2.5f, { 185, 50, 40, 255 });

    // 6. Janelas Industriais Iluminadas (Duplo loop para garantir render estável)
    for (int i = 0; i < 2; i++) {
        float jX = x + 10 + (i * 20);
        float jY = y + 18;
        DrawRectangleGradientV((int)jX, (int)jY, 13, 12, { 40, 60, 80, 255 }, { 25, 35, 50, 255 });
        DrawRectangleLines((int)jX, (int)jY, 13, 12, { 90, 100, 110, 255 });
        DrawLine((int)jX + 6, (int)jY, (int)jX + 6, (int)jY + 12, { 90, 100, 110, 255 });
        DrawLineEx({ jX + 2, jY + 9 }, { jX + 10, jY + 2 }, 1.5f, Fade(SKYBLUE, 0.4f));
    }

    // 7. Letreiro Luminoso Ampliado "MALU QUEIJOS"
    int placaX       = (int)x + 2;
    int placaY       = (int)y - 8;
    int placaLargura = 76;
    int placaAltura  = 14;
    DrawRectangle(placaX, placaY, placaLargura, placaAltura, { 35, 30, 25, 255 });
    DrawRectangleLines(placaX, placaY, placaLargura, placaAltura, GOLD);

    const char* textoFabrica = "MALU QUEIJOS";
    int textoTamanhoFonte    = 10;
    int larguraTexto         = MeasureText(textoFabrica, textoTamanhoFonte);
    int textoX               = placaX + (placaLargura - larguraTexto) / 2;
    int textoY               = placaY + (placaAltura - textoTamanhoFonte) / 2;

    // Outline do Texto (Efeito de sombra robusta nas 4 direções)
    DrawText(textoFabrica, textoX - 1, textoY,     textoTamanhoFonte, BLACK);
    DrawText(textoFabrica, textoX + 1, textoY,     textoTamanhoFonte, BLACK);
    DrawText(textoFabrica, textoX,     textoY - 1, textoTamanhoFonte, BLACK);
    DrawText(textoFabrica, textoX,     textoY + 1, textoTamanhoFonte, BLACK);
    DrawText(textoFabrica, textoX,     textoY,     textoTamanhoFonte, GOLD);

    // 8. Logotipo de Queijo na Fachada
    float lX = x + 23;
    float lY = y + 42;
    DrawCircle((int)lX, (int)lY, 7, GOLD);
    DrawCircle((int)lX - 2, (int)lY - 2, 1.0f, AMARELO_SOMBRA);
    DrawCircle((int)lX + 2, (int)lY + 1, 1.2f, AMARELO_SOMBRA);
    DrawCircle((int)lX,     (int)lY + 3, 1.0f, AMARELO_SOMBRA);
    DrawTriangle({ lX, lY }, { lX + 8, lY - 3 }, { lX + 5, lY + 5 }, { 170, 170, 175, 255 }); // Furo triangular

    // 9. Porta de Carga + Esteira de Production Automática
    DrawRectangle((int)x + 52, (int)y + 25, 22, 35, { 50, 55, 60, 255 });
    DrawRectangle((int)x + 52, (int)y + 25, 22, 6, { 100, 105, 110, 255 });
    DrawRectangle((int)x + 50, (int)y + 52, 26, 5, DARKGRAY);
    DrawRectangle((int)x + 50, (int)y + 55, 26, 3, { 30, 30, 35, 255 });
    DrawRectangle((int)x + 58, (int)y + 46, 10, 7, GOLD); // Caixa amarela pronta
    DrawRectangleLines((int)x + 58, (int)y + 46, 10, 7, ORANGE);

    // 10. Luminária de Segurança (Glow de iluminação noturna)
    DrawCircle((int)x + 63, (int)y + 20, 2.0f, WHITE);
    DrawCircle((int)x + 63, (int)y + 20, 7, Fade(YELLOW, 0.35f));
}

// =========================================================================
// PIPELINE PRINCIPAL DE RENDERIZAÇÃO DE CENÁRIO
// =========================================================================

// Desenha a camada de fundo do chão do mapa, aplicando filtros climáticos e elementos decorativos fixos
void Cenario::DesenharFundo(Estacao estacao, Clima clima, ModoJogo modo, const std::vector<Vector2>& pocasLama) {
    Color corPastoAtual = VERDE_PASTO;
    Color corGramaAtual = VERDE_GRAMA;

    // Adaptação das cores do gramado dependendo da estação anual ativa
    if (estacao == VERAO)   { corPastoAtual = { 160, 220, 95, 255 };  corGramaAtual = { 110, 180, 50, 255 }; }
    if (estacao == OUTONO)  { corPastoAtual = { 200, 175, 95, 255 }; corGramaAtual = { 160, 110, 45, 255 }; }
    if (estacao == INVERNO) { corPastoAtual = { 135, 190, 180, 255 }; corGramaAtual = { 90, 150, 160, 255 }; }

    ClearBackground(corPastoAtual);

    // Filtros de Clima sobre a área de jogo principal (145x65 a 795x595)
    if (clima == SECA)    DrawRectangle(145, 65, 650, 530, Fade(ORANGE, 0.12f));
    if (clima == CHUVOSO) DrawRectangle(145, 65, 650, 530, Fade(BLUE, 0.15f));

    // Renderização Dinâmica de Poças de Lama (Geradas dinamicamente na chuva)
    if (clima == CHUVOSO) {
        for (const auto& poca : pocasLama) {
            DrawEllipse((int)poca.x, (int)poca.y, 25, 17, LAMA_POCOA);
            DrawEllipse((int)poca.x - 5, (int)poca.y + 2, 15, 12, {95, 65, 45, 255});
        }
    }

    // Painel HUD Lateral Esquerdo
    DrawRectangle(0, 0, 145, 600, { 115, 110, 100, 255 });
    DrawRectangle(135, 0, 10, 600, { 95, 90, 80, 255 });

    // Sombreamento interno das bordas das cercas limítrofes
    DrawRectangle(145, 65, 650, 10, { 0, 0, 0, 25 });
    DrawRectangle(145, 65, 12, 530, { 0, 0, 0, 25 });

    // Estradas e caminhos de terra batida estruturados
    DesenharCaminhoTerra(145, 130, 220, 130, 24);
    DesenharCaminhoTerra(145, 480, 250, 480, 26);
    DesenharCaminhoTerra(650, 530, 780, 530, 30);

    // Renderização de Tufos de Grama Decorativos
    DesenharTufoGrama(220, 160, corGramaAtual);
    DesenharTufoGrama(300, 250, corGramaAtual);
    DesenharTufoGrama(340, 520, corGramaAtual);
    DesenharTufoGrama(480, 220, corGramaAtual);
    DesenharTufoGrama(490, 440, corGramaAtual);
    DesenharTufoGrama(710, 140, corGramaAtual);
    DesenharTufoGrama(200, 360, corGramaAtual);
    DesenharTufoGrama(600, 480, corGramaAtual);

    // Renderização de Flores Silvestres
    DesenharFlores(250, 290, estacao);
    DesenharFlores(410, 470, estacao);
    DesenharFlores(520, 150, estacao);
    DesenharFlores(730, 320, estacao);
    DesenharFlores(320, 380, estacao);
    DesenharFlores(630, 180, estacao);
    DesenharFlores(550, 510, estacao);
    DesenharFlores(700, 440, estacao);

    // Renderização de Pedras de Detalhe de Mapa
    DesenharPedra(310, 210, 8);
    DesenharPedra(430, 510, 7);
    DesenharPedra(640, 350, 12);
    DesenharPedra(230, 410, 6);
    DesenharPedra(500, 110, 9);
    DesenharPedra(750, 220, 11);
    DesenharPedra(360, 310, 7);

    // Desenho das Entidades Estáticas de Produção (Fábrica)
    DesenharFabricaQueijo(25, 250);

    // Sistema de Árvores e Bosques (Superior Esquerdo, Topo e Direito)
    DesenharArvoreEstilizada(260, 120, estacao);
    DesenharArvoreEstilizada(330, 100, estacao);
    DesenharArvoreEstilizada(200, 160, estacao);
    DesenharArvoreEstilizada(450, 115, estacao);
    DesenharArvoreEstilizada(520, 105, estacao);
    DesenharArvoreEstilizada(610, 135, estacao);
    DesenharArvoreEstilizada(730, 140, estacao);
    DesenharArvoreEstilizada(275, 345, estacao);
    DesenharArvoreEstilizada(620, 280, estacao);
    DesenharArvoreEstilizada(380, 520, estacao);

    // Elemento Aquático Centralizado
    DesenharLago(lagoCentro, 55.0f, estacao);
}

// Renderiza os edifícios interativos e as cercas limítrofes colidíveis por cima das entidades básicas
void Cenario::DesenharConstrucoes(ModoJogo modo, Estacao estacao) {
    // Desenho dos Edifícios Estruturais
    DesenharCeleiro(590, 430);
    DesenharSilo(25, 85);
    DesenharTanque(25, 440);

    // Desenho do Quadrilátero de Cercas das Margens (Paredes invisíveis visuais)
    DesenharCercaH(145, 800, 65);
    DesenharCercaV(145, 65, 600);
    DesenharCercaV(790, 65, 600);
    DesenharCercaH(145, 800, 595);
}