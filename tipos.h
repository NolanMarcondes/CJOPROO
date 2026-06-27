#ifndef TIPOS_H
#define TIPOS_H

// =========================================================================
// MUNDO E MEIO AMBIENTE
// Define as variáveis globais de ambiente que mudam o comportamento do mapa
// =========================================================================

// Controla o ciclo anual do jogo, afetando o visual do cenário e comportamentos físicos
enum Estacao {
    PRIMAVERA,       // Estação das flores (Cenário desenha flores nascendo, vacas felizes)
    VERAO,           // Estação padrão quente (Visual limpo, comportamento padrão)
    OUTONO,          // Estação de transição (Mudança nas cores das folhas das árvores do Cenário)
    INVERNO,         // Ativa o gelo no lago e mecânicas de deslizamento/inércia para o Fazendeiro e Vacas
    TOTAL_ESTACOES   // Marcador técnico: Usado em loops ou em `GetRandomValue(0, TOTAL_ESTACOES - 1)` para sortear estações
};

// Define as condições meteorológicas diárias e a geração de obstáculos
enum Clima {
    ENSOLARADO,      // Clima limpo padrão (Sem penalidades ou filtros pesados de tela)
    SECA,            // Reduz ou altera parâmetros de recursos (ex: vacas podem dar menos leite ou comer mais)
    CHUVOSO,         // Ativa a renderização visual de chuva, escurece o filtro de luz e gera o vetor de poças de lama no mapa
    TOTAL_CLIMAS     // Marcador técnico: Usado para contar quantos climas existem e sortear o clima do dia aleatoriamente
};


// =========================================================================
// CONFIGURAÇÕES E CUSTOMIZAÇÃO DO JOGADOR
// Modificadores de regras e estética controlados pelo usuário
// =========================================================================

// Dita as regras, tempos e condições de vitória/derrota da partida
enum ModoJogo {
    DIVERSAO,        // Modo livre: Sem Game Over por tempo. Focado em acumular moedas, comprar upgrades e cuidar da fazenda de forma relaxante
    DESAFIO          // Modo hardcore: Metas de leite altas no tanque, tempo de dia curto, IA do lobo agressiva e Minigame de Ordenha ativo
};

// Define o sprite e a identidade visual que o personagem do jogador usará
enum Genero {
    MASCULINO,       // Aplica as texturas/sprites do Fazendeiro homem na classe Fazendeiro
    FEMININO         // Aplica as texturas/sprites da Fazendeira mulher na classe Fazendeiro
};


// =========================================================================
// MÁQUINA DE ESTADOS (FLUXO DO JOGO)
// Usado pela classe Game para gerenciar qual tela do loop principal deve rodar
// =========================================================================

// Controla qual tela/lógica deve ser processada e renderizada pelo motor principal
enum EstadoJogo {
    TELA_MENU,                  // Renderiza a abertura do jogo, título e seleção de ModoJogo (Diversão/Desafio)
    TELA_SELECAO_PERSONAGEM,    // Renderiza a tela de escolha de Genero (Masculino/Feminino) antes de dar o spawn
    TELA_TUTORIAL,              // Exibe uma tela estática ou explicativa com os comandos do teclado e mecânicas básicas
    JOGANDO,                    // O estado principal do loop: Atualiza a física do Fazendeiro, IA das Vacas, perseguição do Lobo e o Calendário
    GAME_OVER                   // Intercepta o desenho do jogo para mostrar a tela de fim de jogo caso o jogador falhe na meta no modo Desafio
};

#endif // TIPOS_H