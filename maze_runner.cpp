#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <stdexcept>  

using Maze = std::vector<std::vector<char>>;

struct Position {
    int row;
    int col;
};

Maze maze;
int num_rows;
int num_cols;
std::mutex maze_mutex;  
std::atomic<bool> exit_found(false);  

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
    std::lock_guard<std::mutex> lock(maze_mutex); 
    for (const auto& row : maze) {
        for (char cell : row) {
            std::cout << cell;
        }
        std::cout << '\n'; 
    }
    std::cout << std::endl;
}

bool is_valid_position(int row, int col) {
    std::lock_guard<std::mutex> lock(maze_mutex); 
    return (row >= 0 && row < num_rows && col >= 0 && col < num_cols && (maze[row][col] == 'x' || maze[row][col] == 's'));
}

bool walk(Position pos) {
      try {
          std::stack<Position> valid_positions;


         if (exit_found) {
            return false;
        }

          if (maze[pos.row][pos.col] == 's') {
           
            exit_found = true;
            return true;
        }

        {
            std::lock_guard<std::mutex> lock(maze_mutex); 
            maze[pos.row][pos.col] = '.';  
            }

        
        print_maze();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));  

        Position directions[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        std::vector<Position> new_positions;

          for (const auto& dir : directions) {
              int new_row = pos.row + dir.row;
              int new_col = pos.col + dir.col;

               if (is_valid_position(new_row, new_col)) {
                new_positions.push_back({new_row, new_col});
              }
          }


          if (new_positions.size() > 1) {
             std::vector<std::thread> threads;

           


              for (size_t i = 1; i < new_positions.size(); ++i) {
                 threads.emplace_back(walk, new_positions[i]);
             }

            
            

            if (walk(new_positions[0])) {
                return true;
            }

           
            for (auto& t : threads) {
                if (t.joinable()) {
                    t.join();
                }
            }
        } else if (!new_positions.empty()) {
           
            if (walk(new_positions[0])) {
                return true;
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Exceção capturada: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Exceção desconhecida capturada!" << std::endl;
        return false;
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

    bool exit_found_by_thread = walk(initial_pos);

    if (exit_found_by_thread || exit_found) {
        std::cout << "Saída encontrada!" << std::endl;
        print_maze(); 
    } else {
        std::cout << "Não foi possível encontrar a saída." << std::endl;
    }

    return 0;
}