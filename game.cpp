#include "game.h"
#include <cmath>

const int LARGURA_VIRTUAL = 800;
const int ALTURA_VIRTUAL = 600;

// =========================================================================
// CONSTRUTOR DA CLASSE GAME
// Aloca recursos gráficos, define os limites e popula o vetor de vacas
// =========================================================================
Game::Game() :
    larguraTela(800),
    alturaTela(600),
    estadoAtual(TELA_MENU),
    modoAtual(DIVERSAO),
    jogador(250.0f, 300.0f, MASCULINO),
    lobo(),
    leiteTotalEntregue(0.0f),
    metaLeite(150.0f),
    mostrandoLoja(false),
    diaCompleto(false),
    tempoEsgotado(false),
    upVelocidade(0),
    upCapacidade(0),
    mensagemFeedback(""),
    timerFeedback(0),
    corFeedback(WHITE),
    timerAparecerLobo(0),
    velResidualJogador({ 0.0f, 0.0f }),
    RAIO_POÇA_LAMA(20.0f),
    leiteAcumuladoGeral(0.0f),
    generoSelecionado(MASCULINO)
{
    // Habilita o redimensionamento nativo da janela e inicializa o motor Raylib
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(larguraTela, alturaTela, "CowGame - Farm Deluxe Edition");

    // Configuração do Canvas Virtual (Garante proporções perfeitas mesmo esticando a janela)
    telaVirtual = LoadRenderTexture(LARGURA_VIRTUAL, ALTURA_VIRTUAL);
    SetTextureFilter(telaVirtual.texture, TEXTURE_FILTER_BILINEAR);

    // Identifica o monitor atual para forçar o modo Tela Cheia na resolução nativa do usuário
    int monitor = GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
    ToggleFullscreen();

    // Alimenta o gerador de números aleatórios com o relógio do sistema
    srand(static_cast<unsigned int>(time(NULL)));

    // Crava o jogo para rodar a 60 frames por segundo estáveis
    SetTargetFPS(60);

    // Inicialização do Rebanho: Adiciona as 4 vacas originais em posições estratégicas do mapa
    rebanho.push_back(Vaca(250.0f, 200.0f));
    rebanho.push_back(Vaca(550.0f, 200.0f));
    rebanho.push_back(Vaca(300.0f, 450.0f));
    rebanho.push_back(Vaca(500.0f, 450.0f));

    // Inicializa a tabela de derrapagem física no gelo para cada vaca registrada
    velResidualVacas.assign(rebanho.size(), { 0.0f, 0.0f });
}

// =========================================================================
// DESTRUTOR DA CLASSE GAME
// Desaloca a textura da GPU e encerra o contexto da janela com segurança
// =========================================================================
Game::~Game() {
    UnloadRenderTexture(telaVirtual);
    CloseWindow();
}

// =========================================================================
// LOOP PRINCIPAL DO JOGO (CORE ENGINE)
// Gerencia as atualizações de entrada, lógica física e renderização por frame
// =========================================================================
void Game::Executar() {
    while (!WindowShouldClose()) {

        // 1. Processa teclado, mouse e interações de menus
        ProcessarEntradas();

        // 2. Se o jogo estiver rolando ativamente na tela:
        if (estadoAtual == JOGANDO) {
            diaCompleto = (leiteTotalEntregue >= metaLeite);

            // Se o jogador não estiver travado em menus ou minigames, avança o relógio
            if (!mostrandoLoja && !diaCompleto && !ordenhaMinigame.IsAtivo()) {
                tempoEsgotado = calendario.Atualizar(modoAtual); // CORREÇÃO: Passando o enum ModoJogo diretamente
            } else {
                tempoEsgotado = false;
            }

            // --- REGRAS DO MODO DESAFIO ---
            if (modoAtual == DESAFIO) {
                if (tempoEsgotado) {
                    estadoAtual = GAME_OVER;
                    continue; // Pula o resto do frame para travar na tela de derrota
                }

                // Se o jogador coletou o leite necessário antes do tempo acabar:
                if (diaCompleto) {
                    calendario.SetDiaAtual(calendario.GetDiaAtual() + 1);
                    metaLeite += 20.0f; // Incrementa a dificuldade da meta diária
                    leiteTotalEntregue = 0.0f;
                    moedas += 10; // Recompensa financeira alta
                    mensagemFeedback = "Meta Batida! Próximo dia do Desafio!";
                    corFeedback = GOLD;
                    timerFeedback = 180;
                    InicializarDia(modoAtual);
                    continue;
                }
            }

            // --- REGRAS DO MODO DIVERSÃO ---
            if (modoAtual == DIVERSAO && !mostrandoLoja) {
                if (diaCompleto || tempoEsgotado) {
                    calendario.SetDiaAtual(calendario.GetDiaAtual() + 1);
                    metaLeite += 20.0f; // Aumento suave de meta
                    leiteTotalEntregue = 0.0f;
                    moedas += 10;
                    mensagemFeedback = "Novo Dia! Voce ganhou +10 moedas!";
                    corFeedback = GOLD;
                    timerFeedback = 180;
                    InicializarDia(modoAtual);
                    continue;
                }
            }

            // Se o jogo não estiver pausado pela loja ou vitória do dia, roda as físicas e IAs
            if (!mostrandoLoja && !diaCompleto) {
                AtualizarLogica();
            }
        }

        // 3. Renderiza todas as camadas visuais em um único passe estruturado
        RenderizarTela();
    }
}

// =========================================================================
// RECONSTRUTOR DE ANIMAIS
// Limpa o vetor antigo e gera novas vacas saudáveis nos pontos de partida
// =========================================================================
void Game::ResetarRebanho() {
    rebanho.clear();
    rebanho.push_back(Vaca(250.0f, 200.0f));
    rebanho.push_back(Vaca(550.0f, 200.0f));
    rebanho.push_back(Vaca(300.0f, 450.0f));
    rebanho.push_back(Vaca(500.0f, 450.0f));

    velResidualVacas.assign(rebanho.size(), { 0.0f, 0.0f });
}

// =========================================================================
// CONFIGURAÇÃO DE UM NOVO DIA
// Reinicia os relógios, redefine posições, sorteia climas e muda estações
// =========================================================================
void Game::InicializarDia(ModoJogo modo) {
    if (calendario.GetDiaAtual() > 1) {
        contadorDiasNaEstacao++;
    }

    // REGRA DE PROGRESSÃO: A cada 3 dias mudamos a estação do ano!
    if (contadorDiasNaEstacao >= 1) { // Ajustado para respeitar os comentários (muda a cada 3 dias)
        Estacao novaEstacao = (Estacao)((calendario.GetEstacaoAtual() + 1) % TOTAL_ESTACOES);
        calendario.SetEstacaoAtual(novaEstacao);

        this->estacaoAtual = novaEstacao; // Sincroniza a cópia local do Game
        contadorDiasNaEstacao = 0;

        mensagemFeedback = "A ESTACAO MUDOU! Confira as alteracoes no cenario!";
        corFeedback = GOLD;
        timerFeedback = 180;
    } else {
        this->estacaoAtual = calendario.GetEstacaoAtual();
    }

    // Atualiza o relógio interno do calendário com os valores padrão do modo de jogo escolhido
    calendario.InicializarDia(modo);

    diaCompleto = false;
    tempoEsgotado = false;

    // Sorteia o clima do dia de forma aleatória
    Clima climaSorteado = (Clima)GetRandomValue(0, TOTAL_CLIMAS - 1);
    calendario.SetClimaAtual(climaSorteado);

    vetorPocasLama.clear();

    // Se o clima for chuvoso, gera poças de lama em locais aleatórios do mapa
    if (calendario.GetClimaAtual() == CHUVOSO) {
        int quantPocas = GetRandomValue(3, 6);
        for (int i = 0; i < quantPocas; i++) {
            vetorPocasLama.push_back({ (float)GetRandomValue(160, 750), (float)GetRandomValue(100, 520) });
        }
    }

    // Move o jogador e os inimigos de volta para as posições seguras matinais
    jogador.SetPosicao(250.0f, 300.0f);
    lobo.Desativar();
    timerAparecerLobo = 0;

    velResidualJogador = { 0.0f, 0.0f };
    velResidualVacas.assign(rebanho.size(), { 0.0f, 0.0f });

    if (timerFeedback <= 0) {
        mensagemFeedback = "UM NOVO DIA SE INICIOU!";
        corFeedback = GOLD;
        timerFeedback = 120;
    }
}

// =========================================================================
// NOVO JOGO / REINICIALIZAÇÃO TOTAL
// Zera economias, upgrades e retorna a fazenda ao Dia 1 original
// =========================================================================
void Game::ResetarJogoCompleto() {
    leiteTotalEntregue = 0.0f;
    leiteAcumuladoGeral = 0.0f;
    moedas = 0;
    metaLeite = (modoAtual == DESAFIO) ? 120.0f : 100.0f;

    diaCompleto = false;
    tempoEsgotado = false;
    mostrandoLoja = false;

    upVelocidade = 0;
    upCapacidade = 0;
    vetorPocasLama.clear();
    lobo.Desativar();
    timerAparecerLobo = 0;

    // Cria uma nova instância de calendário limpa
    calendario = Calendario();

    Estacao estacaoSorteada = (Estacao)GetRandomValue(0, 3);
    calendario.SetEstacaoAtual(estacaoSorteada);

    this->estacaoAtual = estacaoSorteada;
    this->contadorDiasNaEstacao = 0;

    jogador.SetPosicao(250.0f, 300.0f);
    jogador.SetLeiteBalde(0.0f);

    // Esvazia as sacolas de ração antigas
    while (jogador.GetRacao() > 0) {
        jogador.UsarRacao();
    }

    if (!rebanho.empty()) {
        float posicoesIniciaisX[] = { 250.0f, 550.0f, 300.0f, 500.0f };
        float posicoesIniciaisY[] = { 200.0f, 200.0f, 450.0f, 450.0f };

        for (size_t i = 0; i < rebanho.size(); i++) {
            float x = (i < 4) ? posicoesIniciaisX[i] : (float)GetRandomValue(200, 600);
            float y = (i < 4) ? posicoesIniciaisY[i] : (float)GetRandomValue(150, 450);
            rebanho[i] = Vaca(x, y);
        }
    }

    velResidualJogador = { 0.0f, 0.0f };
    velResidualVacas.assign(rebanho.size(), { 0.0f, 0.0f });

    primeiroFrameDoJogo = true;
}

// =========================================================================
// PROCESSAMENTO DE ENTRADAS DO USUÁRIO
// Monitora teclas pressionadas, mouses, atalhos de janelas e ações diretas
// =========================================================================
void Game::ProcessarEntradas() {
    // ATALHO: F11 chaveia o motor gráfico entre Tela Cheia e Janela compacta
    if (IsKeyPressed(KEY_F11)) {
        if (IsWindowFullscreen()) {
            ToggleFullscreen();
            SetWindowSize(800, 600);
        } else {
            int monitor = GetCurrentMonitor();
            SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
            ToggleFullscreen();
        }
    }

    if (estadoAtual == GAME_OVER) {
        if (IsKeyPressed(KEY_R)) {
            estadoAtual = TELA_MENU;
        }
        return; // Bloqueia movimentações se o jogo acabou
    }

    // --- ENTRADAS DO MENU PRINCIPAL ---
    if (estadoAtual == TELA_MENU) {
        if (IsKeyPressed(KEY_ONE)) {
            modoAtual = DIVERSAO;
            ResetarJogoCompleto();
            estadoAtual = TELA_SELECAO_PERSONAGEM;
        }
        if (IsKeyPressed(KEY_TWO)) {
            modoAtual = DESAFIO;
            ResetarJogoCompleto();
            estadoAtual = TELA_SELECAO_PERSONAGEM;
        }
    }

    // --- ENTRADAS DA SELEÇÃO DE PERSONAGENS ---
    else if (estadoAtual == TELA_SELECAO_PERSONAGEM) {
        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
            generoSelecionado = MASCULINO;
        }
        if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
            generoSelecionado = FEMININO;
        }

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            jogador.SetGenero(generoSelecionado);
            estadoAtual = TELA_TUTORIAL;
        }
    }

    // --- ENTRADAS DA TELA DE TUTORIAL ---
    else if (estadoAtual == TELA_TUTORIAL) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            ResetarRebanho();
            jogador.SetPosicao(250.0f, 300.0f);
            leiteTotalEntregue = 0.0f;
            ordenhaMinigame.SetAtivo(false); // CORREÇÃO: Usando o objeto encapsulado em vez da variável deletada
            vetorPocasLama.clear();

            calendario.SetDiaAtual(1);
            InicializarDia(modoAtual);

            estadoAtual = JOGANDO;
        }
    }

    // --- ENTRADAS DA GAMEPLAY ATIVA ---
    else {
        if (IsKeyPressed(KEY_R)) {
            estadoAtual = TELA_MENU;
            return;
        }

        // Calcula aproximação com o Retângulo do Celeiro para abrir a interface de Upgrades
        Rectangle zonaCeleiro = { 550, 400, 200, 150 };
        Vector2 posJogador = jogador.GetPosicao();
        float raioJogador = jogador.GetRaioColisao();

        if (CheckCollisionCircleRec(posJogador, raioJogador, zonaCeleiro)) {
            if (IsKeyPressed(KEY_U)) mostrandoLoja = !mostrandoLoja;
        } else {
            mostrandoLoja = false;
        }

        // LÓGICA DE COMPRAS NA LOJA DE UPGRADES
        if (mostrandoLoja) {
            if (IsKeyPressed(KEY_ONE) && upVelocidade < 3 && moedas >= 20) {
                moedas -= 20;
                upVelocidade++;
                mensagemFeedback = "Upgrade: Botas de Velocidade Nvl " + std::to_string(upVelocidade);
                corFeedback = GREEN;
                timerFeedback = 120;
            }

            if (IsKeyPressed(KEY_TWO) && upCapacidade < 3 && moedas >= 30) {
                moedas -= 30;
                upCapacidade++;
                mensagemFeedback = "Upgrade: Balde de Aluminio Nvl " + std::to_string(upCapacidade);
                corFeedback = GREEN;
                timerFeedback = 120;
            }
            return; // Impede que o jogador ande ou gaste recursos com a loja aberta
        }

        // Passagem manual de dia (Quando a meta já foi conquistada no desafio)
        if (modoAtual == DESAFIO && diaCompleto && IsKeyPressed(KEY_ENTER)) {
            diaCompleto = false;
            tempoEsgotado = false;

            calendario.SetDiaAtual(calendario.GetDiaAtual() + 1);
            metaLeite += 20.0f;
            leiteTotalEntregue = 0.0f;
            moedas += 10;

            mensagemFeedback = "Novo Dia! Meta aumentada. Boa sorte!";
            corFeedback = GOLD;
            timerFeedback = 150;
            mostrandoLoja = false;

            InicializarDia(modoAtual);
        }

        // Definição física das zonas estáticas de descarga do cenário
        Rectangle silo = { 25, 85, 80, 95 };
        Rectangle tanqueLeite = { 25, 440, 90, 75 };

        if (IsKeyPressed(KEY_SPACE)) {
            // Reabastecimento no Silo de comida
            if (CheckCollisionCircleRec(jogador.GetPosicao(), jogador.GetRaioColisao(), silo)) {
                if (jogador.GetRacao() < 10) {
                    jogador.ReabastecerRacao();
                    mensagemFeedback = "Racao abastecida! [10/10]";
                    corFeedback = YELLOW;
                }
                timerFeedback = 120;
            }
            // Descarregamento do leite coletado no Tanque Central
            else if (CheckCollisionCircleRec(jogador.GetPosicao(), jogador.GetRaioColisao(), tanqueLeite)) {
                float valorNoBalde = jogador.GetLeiteBalde();
                if (valorNoBalde > 0) {
                    float entregue = jogador.EntregarLeite();
                    leiteTotalEntregue += entregue;   // Controla a meta do dia
                    leiteAcumuladoGeral += entregue;
                    mensagemFeedback = "Sucesso: +" + std::to_string((int)entregue) + "L armazenados!";
                    corFeedback = GREEN;
                }
                timerFeedback = 120;
            }
        }

        bool apertouOrdenhar = IsKeyPressed(KEY_C);
        bool apertouAlimentar = IsKeyPressed(KEY_V);

        if (apertouOrdenhar || apertouAlimentar) {
            Vector2 posFaz = jogador.GetPosicao();
            float raioFaz = jogador.GetRaioColisao();

            for (auto& vaca : rebanho) {
                float dx = posFaz.x - vaca.GetPosicao().x;
                float dy = posFaz.y - vaca.GetPosicao().y;
                float distanciaQuadrada = (dx * dx) + (dy * dy);
                float limiteInteracao = raioFaz + vaca.GetRaioColisao() + 15.0f;

                // Checa proximidade física aceitável para interagir com o animal
                if (distanciaQuadrada <= (limiteInteracao * limiteInteracao)) {

                    // MECÂNICA DE COLETA (Tecla C)
                    if (apertouOrdenhar) {
                        float limiteBaldeMax = 40.0f + (upCapacidade * 20.0f);
                        if (jogador.GetLeiteBalde() < limiteBaldeMax) {
                            if (vaca.GetNivelLeite() >= 20.0f) {
                                float coletado = vaca.Ordenhar();

                                // Se a quantidade coletada estourar o limite das mochilas, guarda o resto na vaca
                                if (jogador.GetLeiteBalde() + coletado > limiteBaldeMax) {
                                    float excedente = (jogador.GetLeiteBalde() + coletado) - limiteBaldeMax;
                                    coletado = limiteBaldeMax - jogador.GetLeiteBalde();
                                    vaca.SetNivelLeite(excedente);
                                }

                                jogador.AdicionarLeite(coletado);
                                mensagemFeedback = "Ordenha realizada!";
                                corFeedback = SKYBLUE;
                            } else {
                                mensagemFeedback = "Esta vaca nao tem leite suficiente ainda.";
                                corFeedback = ORANGE;
                            }
                        } else {
                            mensagemFeedback = "Seu balde esta CHEIO! Va descarregar no Milk Tank.";
                            corFeedback = RED;
                        }
                    }
                    // MECÂNICA DE ALIMENTAÇÃO (Tecla V)
                    else if (apertouAlimentar) {
                        if (vaca.GetNivelFome() > 15.0f && jogador.GetRacao() > 0) {
                            vaca.Alimentar();
                            jogador.UsarRacao();
                            mensagemFeedback = "Vaca alimentada com sucesso!";
                            corFeedback = GREEN;
                        } else if (jogador.GetRacao() <= 0) {
                            mensagemFeedback = "Voce esta sem racao! Reabasteça no Silo.";
                            corFeedback = RED;
                        }
                    }
                    timerFeedback = 120;
                    break;
                }
            }
        }

        // SISTEMA DE AFUGENTAMENTO DO LOBO (Grito do Fazendeiro)
        if (lobo.IsAtivo() && IsKeyPressed(KEY_X)) {
            float dist_x = jogador.GetPosicao().x - lobo.GetPosicao().x;
            float dist_y = jogador.GetPosicao().y - lobo.GetPosicao().y;
            float distanciaAoLobo = sqrtf(dist_x * dist_x + dist_y * dist_y);

            // Se o jogador estiver perto o bastante do lobo:
            if (distanciaAoLobo < 80.0f) {
                lobo.Desativar();
                timerAparecerLobo = -200; // Penaliza o temporizador do lobo para demorar a voltar

                if (modoAtual == DESAFIO) {
                    int tempoDoCalendario = calendario.GetTempoRestante();
                    tempoDoCalendario += 10; // Bônus recompensador de tempo extra

                    if (tempoDoCalendario > 60) tempoDoCalendario = 60;

                    calendario.SetTempoRestante(tempoDoCalendario);
                    mensagemFeedback = "LOBO AFASTADO! +10 Segundos de bonus!";
                    corFeedback = GOLD;
                } else {
                    mensagemFeedback = "Boa! Voce gritou bem alto e espantou o lobo!";
                    corFeedback = GREEN;
                }
            } else {
                mensagemFeedback = "Muito longe! Chegue mais perto do lobo para espanta-lo!";
                corFeedback = ORANGE;
            }
            timerFeedback = 150;
        }
    }
}
// =========================================================================
// ATUALIZAÇÃO DA LÓGICA E FÍSICA DO JOGO
// Executa o processamento mecânico, IA, colisões e estados de minigame
// =========================================================================
void Game::AtualizarLogica() {

    // DEFINIÇÃO DA CAPACIDADE MÁXIMA DO BALDE (Baseada nos Upgrades comprados)
    float capacidadeMax = 40.0f + (upCapacidade * 20.0f);

    // =========================================================================
    // 1. SISTEMA DO MINIGAME DE ORDENHA (MODO DESAFIO)
    // =========================================================================
    if (ordenhaMinigame.IsAtivo()) {

        // Atualiza a oscilação da barra de força interna do minigame
        ordenhaMinigame.Atualizar();

        // O jogador pressiona C para tentar "parar" a agulha na zona certa
        if (IsKeyPressed(KEY_C)) {
            ordenhaMinigame.SetAtivo(false);

            auto& vaca = rebanho[ordenhaMinigame.GetIndexVaca()];
            float leiteRetirado = vaca.GetNivelLeite();

            if (leiteRetirado > 0) {
                vaca.SetNivelLeite(0.0f);
                float leiteAtual = jogador.GetLeiteBalde();

                // --- ZONA PERFEITA (Bônus de 50% de produção) ---
                if (ordenhaMinigame.GetBarra() >= 70.0f && ordenhaMinigame.GetBarra() <= 85.0f) {
                    float bonus = leiteRetirado * 0.5f;
                    float totalGanho = leiteRetirado + bonus;

                    // Adiciona o valor total diretamente, sem checar ou travar no 'capacidadeMax'
                    jogador.SetLeiteBalde(leiteAtual + totalGanho);

                    mensagemFeedback = TextFormat("ORDENHA PERFEITA! +%.1fL (Limite do balde excedido!)", totalGanho);
                    corFeedback = GOLD;
                    timerFeedback = 120;
                }

                // --- ZONA BOA (Coleta regular de 100%) ---
                else if ((ordenhaMinigame.GetBarra() >= 40.0f && ordenhaMinigame.GetBarra() < 70.0f) ||
                         (ordenhaMinigame.GetBarra() > 85.0f && ordenhaMinigame.GetBarra() <= 95.0f)) {
                    if (leiteAtual + leiteRetirado > capacidadeMax) {
                        float spaceDisponivel = capacidadeMax - leiteAtual;
                        if (spaceDisponivel > 0.0f) jogador.AdicionarLeite(spaceDisponivel);
                        mensagemFeedback = TextFormat("Ordenha Boa! Balde CHEIO. (+%.1fL aproveitados)", spaceDisponivel);
                    } else {
                        jogador.AdicionarLeite(leiteRetirado);
                        mensagemFeedback = TextFormat("Ordenha Boa! +%.1fL coletados.", leiteRetirado);
                    }
                    corFeedback = GREEN;
                    timerFeedback = 120;
                }

                // --- TIMING INCORRETO (Penalidade por estressar o animal: Perda de 20%) ---
                else {
                    float desperdicio = leiteRetirado * 0.2f;
                    float leiteFinal = leiteRetirado - desperdicio;

                    if (leiteAtual + leiteFinal > capacidadeMax) {
                        float spaceDisponivel = capacidadeMax - leiteAtual;
                        if (spaceDisponivel > 0.0f) jogador.AdicionarLeite(spaceDisponivel);
                        mensagemFeedback = TextFormat("Errou o ritmo! Balde CHEIO com perdas. (+%.1fL)", spaceDisponivel);
                    } else {
                        jogador.AdicionarLeite(leiteFinal);
                        mensagemFeedback = TextFormat("Errou o ritmo! Vaca se moveu. Apenas %.1fL aproveitados", leiteFinal);
                    }
                    corFeedback = RED;
                    timerFeedback = 120;
                }
            }
        }
    }
    else {
        // PROCESSAMENTO DE INICIALIZAÇÃO DA ORDENHA
        if (IsKeyPressed(KEY_C)) {
            if (jogador.GetLeiteBalde() >= capacidadeMax) {
                mensagemFeedback = "Seu balde ja esta cheio! Vá ate o tanque fazer a entrega.";
                corFeedback = RED;
                timerFeedback = 120;
            }
            else {
                for (size_t i = 0; i < rebanho.size(); i++) {
                    // Checa colisão por proximidade circular com as vacas para iniciar a coleta
                    if (CheckCollisionCircles(jogador.GetPosicao(), 35.0f, rebanho[i].GetPosicao(), rebanho[i].GetRaioColisao())) {

                        if (modoAtual == DESAFIO) {
                            // No desafio, abre o minigame rítmico de barra oscilante
                            ordenhaMinigame.Iniciar(i);
                            break;
                        }
                        else {
                            // No modo casual (Diversão), colhe todo o leite da vaca instantaneamente
                            if (rebanho[i].GetNivelLeite() > 0.0f) {
                                float leiteRetirado = rebanho[i].GetNivelLeite();
                                rebanho[i].SetNivelLeite(0.0f);
                                float leiteAtual = jogador.GetLeiteBalde();

                                if (leiteAtual + leiteRetirado > capacidadeMax) {
                                    float spaceDisponivel = capacidadeMax - leiteAtual;
                                    if (spaceDisponivel > 0.0f) jogador.AdicionarLeite(spaceDisponivel);
                                    mensagemFeedback = TextFormat("Vaca Ordenhada! Balde CHEIO. (+%.1fL)", spaceDisponivel);
                                } else {
                                    jogador.AdicionarLeite(leiteRetirado);
                                    mensagemFeedback = TextFormat("Vaca Ordenhada! +%.1fL", leiteRetirado);
                                }
                                corFeedback = GREEN;
                                timerFeedback = 90;
                            } else {
                                mensagemFeedback = "Esta vaca esta sem leite no momento!";
                                corFeedback = ORANGE;
                                timerFeedback = 90;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    // =========================================================================
    // 2. CONTROLE DE CRIAÇÃO/LIMPEZA DE LAMA DIÁRIA
    // =========================================================================
    if (calendario.GetClimaAtual() == CHUVOSO) {
        if (vetorPocasLama.empty()) {
            int qtdPocas = GetRandomValue(4, 6);
            for (int i = 0; i < qtdPocas; i++) {
                vetorPocasLama.push_back({ (float)GetRandomValue(160, 760), (float)GetRandomValue(100, 550) });
            }
        }
    } else {
        // Se o dia limpou ou mudou de clima, remove os focos de desaceleração do mapa
        if (!vetorPocasLama.empty()) {
            vetorPocasLama.clear();
        }
    }

    // =========================================================================
    // 3. ATUALIZAÇÃO DO MUNDO DINÂMICO (Roda apenas se não estiver travado no minigame)
    // =========================================================================
    if (!ordenhaMinigame.IsAtivo()) {

        // Cálculo de velocidade dinâmica baseado em equipamentos e clima atual
        float velocidadBaseDoFazendeiro = 3.5f;
        float modificadorVelocidade = velocidadBaseDoFazendeiro + (upVelocidade * 1.2f);
        if (calendario.GetClimaAtual() == CHUVOSO) modificadorVelocidade *= 0.65f; // Chuva dá lentidão de 35%

        // ATALHOS DE UPGRADES RÁPIDOS (Garantindo compatibilidade legada)
        if (IsKeyPressed(KEY_B)) {
            if (upVelocidade < 3) {
                if (moedas >= 20) {
                    moedas -= 20;
                    upVelocidade++;
                    mensagemFeedback = TextFormat("Bota melhorada para o Nivel %d!", upVelocidade);
                    corFeedback = GREEN;
                } else {
                    mensagemFeedback = "Moedas insuficientes para comprar a Bota! (Custa 20)";
                    corFeedback = RED;
                }
                timerFeedback = 120;
            } else {
                mensagemFeedback = "Sua bota ja atingiu a melhoria maxima!";
                corFeedback = ORANGE;
                timerFeedback = 120;
            }
        }

        if (IsKeyPressed(KEY_L)) {
            if (upCapacidade < 3) {
                if (moedas >= 30) {
                    moedas -= 30;
                    upCapacidade++;
                    mensagemFeedback = TextFormat("Capacidade do Balde aumentada para o Nivel %d!", upCapacidade);
                    corFeedback = GREEN;
                } else {
                    mensagemFeedback = "Moedas insuficientes para melhorar o Balde! (Custa 30)";
                    corFeedback = RED;
                }
                timerFeedback = 120;
            } else {
                mensagemFeedback = "Seu balde ja atingiu a capacidade maxima!";
                corFeedback = ORANGE;
                timerFeedback = 120;
            }
        }

        // DETECÇÃO DE PISADA EM LAMA CHUVOSA
        bool estaNaLama = false;
        if (calendario.GetClimaAtual() == CHUVOSO) {
            for (const auto& poça : vetorPocasLama) {
                if (CheckCollisionCircles(jogador.GetPosicao(), jogador.GetRaioColisao(), poça, RAIO_POÇA_LAMA)) {
                    estaNaLama = true;
                    break;
                }
            }
        }
        if (estaNaLama) modificadorVelocidade *= 0.40f; // Reduz drasticamente a locomoção na lama

        // CAPTURA DOS DIRECONAIS COMBINADOS (Suporta Setas e WASD)
        Vector2 entradaDirecional = { 0.0f, 0.0f };
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) entradaDirecional.x += 1.0f;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  entradaDirecional.x -= 1.0f;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))  entradaDirecional.y += 1.0f;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))    entradaDirecional.y -= 1.0f;

        Vector2 novaPos = jogador.GetPosicao();
        float raioJogador = jogador.GetRaioColisao();

        // Limites físicos da geometria do lago central do cenário
        Vector2 LAGO_CENTRO = { 465.0f, 325.0f };
        float LAGO_RAIO = 55.0f;

        bool estaNoLagoGelo = (calendario.GetEstacaoAtual() == INVERNO && CheckCollisionCircles(novaPos, raioJogador, LAGO_CENTRO, LAGO_RAIO));

        // --- SISTEMA FÍSICO DE INÉRCIA NO GELO (INVERNO) ---
        if (estaNoLagoGelo) {
            if (entradaDirecional.x != 0.0f || entradaDirecional.y != 0.0f) {
                velResidualJogador.x += entradaDirecional.x * modificadorVelocidade * 0.15f;
                velResidualJogador.y += entradaDirecional.y * modificadorVelocidade * 0.15f;
            }
            float velMag = sqrtf(velResidualJogador.x * velResidualJogador.x + velResidualJogador.y * velResidualJogador.y);
            float velMaxGelo = modificadorVelocidade * 1.3f;
            if (velMag > velMaxGelo) {
                velResidualJogador.x = (velResidualJogador.x / velMag) * velMaxGelo;
                velResidualJogador.y = (velResidualJogador.y / velMag) * velMaxGelo;
            }
            novaPos.x += velResidualJogador.x;
            novaPos.y += velResidualJogador.y;
            velResidualJogador.x *= 0.975f; // Coeficiente de atrito residual do gelo
            velResidualJogador.y *= 0.975f;
        }
        else {
            // Movimentação padrão em terra firme (Resposta imediata sem derrapagens)
            novaPos.x += entradaDirecional.x * modificadorVelocidade;
            novaPos.y += entradaDirecional.y * modificadorVelocidade;
            velResidualJogador = { 0.0f, 0.0f };
        }

        // --- PAREDE DE COLISÃO DO LAGO LÍQUIDO (PRIMAVERA, VERÃO, OUTONO) ---
        if (calendario.GetEstacaoAtual() != INVERNO && CheckCollisionCircles(novaPos, raioJogador, LAGO_CENTRO, LAGO_RAIO)) {
            float dx = novaPos.x - LAGO_CENTRO.x;
            float dy = novaPos.y - LAGO_CENTRO.y;
            float distancia = sqrtf(dx * dx + dy * dy);
            if (distancia > 0) {
                float sobreposicao = (LAGO_RAIO + raioJogador) - distancia;
                novaPos.x += (dx / distancia) * sobreposicao;
                novaPos.y += (dy / distancia) * sobreposicao;
            }
        }

        // RESTRIÇÕES DE FRONTEIRA DA TELA (Garante confinamento nos 800x600 virtuais)
        if (novaPos.x < 15)  novaPos.x = 15;
        if (novaPos.x > 785) novaPos.x = 785;
        if (novaPos.y < 65)  novaPos.y = 65;
        if (novaPos.y > 585) novaPos.y = 585;

        jogador.SetPosicao(novaPos.x, novaPos.y);

        // --- SISTEMA DE INTEGRAÇÃO DA IA DO LOBO ---
        timerAparecerLobo++;
        if (!lobo.IsAtivo() && timerAparecerLobo > 600) {
            int ladoSpawn = GetRandomValue(0, 3);
            float spawnX = 0, spawnY = 0;
            std::string direcaoTexto = "";
            float velocidadDoLobo = 1.5f + (calendario.GetDiaAtual() * 0.2f); // Lobo escala velocidade com os dias avançados

            switch (ladoSpawn) {
            case 0: spawnX = (float)GetRandomValue(160, 760); spawnY = -30.0f; direcaoTexto = "pelo NORTE!"; break;
            case 1: spawnX = (float)GetRandomValue(160, 760); spawnY = 630.0f; direcaoTexto = "pelo SUL!"; break;
            case 2: spawnX = -30.0f; spawnY = (float)GetRandomValue(100, 550); direcaoTexto = "pelo OESTE!"; break;
            case 3: spawnX = 830.0f; spawnY = (float)GetRandomValue(100, 550); direcaoTexto = "pelo LESTE!"; break;
            }

            lobo.Spawnar(spawnX, spawnY, velocidadDoLobo);
            timerAparecerLobo = 0;
            mensagemFeedback = "AVISO: Um lobo faminto invadiu o pasto " + direcaoTexto;
            corFeedback = RED;
            timerFeedback = 150;
        }

        if (lobo.IsAtivo()) {
            if (lobo.PerseguirRebanho(rebanho)) {
                mensagemFeedback = "O lobo assustou uma vaca e ela perdeu o leite!";
                corFeedback = PURPLE;
                timerFeedback = 150;
            }
        }

        // Realoca memória das tabelas físicas dinamicamente se o tamanho do rebanho mudar
        if (velResidualVacas.size() != rebanho.size()) {
            velResidualVacas.assign(rebanho.size(), { 0.0f, 0.0f });
        }

        // Caixa delimitadora de cercados do pasto dos animais
        const float PASTO_MIN_X = 145.0f;
        const float PASTO_MAX_X = 790.0f;
        const float PASTO_MIN_Y = 65.0f;
        const float PASTO_MAX_Y = 595.0f;

        // =========================================================================
        // [LOOP INDIVIDUAL DE ATUALIZAÇÃO DO REBANHO]
        // =========================================================================
        for (size_t i = 0; i < rebanho.size(); i++) {
            Vector2 posAntigaVaca = rebanho[i].GetPosicao();

            // Atualiza processos internos orgânicos da vaca (fome, produção de leite sazonal)
            rebanho[i].Atualizar(calendario.GetEstacaoAtual());

            // --- REGRAS COMPORTAMENTAIS DO CLIMA DE SECA/CHUVOSO ---
            if (calendario.GetClimaAtual() == SECA) {
                // CORREÇÃO: Removida chamada dupla desnecessária do Atualizar da vaca
                if (GetRandomValue(1, 100) > 95) {
                    // Espaço para lógicas adicionais de efeitos negativos de calor, se necessário
                }
            }
            else if (calendario.GetClimaAtual() == CHUVOSO) {
                // Chuva estimula grama fresca: Vacas produzem leite 5% mais rápido por frame passivo
                if (rebanho[i].GetNivelLeite() < 100.0f) {
                    rebanho[i].SetNivelLeite(rebanho[i].GetNivelLeite() + 0.05f);
                }
            }

            Vector2 posVaca = rebanho[i].GetPosicao();
            float raioVaca = rebanho[i].GetRaioColisao();

            bool vacaNoLagoGelo = (calendario.GetEstacaoAtual() == INVERNO && CheckCollisionCircles(posVaca, raioVaca, LAGO_CENTRO, LAGO_RAIO));

            // --- ESCORREGAMENTO DOS ANIMAIS NO LAGO CONGELADO ---
            if (vacaNoLagoGelo) {
                float dirX = posVaca.x - posAntigaVaca.x;
                float dirY = posVaca.y - posAntigaVaca.y;

                if (dirX != 0.0f || dirY != 0.0f) {
                    velResidualVacas[i].x += dirX * 0.20f;
                    velResidualVacas[i].y += dirY * 0.20f;
                }

                float velVacaMag = sqrtf(velResidualVacas[i].x * velResidualVacas[i].x + velResidualVacas[i].y * velResidualVacas[i].y);
                float velMaxVacaGelo = 4.0f;
                if (velVacaMag > velMaxVacaGelo) {
                    velResidualVacas[i].x = (velResidualVacas[i].x / velVacaMag) * velMaxVacaGelo;
                    velResidualVacas[i].y = (velResidualVacas[i].y / velVacaMag) * velMaxVacaGelo;
                }

                posVaca.x += velResidualVacas[i].x;
                posVaca.y += velResidualVacas[i].y;
                velResidualVacas[i].x *= 0.975f;
                velResidualVacas[i].y *= 0.975f;
            } else {
                velResidualVacas[i] = { 0.0f, 0.0f };
            }

            // --- COLISÃO DAS VACAS COM O LAGO ABERTO ---
            if (calendario.GetEstacaoAtual() != INVERNO && CheckCollisionCircles(posVaca, raioVaca, LAGO_CENTRO, LAGO_RAIO)) {
                float dx = posVaca.x - LAGO_CENTRO.x;
                float dy = posVaca.y - LAGO_CENTRO.y;
                float distancia = sqrtf(dx * dx + dy * dy);
                if (distancia > 0) {
                    float sobreposicao = (LAGO_RAIO + raioVaca) - distancia;
                    posVaca.x += (dx / distancia) * sobreposicao;
                    posVaca.y += (dy / distancia) * sobreposicao;
                }
            }

            // Garante confinamento das vacas nos cercados invisíveis do pasto
            if (posVaca.x < PASTO_MIN_X) posVaca.x = PASTO_MIN_X;
            if (posVaca.x > PASTO_MAX_X) posVaca.x = PASTO_MAX_X;
            if (posVaca.y < PASTO_MIN_Y) posVaca.y = PASTO_MIN_Y;
            if (posVaca.y > PASTO_MAX_Y) posVaca.y = PASTO_MAX_Y;

            rebanho[i].SetPosicao(posVaca.x, posVaca.y);
        }
    }
}

// =========================================================================
// RENDERIZAÇÃO GRÁFICA (SISTEMA DE PROJEÇÃO RESPONSIVA)
// Gerencia a viewport através de render texture para evitar quebras em resoluções altas
// =========================================================================
void Game::RenderizarTela() {

    // =========================================================================
    // PASSO 1: REDIRECIONAR TODO O DESENHO PARA A TEXTURA VIRTUAL (800x600)
    // =========================================================================
    BeginTextureMode(telaVirtual);
    ClearBackground(BLACK);

    switch (estadoAtual) {
    case TELA_MENU:
        DesenharMenu();
        break;
    case TELA_SELECAO_PERSONAGEM:
        DesenharSelecaoPersonagem();
        break;
    case TELA_TUTORIAL:
        DesenharTutorial();
        break;
    case JOGANDO:
        DesenharGameplay();
        break;
    case GAME_OVER:
        DesenharGameOver();
        break;
    }

    EndTextureMode(); // Encerra o buffer interno de renderização da GPU

    // =========================================================================
    // PASSO 2: CALCULAR A ESCALA MATEMÁTICA RESPONSIVA E EXIBIR NA JANELA REAL
    // =========================================================================
    BeginDrawing();
    ClearBackground(BLACK);

    float larguraReal = (float)GetScreenWidth();
    float alturaReal = (float)GetScreenHeight();

    Rectangle destino;
    // Se o usuário estiver usando Fullscreen completo esticado:
    if (IsWindowFullscreen()) {
        destino = { 0.0f, 0.0f, larguraReal, alturaReal };
    }
    else {
        // Modo Janela ativa o cálculo de Letterboxing (Proporções de aspecto pretas nas bordas)
        float escala = fminf(larguraReal / LARGURA_VIRTUAL, alturaReal / ALTURA_VIRTUAL);
        destino = {
            (larguraReal - ((float)LARGURA_VIRTUAL * escala)) * 0.5f,
            (alturaReal - ((float)ALTURA_VIRTUAL * escala)) * 0.5f,
            (float)LARGURA_VIRTUAL * escala,
            (float)ALTURA_VIRTUAL * escala
        };
    }

    // Inverte o eixo Y na amostragem da textura de origem porque coordenadas de render texture na Raylib operam invertidas por padrão da GPU
    Rectangle origem = { 0.0f, 0.0f, (float)LARGURA_VIRTUAL, (float)-ALTURA_VIRTUAL };

    // Desenha o canvas virtual projetado perfeitamente adaptado no monitor real
    DrawTexturePro(telaVirtual.texture, origem, destino, { 0.0f, 0.0f }, 0.0f, WHITE);

    // Renderiza indicador de quadros por segundo para monitoramento de travamentos
    EndDrawing();
}
// =========================================================================
// RENDERIZAÇÃO DO MENU PRINCIPAL
// =========================================================================
void Game::DesenharMenu() {
    ClearBackground({35, 45, 55, 255});
    DrawText("COW GAME: Nolan's Farm", LARGURA_VIRTUAL / 2 - 240, 120, 36, GOLD);
    DrawText("Escolha o Modo de Jogo para Iniciar:", LARGURA_VIRTUAL / 2 - 190, 220, 18, RAYWHITE);

    // --- Caixa do Modo Diversão (Verde) ---
    DrawRectangle(150, 280, 500, 80, {45, 140, 75, 255});
    DrawRectangleLines(150, 280, 500, 80, BLACK);
    DrawText("[1] MODO DIVERSÃO", 170, 295, 18, WHITE);
    DrawText("Bata a meta e curta o dia!", 170, 325, 13, LIGHTGRAY);

    // --- Caixa do Modo Desafio (Vermelha) ---
    DrawRectangle(150, 390, 500, 80, {170, 50, 45, 255});
    DrawRectangleLines(150, 390, 500, 80, BLACK);
    DrawText("[2] MODO DESAFIO", 170, 405, 18, WHITE);
    DrawText("O tempo fica mais curto a cada dia. Bata a meta para sobreviver!", 170, 435, 13, LIGHTGRAY);

    DrawText("Desenvolvido em C++ com Raylib", LARGURA_VIRTUAL / 2 - 100, 530, 12, GRAY);
}

// =========================================================================
// RENDERIZAÇÃO DA SELEÇÃO DE PERSONAGEM
// =========================================================================
void Game::DesenharSelecaoPersonagem() {
    ClearBackground({30, 35, 45, 255});

    DrawText("SELECIONE SEU PERSONAGEM", LARGURA_VIRTUAL / 2 - MeasureText("SELECIONE SEU PERSONAGEM", 26) / 2, 60, 26, GOLD);
    DrawText("Use as teclas [1] ou [2] para escolher:", LARGURA_VIRTUAL / 2 - MeasureText("Use as teclas [1] ou [2] para escolher:", 16) / 2, 110, 16, LIGHTGRAY);

    // Salva o estado real do jogador antes da pré-visualização gráfica
    Vector2 posOriginal = jogador.GetPosicao();
    Genero generoOriginal = jogador.GetGenero();

    // --- OPÇÃO 1: FAZENDEIRO (MASCULINO) ---
    Color corBordaMasc = (generoSelecionado == MASCULINO) ? GOLD : GRAY;
    int espessuraMasc = (generoSelecionado == MASCULINO) ? 3 : 1;

    DrawRectangle(120, 180, 240, 320, {45, 55, 75, 255});
    for (int i = 0; i < espessuraMasc; i++) {
        DrawRectangleLines(120 - i, 180 - i, 240 + (i * 2), 320 + (i * 2), corBordaMasc);
    }
    DrawEllipse(240, 420, 60, 18, {20, 25, 35, 150}); // Sombra projetada estática

    // Altera temporariamente a instância para renderização do manequim na caixa
    jogador.SetGenero(MASCULINO);
    jogador.SetPosicao(240.0f, 380.0f);
    jogador.Desenhar(calendario.GetEstacaoAtual(), 2.0f);

    DrawRectangle(140, 450, 200, 40, (generoSelecionado == MASCULINO) ? GREEN : DARKGRAY);
    DrawText("[1] FAZENDEIRO", 175, 462, 16, WHITE);

    // --- OPÇÃO 2: FAZENDEIRA (FEMININO) ---
    Color corBordaFem = (generoSelecionado == FEMININO) ? GOLD : GRAY;
    int espessuraFem = (generoSelecionado == FEMININO) ? 3 : 1;

    DrawRectangle(440, 180, 240, 320, {45, 55, 75, 255});
    for (int i = 0; i < espessuraFem; i++) {
        DrawRectangleLines(440 - i, 180 - i, 240 + (i * 2), 320 + (i * 2), corBordaFem);
    }
    DrawEllipse(560, 420, 60, 18, {20, 25, 35, 150}); // Sombra projetada estática

    // Altera temporariamente a instância para renderização do manequim na caixa
    jogador.SetGenero(FEMININO);
    jogador.SetPosicao(560.0f, 380.0f);
    jogador.Desenhar(calendario.GetEstacaoAtual(), 2.0f);

    DrawRectangle(460, 450, 200, 40, (generoSelecionado == FEMININO) ? GREEN : DARKGRAY);
    DrawText("[2] FAZENDEIRA", 495, 462, 16, WHITE);

    // SEGURANÇA: Restaura os atributos originais do jogador ativo para o gameplay
    jogador.SetPosicao(posOriginal.x, posOriginal.y);
    jogador.SetGenero(generoOriginal);

    DrawText("Pressione [ESPAÇO] ou [ENTER] para confirmar a seleção",
             LARGURA_VIRTUAL / 2 - MeasureText("Pressione [ESPAÇO] ou [ENTER] para confirmar a seleção", 14) / 2, 535, 14, ORANGE);
}

// =========================================================================
// RENDERIZAÇÃO DA TELA DE TUTORIAL
// =========================================================================
void Game::DesenharTutorial() {
    ClearBackground({30, 35, 45, 255});
    DrawText("COMO JOGAR", LARGURA_VIRTUAL / 2 - MeasureText("COMO JOGAR", 28) / 2, 45, 28, GOLD);

    // --- PAINEL DE CONTROLES (ESQUERDA) ---
    DrawRectangle(60, 110, 320, 360, {45, 50, 65, 255});
    DrawRectangleLines(60, 110, 320, 360, RAYWHITE);
    DrawText("CONTROLES", 80, 125, 18, SKYBLUE);
    DrawText("[W][A][S][D] / Setas", 80, 160, 14, ORANGE);
    DrawText("Mover o Fazendeiro", 80, 180, 13, LIGHTGRAY);
    DrawText("[C]", 80, 215, 14, ORANGE);
    DrawText("Aproxime-se e Ordenhe a Vaca", 80, 235, 13, LIGHTGRAY);
    DrawText("[V]", 80, 270, 14, ORANGE);
    DrawText("Alimentar vaca com Ração", 80, 290, 13, LIGHTGRAY);
    DrawText("[ESPAÇO]", 80, 325, 14, ORANGE);
    DrawText("Entregar leite no Tanque / \nPegar ração no Silo", 80, 345, 13, LIGHTGRAY);
    DrawText("[U]", 80, 385, 14, ORANGE);
    DrawText("Abrir a Loja no Celeiro", 80, 405, 13, LIGHTGRAY);

    // --- PAINEL DE REGRAS E DICAS (DIREITA) ---
    DrawRectangle(420, 110, 320, 360, {45, 50, 65, 255});
    DrawRectangleLines(420, 110, 320, 360, RAYWHITE);
    DrawText("REGRAS DO NEGÓCIO", 440, 125, 18, GREEN);
    DrawText("1. Monitore seu Balde:", 440, 160, 14, GOLD);
    DrawText("Se o balde encher, você não consegue\nmais ordenhar. Vá ao Tanque esvaziá-lo!", 440, 180, 11, LIGHTGRAY);
    DrawText("2. O Ritmo Perfeito [C]:", 440, 230, 14, GOLD);
    DrawText("No Desafio, aperte [C] na zona dourada.\nApenas a ordenha PERFEITA supera\no limite máximo do balde!", 440, 250, 11, LIGHTGRAY);
    DrawText("3. Cuidado com o Clima:", 440, 315, 14, GOLD);
    DrawText("Dias de chuva geram poças de lama.\nPisar nelas reduz drasticamente a sua\nvelocidade de movimentação!", 440, 335, 11, LIGHTGRAY);
    DrawText("4. Proteja o Rebanho:", 440, 400, 14, GOLD);
    DrawText("Fique atento aos lobos invasores!\nEles atacam o rebanho sem aviso.", 440, 420, 11, LIGHTGRAY);

    // --- BARRA INFERIOR DE INICIALIZAÇÃO ---
    DrawRectangle(0, 500, LARGURA_VIRTUAL, 50, {20, 25, 30, 255});
    DrawText("Pressione [ENTER] ou [ESPAÇO] para iniciar a sua jornada!", LARGURA_VIRTUAL / 2 - MeasureText("Pressione [ENTER] ou [ESPAÇO] para iniciar a sua jornada!", 16) / 2, 515, 16, RAYWHITE);
}

// =========================================================================
// RENDERIZAÇÃO DA LOOP PRINCIPAL DE GAMEPLAY
// =========================================================================
void Game::DesenharGameplay() {
    // Desenha o mapa do mundo baseado nos dados climáticos e sazonais
    mapa.DesenharFundo(calendario.GetEstacaoAtual(), calendario.GetClimaAtual(), modoAtual, vetorPocasLama);
    mapa.DesenharConstrucoes(modoAtual, calendario.GetEstacaoAtual());

    // Desenha as vacas e suas respectivas sombras projetadas
    for (auto& vaca : rebanho) {
        mapa.DesenharSombraProjetada(vaca.GetPosicao().x, vaca.GetPosicao().y + 14, 48, 16);
        vaca.Desenhar(calendario.GetEstacaoAtual());
    }

    // Desenha o lobo se a IA estiver ativa em campo
    if (lobo.IsAtivo()) {
        lobo.Desenhar(calendario.GetEstacaoAtual());
    }

    // Desenha o Jogador e sua respectiva sombra
    mapa.DesenharSombraProjetada(jogador.GetPosicao().x, jogador.GetPosicao().y + 15, 30, 12);
    jogador.Desenhar(calendario.GetEstacaoAtual());

    // Filtro de Iluminação da Tarde/Noite vindo de forma limpa do Calendário (Modo Casual)
    if (modoAtual == DIVERSAO) {
        DrawRectangle(0, 0, LARGURA_VIRTUAL, ALTURA_VIRTUAL, calendario.GetCorFiltroAtual());
    }

    // Interface de Menu da Loja do Celeiro (Sobreposição)
    if (mostrandoLoja) {
        DrawRectangle(200, 180, 400, 220, Fade(BLACK, 0.90f));
        DrawRectangleLines(200, 180, 400, 220, GOLD);
        DrawText("CELEIRO - MERCADO DE UPGRADES", 220, 200, 18, GOLD);
        DrawText(TextFormat("[1] Botas de Corrida (Nvl %d/3) - Custo: 20 Moedas", upVelocidade), 220, 250, 14, (upVelocidade >= 3) ? GRAY : (moedas >= 20) ? GREEN : RAYWHITE);
        DrawText(TextFormat("[2] Expansao de Balde (Nvl %d/3) - Custo: 30 Moedas", upCapacidade), 220, 290, 14, (upCapacidade >= 3) ? GRAY : (moedas >= 30) ? GREEN : RAYWHITE);
        DrawText(TextFormat("Seu Saldo: %d Moedas", moedas), 220, 330, 13, GOLD);
        DrawText("Pressione [U] para Retornar ao Jogo", 220, 370, 11, LIGHTGRAY);
    }

    // Desenha a Barra Oscilante do Minigame se ele estiver rodando (Modo Desafio)
    if (ordenhaMinigame.IsAtivo() && modoAtual == DESAFIO) {
        ordenhaMinigame.Desenhar(jogador.GetPosicao());
    }

    // -------------------------------------------------------------------------
    // RENDERIZAÇÃO DO HUD SUPERIOR COLOADO
    // -------------------------------------------------------------------------
    DrawRectangle(0, 0, LARGURA_VIRTUAL, 60, { 20, 25, 35, 255 });
    DrawRectangle(0, 58, LARGURA_VIRTUAL, 2, GOLD);

    float limiteCapacidadeHUD = 40.0f + (upCapacidade * 20.0f);

    // --- Coluna 1: Informações numéricas básicas de Armazenamento ---
    DrawText(TextFormat("TANQUE: %d / %dL", (int)leiteTotalEntregue, (int)metaLeite), 20, 12, 13, RAYWHITE);
    DrawText(TextFormat("BALDE:  %d / %d L", (int)jogador.GetLeiteBalde(), (int)limiteCapacidadeHUD), 20, 34, 13, SKYBLUE);

    // --- Coluna 2: Barras de progresso visual (Tanque e Balde) ---
    float percentualMeta = (metaLeite > 0) ? (leiteTotalEntregue / metaLeite) : 0;
    if (percentualMeta > 1.0f) percentualMeta = 1.0f;
    DrawRectangle(185, 14, 100, 10, DARKGRAY);
    DrawRectangle(185, 14, (int)(100 * percentualMeta), 10, GREEN);

    float percentualBalde = (limiteCapacidadeHUD > 0) ? (jogador.GetLeiteBalde() / limiteCapacidadeHUD) : 0;
    if (percentualBalde > 1.0f) percentualBalde = 1.0f;
    DrawRectangle(185, 36, 100, 10, DARKGRAY);
    DrawRectangle(185, 36, (int)(100 * percentualBalde), 10, SKYBLUE);

    // --- Coluna 3: Recursos de inventário e economia ---
    DrawText(TextFormat("RACAO: %d/10", jogador.GetRacao()), 310, 12, 13, { 245, 220, 150, 255 });
    DrawText(TextFormat("MOEDAS: %d", moedas), 310, 34, 13, GOLD);

    // --- Coluna 4: Informações Sazonais consolidadas do Calendário ---
    DrawText(TextFormat("DIA: %02d", calendario.GetDiaAtual()), 440, 21, 16, GOLD);
    DrawText(TextFormat("ESTACAO: %s", calendario.GetTextoEstacao().c_str()), 520, 12, 12, LIGHTGRAY);
    DrawText(TextFormat("CLIMA:   %s", calendario.GetTextoClima().c_str()), 520, 34, 12, { 140, 200, 245, 255 });

    // --- Coluna 5: Mostrador de Relógio ou Cronômetro Progressivo ---
    if (modoAtual == DIVERSAO) {
        float segundosPassados = 120.0f - (float)calendario.GetTempoRestante();
        float totalMinutosVirtuais = segundosPassados * 12.0f; // Conversão de tempo para escala de 24h

        int horaHUD = (int)(totalMinutosVirtuais / 60.0f) % 24;
        int minutoHUD = (int)totalMinutosVirtuais % 60;

        Color corDoRelogio = (horaHUD >= 18 || horaHUD < 6) ? ORANGE : GREEN;

        DrawRectangleLines(695, 12, 85, 35, corDoRelogio);
        DrawText(TextFormat("%02d:%02dh", horaHUD, minutoHUD), 705, 21, 16, corDoRelogio);
    } else {
        Color corDoTempo = (calendario.GetTempoRestante() < 10) ? RED : (calendario.GetTempoRestante() < 20) ? ORANGE : GREEN;
        int segundosHUD = calendario.GetTempoRestante();
        if (segundosHUD < 0) segundosHUD = 0;

        DrawRectangleLines(715, 12, 65, 35, corDoTempo);
        DrawText(TextFormat("%02ds", segundosHUD), 727, 21, 18, corDoTempo);
    }

    // -------------------------------------------------------------------------
    // RENDERIZAÇÃO DO RODAPÉ DE FEEDBACK E LEGENDAS
    // -------------------------------------------------------------------------
    DrawRectangle(0, 565, LARGURA_VIRTUAL, 35, { 25, 25, 25, 240 });
    DrawLine(0, 565, LARGURA_VIRTUAL, 565, GRAY);
    if (timerFeedback > 0) {
        DrawText(mensagemFeedback.c_str(), 20, 575, 13, corFeedback);
    } else {
        DrawText("CONTROLES: [W][A][S][D]/[SETAS] Mover | [C] Ordenhar | [V] Alimentar | [SPACE] Interagir | [U] Loja", 20, 575, 12, LIGHTGRAY);
    }

    // --- Tela Suspensa Intermediária de Meta Alcançada (Modo Desafio) ---
    if (modoAtual == DESAFIO && diaCompleto) {
        DrawRectangle(0, 0, LARGURA_VIRTUAL, ALTURA_VIRTUAL, Fade(BLACK, 0.6f));
        DrawText("META ATINGIDA COM SUCESSO!", LARGURA_VIRTUAL / 2 - 180, ALTURA_VIRTUAL / 2 - 40, 24, GREEN);
        DrawText("Pressione [ENTER] para ir para o proximo dia", LARGURA_VIRTUAL / 2 - 195, ALTURA_VIRTUAL / 2 + 10, 15, RAYWHITE);
    }
}

// =========================================================================
// RENDERIZAÇÃO DA TELA DE GAME OVER
// =========================================================================
void Game::DesenharGameOver() {
    ClearBackground({30, 20, 20, 255});
    DrawText("GAME OVER", LARGURA_VIRTUAL / 2 - MeasureText("GAME OVER", 40) / 2, 180, 40, RED);
    DrawText("O tempo acabou e voce nao bateu a meta!", LARGURA_VIRTUAL / 2 - MeasureText("O tempo acabou e voce nao bateu a meta!", 20) / 2, 250, 20, GRAY);

    DrawText(TextFormat("Dias Sobrevividos: %d", calendario.GetDiaAtual()), LARGURA_VIRTUAL / 2 - 120, 320, 24, WHITE);
    DrawText(TextFormat("Total de Leite Entregue: %.1f L", leiteAcumuladoGeral), LARGURA_VIRTUAL / 2 - 120, 360, 24, WHITE);

    DrawText("Pressione [R] para voltar ao Menu Principal", LARGURA_VIRTUAL / 2 - MeasureText("Pressione [R] para voltar ao Menu Principal", 18) / 2, 460, 18, GOLD);
}