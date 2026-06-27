#ifndef MINIGAME_ORDENHA_H
#define MINIGAME_ORDENHA_H

#include "raylib.h"    // Biblioteca gráfica para o tipo Vector2
#include <cstddef>     // Necessário para o tipo 'size_t' (índice do vetor)

class MinigameOrdenha {
    // =========================================================================
    // ATRIBUTOS PRIVADOS (VARIÁVEIS DE ESTADO DO MINIGAME)
    // Tudo o que a barra precisa para funcionar de forma independente
    // =========================================================================
private:
    bool   ativo;                  // Diz se a tela de ordenha está aparecendo e bloqueando o jogador
    float  barra;                  // Posição atual do ponteiro/cursor (varia de 0.0 a 100.0)
    float  velocidade;             // Ritmo de subida/descida do ponteiro (dificuldade)
    bool   subindo;                // Controla a direção do ponteiro (true = subindo, false = descendo)
    size_t indexVacaSendoOrdenhada;// Guarda qual vaca do vetor do rebanho disparou este minigame

    // =========================================================================
    // MÉTODOS PÚBLICOS (INTERFACE DE CONTROLE)
    // =========================================================================
public:
    // Construtor: Inicializa o minigame desligado (ativo = false) e zera a barra
    MinigameOrdenha();

    // Ativa o minigame e vincula a ele o índice da vaca que o jogador interagiu
    void Iniciar(size_t indexVaca);

    // CORREÇÃO: Nome ajustado para 'Atualizar()' para sanar o erro de compilação no game.cpp
    void Atualizar();

    // Renderiza a barra de acerto na tela (desenha a barra flutuando perto do jogador)
    void Desenhar(Vector2 posJogador);

    // =========================================================================
    // GETTERS E SETTERS (COMUNICAÇÃO COM A CLASSE GAME)
    // Permitem que o Game saiba quando o minigame acabou ou se o jogador acertou
    // =========================================================================
    bool   IsAtivo() const               { return ativo; }                  // O Game usa isso para congelar o movimento do jogador
    void   SetAtivo(bool estado)         { ativo = estado; }                // Permite fechar o minigame por fora
    float  GetBarra() const              { return barra; }                  // O Game lê isso para saber se o jogador acertou a zona dourada
    size_t GetIndexVaca() const          { return indexVacaSendoOrdenhada; } // Diz ao Game qual vaca deve ser modificada
};

#endif // MINIGAME_ORDENHA_H