#ifndef CALENDARIO_H
#define CALENDARIO_H

#include "raylib.h"   // Biblioteca gráfica (usada aqui para o tipo 'Color')
#include <string>     // Usado para manipulação de textos (std::string)
#include "tipos.h"    // Contém as definições de 'Estacao' e 'Clima' (provavelmente enums)

class Calendario {
private:
    // === ATRIBUTOS PRIVADOS (Dados internos da classe) ===
    int diaAtual;             // Armazena o número do dia em que o jogador está (ex: Dia 1, Dia 2)
    int tempoRestante;        // Contador de tempo para o fim do dia atual (em segundos ou frames)
    Estacao estacaoAtual;     // Guarda a estação do ano (ex: Primavera, Verão) vinda de 'tipos.h'
    Clima climaAtual;         // Guarda o clima do dia (ex: Ensolarado, Chuvoso) vindo de 'tipos.h'
    Color corFiltroAtual;     // Cor usada para aplicar um filtro na tela (ex: escurecer à noite ou avermelhar no entardecer)
    bool primeiroFrame;       // Flag para verificar se é o primeiro frame do dia (útil para setups iniciais)
    float acumuladorTempoReal;// Variável para contar o tempo que passou no mundo real e atualizar o relógio do jogo

public:
    // === CONSTRUTOR ===
    Calendario();             // Inicializa a classe com valores padrão (zera o dia, define clima inicial, etc.)

    // === MÉTODOS PRINCIPAIS ===
    void InicializarDia(ModoJogo modo); // Configura o dia que está começando com base no modo de jogo atual
    bool Atualizar(ModoJogo modoAtual);      // Atualiza o tempo a cada frame. Retorna true se o dia acabou

    // === GETTERS E SETTERS (Acesso seguro às variáveis privadas) ===

    // Controle do número do dia
    int GetDiaAtual() const { return diaAtual; }
    void SetDiaAtual(int dia) { diaAtual = dia; }

    // Controle do relógio/tempo do dia
    int GetTempoRestante() const { return tempoRestante; }
    void SetTempoRestante(int tempo) { tempoRestante = tempo; }

    // Controle da estação do ano
    Estacao GetEstacaoAtual() const { return estacaoAtual; }
    void SetEstacaoAtual(Estacao est) { estacaoAtual = est; }

    // Controle do clima
    Clima GetClimaAtual() const { return climaAtual; }
    void SetClimaAtual(Clima clm) { climaAtual = clm; }

    // Retorna a cor do filtro visual (apenas leitura, pois o filtro muda internamente no 'Atualizar')
    Color GetCorFiltroAtual() const { return corFiltroAtual; }

    // Controle da flag de primeiro frame
    bool IsPrimeiroFrame() const { return primeiroFrame; }
    void SetPrimeiroFrame(bool estado) { primeiroFrame = estado; }

    // === MÉTODOS AUXILIARES DE TEXTO ===
    // Convertem os enums (Estacao e Clima) em texto formatado para mostrar na tela (HUD) do jogador
    std::string GetTextoEstacao() const; // Ex: Transforma ESTACAO_VERAO em "Verão"
    std::string GetTextoClima() const;   // Ex: Transforma CLIMA_CHUVA em "Chuvoso"
};

#endif // CALENDARIO_H