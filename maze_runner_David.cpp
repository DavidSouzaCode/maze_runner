#include <stdio.h>
#include <stack>

// Matriz de char representando o labirinto
char** maze;

// Numero de linhas e colunas do labirinto
int num_rows;
int num_cols;

// Representação de uma posição
struct pos_t {
	int i;
	int j;
};

// Estrutura de dados contendo as próximas posicões a serem exploradas no labirinto
std::stack<pos_t> valid_positions;

// Função que le o labirinto de um arquivo texto, carrega em memória e retorna a posição inicial
pos_t load_maze(const char* file_name) {
	pos_t initial_pos;
	
	FILE* file = fopen(file_name, "r");
    /*
	if (!file) {
		printf("Erro ao abrir o arquivo.\n");
		exit(1);
	}
    */
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

// Função que imprime o labirinto
void print_maze() {
	for (int i = 0; i < num_rows; ++i) {
		for (int j = 0; j < num_cols; ++j) {
			printf("%c", maze[i][j]);
		}
		printf("\n");
	}
}

// Função responsável pela navegação.
bool walk(pos_t pos) {
	if (maze[pos.i][pos.j] == 's') {
		return true; // Saída encontrada
	}

	// Marcar a posição atual como visitada
	maze[pos.i][pos.j] = '.';

	// Limpar a tela e imprimir o labirinto
	system("clear"); // Para sistemas Unix-like
	//system("cls"); // Para sistemas Windows
	print_maze();

	// Verificar as próximas posições possíveis
	if (pos.i > 0 && maze[pos.i - 1][pos.j] != '.' && maze[pos.i - 1][pos.j] != 'x') {
		valid_positions.push({pos.i - 1, pos.j});
	}
	if (pos.i < num_rows - 1 && maze[pos.i + 1][pos.j] != '.' && maze[pos.i + 1][pos.j] != 'x') {
		valid_positions.push({pos.i + 1, pos.j});
	}
	if (pos.j > 0 && maze[pos.i][pos.j - 1] != '.' && maze[pos.i][pos.j - 1] != 'x') {
		valid_positions.push({pos.i, pos.j - 1});
	}
	if (pos.j < num_cols - 1 && maze[pos.i][pos.j + 1] != '.' && maze[pos.i][pos.j + 1] != 'x') {
		valid_positions.push({pos.i, pos.j + 1});
	}

	// Verifica se a pilha de posições não está vazia
	if (!valid_positions.empty()) {
		pos_t next_position = valid_positions.top();
		valid_positions.pop();
		return walk(next_position);
	}

	return false; // Saída não encontrada
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Uso: %s <nome_do_arquivo>\n", argv[0]);
		return 1;
	}

	// Carregar o labirinto com o nome do arquivo recebido como argumento
	pos_t initial_pos = load_maze(argv[1]);

	// Chamar a função de navegação
	bool exit_found = walk(initial_pos);

	// Tratar o retorno (imprimir mensagem)
	if (exit_found) {
		printf("Saída encontrada!\n");
	} else {
		printf("Saída não encontrada.\n");
	}

	// Liberar a memória alocada para o labirinto
	for (int i = 0; i < num_rows; ++i) {
		delete[] maze[i];
	}
	delete[] maze;

	return 0;
}
