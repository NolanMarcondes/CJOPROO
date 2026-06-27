#ifndef LOBO_H
#define LOBO_H

#include "raylib.h"   // Biblioteca gráfica para o tipo Vector2
#include "vaca.h"     // Necessário para a IA interagir e escanear a classe Vaca
#include "tipos.h"    // Enumerações globais, como 'Estacao'
#include <vector>     // Necessário para receber a lista de vacas (std::vector)

class Lobo {
    // =========================================================================
    // ATRIBUTOS PRIVADOS (VARIÁVEIS DE ESTADO E FÍSICA DA IA)
    // =========================================================================
private:
    Vector2 posicao;        // Coordenadas X e Y do lobo no plano 2D do jogo
    float velocidade;       // Velocidade de deslocamento (pixels por frame) quando estiver ativo
    float raioColisao;      // Raio do círculo invisível em volta do lobo (usado para morder a vaca ou ser espantado pelo jogador)
    bool ativo;             // Flag que define se o lobo está presente no mapa caçando ou se está "escondido/inativo"
    float direcaoX;         // Guarda a direção horizontal (-1 para esquerda, 1 para direita) para espelhar o sprite corretamente


    // =========================================================================
    // MÉTODOS PÚBLICOS (CONSTRUTORES)
    // =========================================================================
public:
    // Construtor padrão: Inicializa o lobo em um estado seguro (ativo = false, posições zeradas) para não quebrar o jogo no início
    Lobo();

    // Construtor com parâmetros: Cria o lobo diretamente em um local específico (útil para testes de spawn)
    Lobo(float x, float y);


    // =========================================================================
    // MÉTODOS DE CICLO DE VIDA E CORE ENGINE
    // =========================================================================
public:
    // Renderiza o sprite do lobo na tela (pode mudar a cor do pelo ou comportamento visual dependendo da Estacao)
    void Desenhar(Estacao estacao);

    // Atualiza estados internos gerais que rodam todo frame (ex: timers de animação, lógica de latido, etc.)
    void Atualizar();


    // =========================================================================
    // LÓGICA DE SPAWN E INTELIGÊNCIA ARTIFICIAL (IA)
    // =========================================================================
public:
    // Faz o lobo aparecer em uma borda do mapa com uma velocidade específica (que pode aumentar em dias mais avançados)
    void Spawnar(float x, float y, float vel);

    // Desliga a lógica do lobo e o remove visualmente do mapa (usado quando o dia acaba ou ele é afugentado)
    void Desativar();

    // Modifica diretamente as coordenadas do lobo (útil para teletransportá-lo ou ajustar o spawn fora da tela)
    void SetPosicao(float x, float y);

    // O coração da IA do inimigo: calcula a distância de todas as vacas do vetor, encontra a mais perto,
    // move-se em direção a ela e retorna 'true' se conseguir encostar no raio de colisão da vaca para atacá-la.
    bool PerseguirRebanho(std::vector<Vaca>& rebanho);


    // =========================================================================
    // MÉTODOS DE ACESSO (GETTERS - APENAS LEITURA)
    // Permitem que a classe principal (Game) consulte o estado do lobo de forma segura
    // =========================================================================
public:
    Vector2 GetPosicao() const;     // Retorna a posição atual do lobo (usado para o jogador saber a distância dele)
    float GetRaioColisao() const;   // Retorna o tamanho da área de perigo/ataque do lobo
    bool IsAtivo() const;           // Retorna se o lobo está caçando no momento (usado pelo Game para saber se deve rodar a IA)
};

#endif // LOBO_H