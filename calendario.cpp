#include "Calendario.h"

// =========================================================================
// CONSTRUTOR
// =========================================================================
Calendario::Calendario() {
    diaAtual = 1;
    tempoRestante = 60; // Começa com 60s (tempo padrão do Dia 1 no modo Desafio)
    estacaoAtual = PRIMAVERA;
    climaAtual = ENSOLARADO;
    corFiltroAtual = BLANK; // Sem filtro de cor inicial (tela limpa)
    primeiroFrame = true;
    acumuladorTempoReal = 0.0f;
}

// =========================================================================
// INICIALIZAÇÃO DE DIA
// =========================================================================
void Calendario::InicializarDia(ModoJogo modo) {
    acumuladorTempoReal = 0.0f; // Reseta o acumulador para evitar que o tempo do dia anterior influencie o novo

    //if (modo == DESAFIO) {
    //    if (diaAtual < 1) diaAtual = 1;
    //    tempoRestante = 60; // Força o tempo a ser exatamente 60 segundos no modo Desafio
    //
    if (modo == DESAFIO) {
        if (diaAtual < 1) diaAtual = 1;

        // CÁLCULO DINÂMICO: Começa com 60 e perde 5 segundos a cada novo dia
        // Dia 1: 60 - (0 * 5) = 60s
        // Dia 2: 60 - (1 * 5) = 55s
        // Dia 3: 60 - (2 * 5) = 50s...
        int tempoCalculado = 60 - ((diaAtual - 1) * 5);

        // TRAVA DE SEGURANÇA: Define um tempo mínimo (ex: 15 segundos)
        // para o jogo não ficar impossível ou com tempo negativo em dias avançados!
        if (tempoCalculado < 15) {
            tempoCalculado = 15;
        }

        tempoRestante = tempoCalculado;
    }else {
        tempoRestante = 120; // 120 segundos fixos no modo Diversão
    }
}

// =========================================================================
// CICLO DE ATUALIZAÇÃO (GAME LOOP)
// CORREÇÃO: Alterado de 'int' para 'ModoJogo' para manter a consistência com tipos.h
// =========================================================================
bool Calendario::Atualizar(ModoJogo modoAtual) {
    // Acumula o tempo que passou entre o frame anterior e o atual (Delta Time)
    acumuladorTempoReal += GetFrameTime();

    // Verificação de segundo em segundo real
    if (acumuladorTempoReal >= 1.0f) {
        if (tempoRestante > 0) {
            tempoRestante -= 1; // Deduz 1 segundo do timer do jogo
            if (tempoRestante < 0) tempoRestante = 0;
        }
        acumuladorTempoReal -= 1.0f; // Deduz 1 segundo inteiro, mantendo milissegundos restantes de sobra
    }

    // =========================================================================
    // LÓGICA DE INTERPOLAÇÃO DAS CORES DO CÉU (Exclusiva do Modo Diversão)
    // =========================================================================
    if (modoAtual == DIVERSAO) {
        // Calcula quantos segundos reais já se passaram desde o início da manhã
        int totalSegundosPassados = 120 - tempoRestante;

        // Se 120 segundos reais = 24 horas virtuais, cada hora virtual dura 5 segundos reais (120 / 24 = 5)
        int horaAtual = (totalSegundosPassados / 5) % 24;

        Color corFiltroAlvo = BLANK; // Por padrão, o filtro do céu é transparente (dia limpo)

        // --- TARDE / PÔR DO SOL ---
        // Das 16h às 17h virtuais (Segundos reais 80 a 85)
        if (horaAtual >= 16 && horaAtual < 17) {
            float progressoNoite = (float)(totalSegundosPassados - 80) / 5.0f;
            // Interpola suavemente para um tom alaranjado do entardecer (com opacidade máxima de 40%)
            corFiltroAlvo = Fade({ 230, 90, 40, 255 }, progressoNoite * 0.4f);
        }
        // --- TRANSIÇÃO PARA A NOITE ---
        // Das 17h às 19h virtuais (Segundos reais 85 a 95)
        else if (horaAtual >= 17 && horaAtual < 19) {
            float progressoNoite = (float)(totalSegundosPassados - 85) / 10.0f;
            Color baseLaranja = Fade({ 230, 90, 40, 255 }, 0.4f);
            Color baseNoite = Fade({ 15, 20, 45, 255 }, 0.75f);

            // Mistura manual dos canais RGBA do laranja para o azul escuro da noite
            corFiltroAlvo.r = baseLaranja.r + (baseNoite.r - baseLaranja.r) * progressoNoite;
            corFiltroAlvo.g = baseLaranja.g + (baseNoite.g - baseLaranja.g) * progressoNoite;
            corFiltroAlvo.b = baseLaranja.b + (baseNoite.b - baseLaranja.b) * progressoNoite;
            corFiltroAlvo.a = baseLaranja.a + (baseNoite.a - baseLaranja.a) * progressoNoite;
        }
        // --- NOITE FECHADA ---
        // Das 19h até as 05h da madrugada seguinte
        else if (horaAtual >= 19 || horaAtual < 5) {
            // Aplica um filtro azul escuro com 75% de opacidade para escurecer o mapa
            corFiltroAlvo = Fade({ 15, 20, 45, 255 }, 0.75f);
        }
        // --- AMANHECER (CUIDADO COM O TIMING!) ---
        // Das 05h às 06h da manhã (Segundos reais 25 a 30)
        else if (horaAtual >= 5 && horaAtual < 6) {
            float progressoDia = (float)(totalSegundosPassados - 25) / 5.0f;
            if (progressoDia > 1.0f) progressoDia = 1.0f;
            // Vai clareando a tela reduzindo a opacidade do filtro da noite a zero
            corFiltroAlvo = Fade({ 15, 20, 45, 255 }, 0.75f * (1.0f - progressoDia));
        }

        // --- APLICAÇÃO SUAVE DO FILTRO DE TELA (LERP) ---
        if (primeiroFrame) {
            corFiltroAtual = corFiltroAlvo; // No primeiro frame, aplica direto para evitar piscadas
            primeiroFrame = false;
        } else {
            // Aplica aproximação linear (Inércia visual) de 3% por frame para a transição não ser brusca
            float velocidadTransicao = 0.03f;
            corFiltroAtual.r = (unsigned char)(corFiltroAtual.r + (corFiltroAlvo.r - corFiltroAtual.r) * velocidadTransicao);
            corFiltroAtual.g = (unsigned char)(corFiltroAtual.g + (corFiltroAlvo.g - corFiltroAtual.g) * velocidadTransicao);
            corFiltroAtual.b = (unsigned char)(corFiltroAtual.b + (corFiltroAlvo.b - corFiltroAtual.b) * velocidadTransicao);
            corFiltroAtual.a = (unsigned char)(corFiltroAtual.a + (corFiltroAlvo.a - corFiltroAtual.a) * velocidadTransicao);
        }
    }

    // Se o cronômetro do dia zerou, avisa a classe Game para encerrar a rodada
    if (tempoRestante <= 0) {
        return true;
    }

    return false;
}

// =========================================================================
// TRADUTORES DE ENUM PARA STRING (Interface Gráfica / HUD)
// =========================================================================
std::string Calendario::GetTextoEstacao() const {
    switch (estacaoAtual) {
    case PRIMAVERA: return "PRIMAVERA";
    case VERAO:     return "VERAO";
    case OUTONO:    return "OUTONO";
    case INVERNO:   return "INVERNO";
    default:        return "DESCONHECIDO";
    }
}

std::string Calendario::GetTextoClima() const {
    switch (climaAtual) {
    case ENSOLARADO: return "ENSOLARADO";
    case SECA:       return "SECA";
    case CHUVOSO:    return "CHUVOSO";
    default:         return "DESCONHECIDO";
    }
}