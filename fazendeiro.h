#ifndef FAZENDEIRO_H
#define FAZENDEIRO_H

#include "raylib.h"  // Biblioteca gráfica para o tipo Vector2 (posição bidimensional)
#include "tipos.h"   // Enumerações do jogo, como 'Genero' e 'Estacao'

class Fazendeiro {
    // =========================================================================
    // ATRIBUTOS PRIVADOS (VARIÁVEIS DE ESTADO E INVENTÁRIO)
    // Dados controlados internamente pela classe para a segurança do estado do jogo
    // =========================================================================
private:
    // Física e Posicionamento
    Vector2 posicao;           // Coordenadas X e Y do fazendeiro no mapa do jogo
    float velocidade;          // Taxa de movimento (quantos pixels o personagem anda por frame/segundo)
    float raioColisao;         // Raio do círculo invisível em volta do jogador para checar colisões e alcance de interação

    // Inventário e Recursos
    float leiteNoBalde;        // Litros de leite coletados das vacas que ainda estão com o jogador
    int racaoNoInventario;     // Estoque atual de ração sendo carregado no bolso/mochila (máximo 10 unidades)

    // Customização e Estado Visual
    Genero generoAtual;        // Define o modelo visual (MASCULINO ou FEMININO) escolhido pelo jogador
    int estacaoAtual;          // Guarda a estação atual do ano (pode ser usado para trocar o sprite para roupas de frio/calor)


    // =========================================================================
    // MÉTODOS PÚBLICOS (INTERFACE DE CONTROLE DO PERSONAGEM)
    // Chamados no loop principal do jogo para dar vida ao personagem
    // =========================================================================
public:
    // Construtor: Inicializa a posição (X, Y) do nascimento e define o gênero (padrão é MASCULINO se não passar nada)
    Fazendeiro(float x, float y, Genero gen = MASCULINO);

    // Ciclo de Jogo (Atualização de Lógica e Renderização)
    void Atualizar();          // Lê as entradas do teclado/gamepad para mover o personagem e atualizar suas mecânicas
    void Desenhar(Estacao estacao, float escala = 1.0f); // Renderiza o sprite do fazendeiro na tela na escala correta


    // =========================================================================
    // MÉTODOS DE REGRA DE NEGÓCIO (AÇÕES E SETTERS)
    // Funções que alteram os estados internos de acordo com as ações do jogador
    // =========================================================================
public:
    // Gerenciamento de Leite
    void AdicionarLeite(float qtd); // Chamado ao ordenhar uma vaca; aumenta o leite no balde
    float EntregarLeite();          // Chamado no tanque de resfriamento; esvazia o balde e retorna a quantidade entregue para somar o dinheiro

    // Gerenciamento de Ração
    bool ReabastecerRacao();        // Chamado quando o jogador compra/pega mais ração no Silo. Retorna true se conseguiu reabastecer
    void UsarRacao();               // Chamado ao alimentar uma vaca; reduz em 1 a unidade carregada

    // Modificadores de Estado (Setters)
    void SetPosicao(float x, float y); // Força o fazendeiro a ir para uma posição específica (útil para dar spawn ou após cutscenes)
    void SetLeiteBalde(float qtd);     // Modifica diretamente a quantidade de leite do balde
    void SetGenero(Genero gen);        // Altera o gênero do personagem (caso haja um menu de customização em tempo real)


    // =========================================================================
    // MÉTODOS DE ACESSO (GETTERS - APENAS LEITURA)
    // Permitem que outras classes olhem os dados do fazendeiro sem risco de alterá-los diretamente
    // =========================================================================
public:
    Vector2 GetPosicao() const;        // Retorna a posição X e Y do jogador (usado pela câmera ou IA das vacas)
    float GetRaioColisao() const;     // Retorna o tamanho da área de colisão do jogador
    int GetRacao() const;              // Retorna a quantidade de ração na mochila (usado para desenhar a HUD)
    float GetLeiteBalde() const;       // Retorna quanto leite há no balde (usado para desenhar a HUD)
    Genero GetGenero() const;          // Retorna o gênero atual do fazendeiro
};

#endif // FAZENDEIRO_H