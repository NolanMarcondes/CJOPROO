cmake_minimum_required(VERSION 3.5)

project(CowGame VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-D_FILE_OFFSET_BITS=64 -D__MSVCRT_VERSION__=0x0700)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define os arquivos do projeto
add_executable(CowGame
    main.cpp
    vaca.h
    vaca.cpp
    fazendeiro.h
    fazendeiro.cpp
    lobo.h lobo.cpp
    tipos.h tipos.cpp
    cenario.h cenario.cpp
    game.h game.cpp
    minigame_ordenha.h minigame_ordenha.cpp
    calendario.h calendario.cpp






)

# Caminhos de desenvolvimento da Raylib no Windows
target_include_directories(CowGame PRIVATE "C:/raylib/raylib/src")

# Vincula usando as flags padrão do Windows e a biblioteca de importação dinâmina
# Linkagem direta com o arquivo .a original
target_link_libraries(CowGame PRIVATE
    "C:/raylib/raylib/src/libraylib.a"
    opengl32
    gdi32
    winmm
)
target_link_options(CowGame PRIVATE "-Wl,--allow-multiple-definition")