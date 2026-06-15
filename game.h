#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

// Definições de componentes e atores do projeto
#include "tipos.h"
#include "vaca.h"
#include "fazendeiro.h"
#include "lobo.h"
#include "cenario.h"
#include "Minigame_Ordenha.h"
#include "Calendario.h"

class Game {
    // =========================================================================
    // ATRIBUTOS PRIVADOS (VARIÁVEIS INTERNAS DO SISTEMA)
    // =========================================================================
private:
    // --- Configurações de Resolução e Renderização Virtual (Upscaling) ---
    const int larguraTela;                  // Largura física inicial da janela do sistema
    const int alturaTela;                   // Altura física inicial da janela do sistema
    int larguraJanelaAtual;                 // Controla a largura dinâmica ao redimensionar ou alternar Fullscreen
    int alturaJanelaAtual;                  // Controla a altura dinâmica ao redimensionar ou alternar Fullscreen
    const int LARGURA_VIRTUAL = 800;        // Resolução interna fixa para garantir proporção perfeita (Pixel Art/Lógica)
    const int ALTURA_VIRTUAL = 600;         // Resolução interna fixa para garantir proporção perfeita (Pixel Art/Lógica)
    RenderTexture2D telaVirtual;            // Canvas onde tudo é desenhado antes de ser esticado para a janela física

    // --- Máquina de Estados e Configurações Globais ---
    EstadoJogo estadoAtual;                 // Controla em qual tela o jogo está (MENU, JOGANDO, GAME_OVER, etc.)
    ModoJogo modoAtual;                     // Modo selecionado (DIVERSÃO sem punição ou DESAFIO com metas rígidas)
    Estacao estacaoAtual;                   // Estação do ano atual (Muda o visual do mapa e comportamento dos animais)
    Clima climaAtual;                       // Condição climática do dia (ENSOLARADO, SECA, CHUVOSO)
    Genero generoSelecionado;               // Gênero escolhido para o sprite do personagem principal
    MinigameOrdenha ordenhaMinigame;        // Instância da classe que gerencia a mecânica de ordenhar de forma encapsulada
    Calendario calendario;                  // Instância da classe que gerencia a passagem de tempo e dias do jogo

    // --- Progressão e Metas ---
    int contadorDiasNaEstacao;              // Controla quantos dias se passaram na estação atual para disparar a troca
    float leiteTotalEntregue;               // Quantidade acumulada de leite guardada no tanque central da fazenda
    float leiteAcumuladoGeral;              // Guarda o Histórico do leite do jogo todo
    float metaLeite;                        // Meta estipulada de entrega de leite necessária para vencer o dia
    int timerAparecerLobo;                  // Tempo restante em frames para o Lobo dar spawn no mapa

    // --- Sinalizadores de Controle (Flags de Estado) ---
    bool mostrandoLoja;                     // Define se o menu de compras/upgrades do celeiro deve interceptar a tela
    bool diaCompleto;                       // Indica se o jogador atingiu a meta de leite exigida para o dia
    bool tempoEsgotado;                     // Indica se o relógio chegou a zero antes de terminar as tarefas

    // --- Sistema de Economia e Upgrades ---
    int moedas;                             // Saldo atual de dinheiro disponível para compras
    int upVelocidade;                       // Nível do upgrade das botas do fazendeiro (melhora movimentação)
    int upCapacidade;                       // Nível do upgrade do balde (permite carregar mais leite de uma vez)

    // --- Ciclo de Iluminação e Efeitos Visuais ---
    Color corFiltroAtual;                   // Filtro de cor aplicado sobre a tela virtual para simular Entardecer/Noite
    bool primeiroFrameDoJogo;               // Evita que o jogo tente fazer transições bruscas de luz na primeira inicialização

    // --- Clima Dinâmico e Física de Deslizamento (Inércia) ---
    std::vector<Vector2> vetorPocasLama;    // Lista de posições de poças geradas aleatoriamente em dias de chuva
    const float RAIO_POÇA_LAMA;             // Tamanho da área de colisão que reduz a velocidade do fazendeiro
    Vector2 velResidualJogador;             // Efeito de derrapagem/inércia aplicado ao jogador sobre o lago congelado no Inverno
    std::vector<Vector2> velResidualVacas;  // Vetor que guarda a derrapagem individual de cada vaca no gelo

    // --- Sistema de Feedback Visual (Mensagens na HUD) ---
    std::string mensagemFeedback;           // Texto temporário exibido na tela (ex: "Balde Cheio!", "Ração Esgotada!")
    int timerFeedback;                      // Duração em frames que a mensagem continuará visível antes de sumir
    Color corFeedback;                      // Cor associada à mensagem para indicar sucesso, alerta ou erro


    // =========================================================================
    // ENTIDADES / ATORES DO JOGO (Objetos instanciados no mapa)
    // =========================================================================
private:
    Fazendeiro jogador;                     // O herói/fazendeiro controlado diretamente por você
    Lobo lobo;                              // O inimigo que tenta atacar as vacas
    std::vector<Vaca> rebanho;              // Lista dinâmica que gerencia e renderiza todas as vacas ativas
    Cenario mapa;                           // O mundo do jogo (chão, construções, lago e estradas)


    // =========================================================================
    // MÉTODOS AUXILIARES E CORE INTERNO
    // =========================================================================
private:
    void ResetarRebanho();                  // Restaura a vida, posições e estados iniciais de todas as vacas
    void InicializarDia(ModoJogo modo);     // Sorteia clima, limpa poças do dia anterior e aplica regras da manhã
    void ResetarJogoCompleto();             // Zera moedas, upgrades e dias (usado ao morrer ou dar New Game)
    void ProcessarEntradas();               // Captura cliques, teclas de movimento e botões de interação
    void AtualizarLogica();                 // Processa IA das vacas, colisões, movimento do lobo e regras de fim de dia
    void RenderizarTela();                  // Direciona o fluxo de desenho para a tela virtual correta

    // Sub-rotinas de desenho específicas para cada estado de tela
    void DesenharMenu();                    // Menu principal
    void DesenharSelecaoPersonagem();       // Escolha de gênero/visual
    void DesenharTutorial();                // Tela de instruções básicas
    void DesenharGameplay();                // Renderiza o mapa, atores, filtros climáticos e a HUD
    void DesenharGameOver();                // Tela de derrota ou vitória definitiva


    // =========================================================================
    // MÉTODOS PÚBLICOS (INTERFACE EXTERNA)
    // =========================================================================
public:
    Game();                                 // Construtor: Inicializa a Raylib, aloca janelas e configura a semente randômica
    ~Game();                                // Destrutor: Garante a liberação de memória e encerramento seguro do motor gráfico

    void Executar();                        // O coração do projeto. Roda o loop principal (Update/Draw) continuamente
};

#endif // GAME_H