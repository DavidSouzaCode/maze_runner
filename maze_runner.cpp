#include <stdio.h>
#include <stack>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <iostream>
#include <mutex>
#include <vector>

char** maze;
int num_rows;
int num_cols;

struct pos_t {
    int i;
    int j;
};

std::stack<pos_t> valid_positions;
std::mutex mtx; // Mutex para sincronizar a impressão do labirinto

// Função para imprimir o labirinto em uma thread separada
void print_maze_thread() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Imprimir a cada meio segundo
        mtx.lock(); // Bloquear o mutex antes de imprimir
        system("clear"); // Limpar a tela (Windows: "cls")
        for (int i = 0; i < num_rows; ++i) {
            for (int j = 0; j < num_cols; ++j) {
                printf("%c", maze[i][j]);
            }
            printf("\n");
        }
        mtx.unlock(); // Desbloquear o mutex após a impressão
    }
}

pos_t load_maze(const char* file_name) {
    pos_t initial_pos;

    FILE* file = fopen(file_name, "r");

    if (!file) {
        printf("Não foi possível abrir o arquivo.\n");
        exit(1);
    }

    fscanf(file, "%d %d", &num_rows, &num_cols);

    maze = new char*[num_rows];
    for (int i = 0; i < num_rows; ++i) {
        maze[i] = new char[num_cols];
    }

    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            fscanf(file, " %c", &maze[i][j]);
            if (maze[i][j] == 'e') {
                initial_pos.i = i;
                initial_pos.j = j;
            }
        }
    }

    fclose(file);
    return initial_pos;
}

bool walk(pos_t pos) {
    if (maze[pos.i][pos.j] == 's') {
        return true; // Saída encontrada
    }

    maze[pos.i][pos.j] = '.';

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
/*
    mtx.lock(); // Bloquear o mutex antes de imprimir
    system("clear"); // Limpar a tela (Windows: "cls")
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            printf("%c", maze[i][j]);
        }
        printf("\n");
    }
    mtx.unlock(); // Desbloquear o mutex após a impressão
*/
    std::vector<std::thread> threads; // Para armazenar threads adicionais

    if (pos.i > 0 && maze[pos.i - 1][pos.j] != '.' && maze[pos.i - 1][pos.j] != '#') {
        valid_positions.push({pos.i - 1, pos.j});
        //std::thread t(walk, pos_t{pos.i - 1, pos.j}); // Criar uma thread para explorar
        //t.detach(); // Desanexar a thread
    }
    if (pos.i < num_rows - 1 && maze[pos.i + 1][pos.j] != '.' && maze[pos.i + 1][pos.j] != '#') {
        valid_positions.push({pos.i + 1, pos.j});
        //std::thread t(walk, pos_t{pos.i + 1, pos.j}); // Criar uma thread para explorar
        //t.detach(); // Desanexar a thread
    }
    if (pos.j > 0 && maze[pos.i][pos.j - 1] != '.' && maze[pos.i][pos.j - 1] != '#') {
        valid_positions.push({pos.i, pos.j - 1});
        //std::thread t(walk, pos_t{pos.i, pos.j - 1}); // Criar uma thread para explorar
        //t.detach(); // Desanexar a thread
    }
    if (pos.j < num_cols - 1 && maze[pos.i][pos.j + 1] != '.' && maze[pos.i][pos.j + 1] != '#') {
        valid_positions.push({pos.i, pos.j + 1});
        //std::thread t(walk, pos_t{pos.i, pos.j + 1}); // Criar uma thread para explorar
        //t.detach(); // Desanexar a thread
    }

	// Verifica se a pilha de posições não está vazia
	if (!valid_positions.empty()) {
		pos_t next_position = valid_positions.top();
		valid_positions.pop();

        if(!valid_positions.empty()){
            pos_t new_position = valid_positions.top();
            valid_positions.pop();
            std::thread t(walk, pos_t{new_position}); // Criar uma thread para explorar
            t.detach(); // Desanexar a thread
        }

		return walk(next_position);
	}

    return false; // Saída não encontrada
}

int main(int argc, char* argv[]) {
    pos_t initial_pos = load_maze("../data/maze6.txt");

    printf("Iniciando a navegação pelo labirinto...\n");

    // Criar uma thread para imprimir o labirinto (joinable)
    std::thread printer(print_maze_thread);

    bool exit_found = walk(initial_pos);

    // Esperar que a thread de impressão termine antes de encerrar o programa
    printer.join();

    // Tratar o retorno (imprimir mensagem)
    if (exit_found) {
        printf("\nSaída encontrada!\n");
    } else {
        printf("\nSaída não encontrada.\n");
    }

    // Liberar a memória alocada para o labirinto
    for (int i = 0; i < num_rows; ++i) {
        delete[] maze[i];
    }
    delete[] maze;

    return 0;
}
