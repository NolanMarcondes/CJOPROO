#ifndef VACA_H
#define VACA_H

#include "raylib.h"  // Biblioteca gráfica para o tipo Vector2
#include "tipos.h"   // Enumerações globais do jogo, como 'Estacao'

class Vaca {
    // =========================================================================
    // ATRIBUTOS PRIVADOS (ESTADO, FÍSICA E ATRIBUTOS DA VACA)
    // Dados internos que controlam o comportamento individual de cada animal
    // =========================================================================
private:
    // Física e Movimentação
    Vector2 posicao;            // Coordenadas X e Y da vaca no plano 2D do pasto
    float velocidade;           // Velocidade de deslocamento (quantos pixels ela anda por frame)
    float raioColisao;          // Área circular em volta da vaca para checar se o jogador encostou ou se o lobo a atacou
    int tempoMudarDirecao;      // Contador de frames regressivo; quando chega a zero, a vaca escolhe um novo rumo para andar
    Vector2 direcaoAtual;       // Vetor unitário (X, Y) que dita para onde a vaca está caminhando no momento

    // Necessidades e Produção
    float nivelFome;            // Variável que aumenta com o tempo. Se ficar muito alta, a vaca para de produzir leite
    float nivelLeite;           // Quantidade de leite acumulada na teta da vaca, pronta para ser coletada
    Estacao estacaoAtual;       // Guarda a estação do ano (útil para mudar a velocidade ou o visual do sprite no inverno)


    // =========================================================================
    // MÉTODOS PÚBLICOS (CONSTRUTOR E CICLO DE JOGO)
    // Funções principais chamadas pelo gerenciador do rebanho na classe Game
    // =========================================================================
public:
    // Construtor: Coloca a vaca em uma coordenada (X,Y) inicial no mapa e zera seus status de leite e fome
    Vaca(float x, float y);

    // Executa a IA de movimentação aleatória, checa os cronômetros e aumenta os níveis de fome e leite a cada frame
    void Atualizar(Estacao estacaoAtual);

    // Renderiza o sprite da vaca na tela (pode desenhar ela mastigando, dormindo ou com visual diferente conforme a estação)
    void Desenhar(Estacao estacao);


    // =========================================================================
    // MÉTODOS DE REGRA DE NEGÓCIO (AÇÕES E MODIFICADORES)
    // Executam as mecânicas de interação direta com o jogador
    // =========================================================================
public:
    // Interações com o Fazendeiro
    void Alimentar();           // Reduz o 'nivelFome' para zero. Chamado quando o jogador usa ração perto do animal
    float Ordenhar();           // Retorna a quantidade de 'nivelLeite' atual para o balde do jogador e zera o estoque da vaca

    // Modificadores de Estado (Setters)
    void SetPosicao(float x, float y); // Força a vaca a ir para um ponto (útil para empurrá-la para fora de obstáculos ou cercas)
    void SetNivelLeite(float novoNivel) { nivelLeite = novoNivel; } // Ajusta diretamente o leite interno (útil para efeitos ou bônus)


    // =========================================================================
    // MÉTODOS DE ACESSO (GETTERS - APENAS LEITURA)
    // Permitem que a classe Game ou a interface da HUD vejam o estado da vaca com segurança
    // =========================================================================
public:
    Vector2 GetPosicao() const;     // Retorna a localização da vaca (usado pela IA do lobo para perseguição)
    float GetRaioColisao() const;   // Retorna o tamanho do círculo de colisão do animal
    float GetNivelLeite() const;    // Retorna a quantidade de leite disponível (usado para desenhar uma mini barra flutuante)
    float GetNivelFome() const;     // Retorna o nível de fome atual (usado para saber se ela precisa comer)
};

#endif // VACA_H