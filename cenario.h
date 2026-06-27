#ifndef CENARIO_H
#define CENARIO_H

#include "raylib.h"   // Biblioteca gráfica para tipos como Color e Vector2
#include <vector>     // Necessário para usar std::vector (na lista de poças de lama)
#include "tipos.h"    // Enumerações do jogo como Estacao, Clima e ModoJogo

class Cenario {
    // =========================================================================
    // ATRIBUTOS PRIVADOS (VARIÁVEIS INTERNAS)
    // =========================================================================
private:
    // Paleta de Cores do Cenário (Centraliza as cores para facilitar mudanças de arte)
    Color VERDE_PASTO;        // Cor base para o gramado principal da fazenda
    Color VERDE_GRAMA;        // Cor usada em detalhes, como tufos de grama espalhados
    Color SOMBRA_REFRACAO;    // Tom translúcido para efeitos visuais na água ou gelo do lago
    Color CELEIRO_VERMELHO;   // Cor predominante das paredes do celeiro (estilo clássico)
    Color CELEIRO_ESCURO;     // Tom mais escuro para as sombras nas paredes e teto do celeiro
    Color CELEIRO_LUZ;        // Tom mais claro para simular a luz batendo nas quinas do celeiro
    Color AMARELO_MILHO;      // Cor vibrante para a ração estocada visível no silo
    Color AMARELO_SOMBRA;     // Tom mais fosco/escuro para a sombra nos estoques de comida
    Color MADEIRA_CERCA;      // Cor marrom para os postes de madeira que delimitam o cenário
    Color TERRA_BATIDA;       // Cor para as estradas e caminhos onde o jogador caminha
    Color TERRA_ESCURA;       // Usada para canteiros, terra arada ou áreas úmidas do mapa
    Color LAMA_POCOA;         // Cor marrom-escura para desenhar as poças quando estiver chovendo

    // Propriedades do Lago do Pasto (Usadas para renderização e cálculos de limite)
    Vector2 lagoCentro;       // Posição X e Y do centro do lago no mapa
    float lagoRaio;           // Raio do lago (define o tamanho do círculo)


    // =========================================================================
    // MÉTODOS PRIVADOS (AUXILIARES DE DESENHO INTERNO)
    // Esses métodos só podem ser chamados pela própria classe Cenário para montar o mapa
    // =========================================================================
private:
    // Elementos da Natureza e Terreno
    void DesenharLago(Vector2 centro, float raio, Estacao estacao); // Desenha o lago (muda o visual se estiver congelado no Inverno)
    void DesenharTufoGrama(float x, float y, Color corGrama);       // Desenha pequenos detalhes de grama para quebrar o visual plano
    void DesenharFlores(float x, float y, Estacao estacao);         // Desenha flores (nascem na Primavera, somem no Inverno)
    void DesenharPedra(float x, float y, float raio);               // Desenha pedras decorativas pelo mapa
    void DesenharCaminhoTerra(float x1, float y1, float x2, float y2, float largura); // Desenha as estradas conectando os pontos
    void DesenharArvoreEstilizada(float x, float y, Estacao estacao); // Desenha árvores (folhas verdes, laranjas ou sem folhas conforme a estação)

    // Estruturas e Delimitações do Mapa
    void DesenharCercaH(float x1, float x2, float y); // Desenha uma linha de cerca na horizontal
    void DesenharCercaV(float x, float y1, float y2); // Desenha uma linha de cerca na vertical

    // Edifícios Interativos e Prédios da Fazenda
    void DesenharSilo(float x, float y);          // Desenha a torre do silo de comida
    void DesenharTanque(float x, float y);        // Desenha o tanque de resfriamento de leite
    void DesenharCeleiro(float x, float y);       // Desenha o edifício principal do celeiro
    void DesenharFabricaQueijo(float x, float y);  // Desenha a estação de artesanato/produção de queijo


    // =========================================================================
    // MÉTODOS PÚBLICOS (INTERFACE COM A CLASSE GAME)
    // Funções que o motor principal do jogo chama no loop de renderização
    // =========================================================================
public:
    // Construtor: Inicializa as variáveis de cor e define onde o lago e estruturas fixas vão ficar
    Cenario();

    // Renderiza a camada de baixo (Chão, estradas, lago, flores e as poças de lama dinâmicas passadas por vetor)
    void DesenharFundo(Estacao estacao, Clima clima, ModoJogo modo, const std::vector<Vector2>& pocasLama);

    // Renderiza a camada de cima (Prédios e cercas). Separado do fundo para permitir que personagens passem atrás das construções
    void DesenharConstrucoes(ModoJogo modo, Estacao estacao);

    // Desenha aquela elipse escura clássica de jogos 2D embaixo de objetos e personagens para dar senso de profundidade
    void DesenharSombraProjetada(float x, float y, float largura, float altura);
};

#endif // CENARIO_H