#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>

// Representação do labirinto
using Maze = std::vector<std::vector<char>>;

// Estrutura para representar uma posição no labirinto
struct Position {
    int row;
    int col;
};

// Variáveis globais
Maze maze;
int num_rows;
int num_cols;
std::stack<Position> valid_positions;

Position load_maze(const std::string& file_name) {
    std::ifstream arquivo(file_name);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo." << std::endl;
        return {-1, -1};
    }

   
    
      arquivo >> num_rows >> num_cols;
    maze.resize(num_rows, std::vector<char>(num_cols));

    Position start{-1, -1};
    for (int row = 0; row < num_rows; ++row) {
        for (int col = 0; col < num_cols; ++col) {
            arquivo >> maze[row][col];
            if (maze[row][col] == 'e') {
                start = {row, col}; 
            }
        }
    }

    arquivo.close();
    return start; 
}


void print_maze() {
    for (const auto& row : maze) {
        for (char cell : row) {
            std::cout << cell;
        }
        std::cout << '\n'; 
    }
}

bool is_valid_position(int row, int col) {
    return (row >= 0 && row < num_rows && col >= 0 && col < num_cols && maze[row][col] == 'x');
}

bool walk(Position pos) {
    maze[pos.row][pos.col] = '.';
    print_maze();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (maze[pos.row][pos.col] == 's') {
        return true;
    }

   
    Position directions[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; 
    for (const auto& dir : directions) {
        int new_row = pos.row + dir.row;
        int new_col = pos.col + dir.col;

        if (is_valid_position(new_row, new_col)) {
            valid_positions.push({new_row, new_col});
        }
    }

    
    while (!valid_positions.empty()) {
        Position next_pos = valid_positions.top();
        valid_positions.pop();

       
        if (walk(next_pos)) {
            return true; 
        }
    }

    return false;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_labirinto>" << std::endl;
        return 1;
    }

    Position initial_pos = load_maze(argv[1]);
    if (initial_pos.row == -1 || initial_pos.col == -1) {
        std::cerr << "Posição inicial não encontrada no labirinto." << std::endl;
        return 1;
    }

    bool exit_found = walk(initial_pos);

    if (exit_found) {
        std::cout << "Saída encontrada!" << std::endl;
    } else {
        std::cout << "Não foi possível encontrar a saída." << std::endl;
    }

    return 0;
}
