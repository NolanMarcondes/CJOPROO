// =========================================================================
// ARQUIVO PRINCIPAL DO SISTEMA (PONTO DE ENTRADA DO JOGO)
// =========================================================================
#include "game.h"
#include <ctime>
#include <cstdlib>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif
int _stat64i32(const char* path, struct _stat64i32* buffer) {
    return _stat(path, (struct _stat*)buffer);
}
#ifdef __cplusplus
}
#endif

int main() {
    // -------------------------------------------------------------------------
    // 1. INICIALIZAÇÃO DO MOTOR ALEATÓRIO (SEED)
    // -------------------------------------------------------------------------
    // Semeia o gerador pseudoaleatório usando o timestamp atual do sistema.
    // Essencial para o comportamento da IA dos lobos e movimentação das vacas.
    srand(static_cast<unsigned int>(time(NULL)));

    // -------------------------------------------------------------------------
    // 2. INSTANCIAÇÃO DO MÓDULO GERENCIAL (GAME CORE)
    // -------------------------------------------------------------------------
    // Cria o objeto principal que encapsula o contexto da Raylib, inicializa
    // a janela de exibição, buffers gráficos e carrega os assets necessários.
    Game meuJogo;

    // -------------------------------------------------------------------------
    // 3. ENTRADA NO LOOP PRINCIPAL DE EXECUÇÃO
    // -------------------------------------------------------------------------
    // Dispara a máquina de estados do jogo. Esse loop roda continuamente
    // processando Input -> Update -> Render até que o gatilho de saída seja acionado.
    meuJogo.Executar();

    // -------------------------------------------------------------------------
    // 4. ENCERRAMENTO E DESALOCAÇÃO DE MEMÓRIA
    // -------------------------------------------------------------------------
    // O programa atinge este ponto após o fechamento seguro da janela gráfica.
    // Retorna 0 indicando que o processo foi finalizado com sucesso pelo S.O.
    return 0;
}