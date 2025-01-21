#include "GameController.h"

// Simply instantiates the game
GameController::GameController(
        const string &space_grid_file_name,
        const string &celestial_objects_file_name,
        const string &leaderboard_file_name,
        const string &player_file_name,
        const string &player_name

) {
    game = new AsteroidDash(space_grid_file_name, celestial_objects_file_name, leaderboard_file_name, player_file_name,
                            player_name);
    // TODO: Your code here, if you want to perform extra initializations
}

void GameController::play(const string &commands_file) {
    game->read_commands(commands_file);

    int grid_width = game->space_grid[0].size();
    int grid_height = game->space_grid.size();

    int tick = 0;

    while (true) {
        game->game_time++;

        if (tick >= game->commands.size()) {
            std::cout << "GAME FINISHED! No more commands!" << std::endl;
            std::cout << "Tick: " << game->game_time-2 << "\n";
            std::cout << "Lives: " << game->player->lives << "\n";
            std::cout << "Ammo: " << game->player->current_ammo << "\n";
            std::cout << " " <<  "\n";
            std::cout << " " <<  "\n";
            std::cout << "Player: " << game->player->player_name << "\n";
            for (int i = 0; i < game->grid_height; ++i) {
                for (int j = 0; j < game->grid_width; ++j) {
                    if (game->space_grid[i][j] == 1 || game->space_grid[i][j] == 2) {
                        std::cout << "██";
                    } else {
                        std::cout << "▒▒";
                    }
                }
                std::cout << "\n";
            }

            std::cout << "\n";
            game->leaderboard.print_leaderboard();
            break;
        }

        const std::string &command = game->commands[tick];
        if (command == "MOVE_UP") {
            game->player->move_up();
        } else if (command == "MOVE_DOWN") {
            game->player->move_down(grid_height);
        } else if (command == "MOVE_LEFT") {
            game->player->move_left();
        } else if (command == "MOVE_RIGHT") {
            game->player->move_right(grid_width);
        } else if (command == "SHOOT") {
            game->shoot();
        }else if (command == "PRINT_GRID") {
            game->print_space_grid();
        }

        game->update_space_grid();

        if (game->player->lives <= 0) {
            std::cout << "GAME OVER!" << std::endl;
            std::cout << "Tick: " << game->game_time-2 << "\n";
            std::cout << "Lives: " << game->player->lives << "\n";
            std::cout << "Ammo: " << game->player->current_ammo << "\n";
            std::cout << " " <<  "\n";
            std::cout << " " <<  "\n";
            std::cout << "Player: " << game->player->player_name << "\n";
            for (int i = 0; i < game->grid_height; ++i) {
                for (int j = 0; j < game->grid_width; ++j) {
                    if (game->space_grid[i][j] == 1 || game->space_grid[i][j] == 2) {
                        std::cout << "██";
                    } else {
                        std::cout << "▒▒";
                    }
                }
                std::cout << "\n";
            }

            std::cout << "\n";
            game->leaderboard.print_leaderboard();
            break;
        }
        tick++;
    }
}

GameController::~GameController() {
    delete game;
}
