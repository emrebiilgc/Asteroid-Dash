#include "AsteroidDash.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

// Constructor to initialize AsteroidDash with the given parameters
AsteroidDash::AsteroidDash(const string &space_grid_file_name,
                           const string &celestial_objects_file_name,
                           const string &leaderboard_file_name,
                           const string &player_file_name,
                           const string &player_name)

        : leaderboard_file_name(leaderboard_file_name), leaderboard(Leaderboard()) {

    read_player(player_file_name, player_name);  // Initialize player using the player.dat file
    read_space_grid(space_grid_file_name);  // Initialize the grid after the player is loaded
    read_celestial_objects(celestial_objects_file_name);  // Load celestial objects
    leaderboard.read_from_file(leaderboard_file_name);

    game_time++;

}

// Function to read the space grid from a file
void AsteroidDash::read_space_grid(const string &input_file) {
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << input_file << std::endl;
        return;
    }

    std::string line;
    std::vector<std::vector<int>> temp_grid;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<int> row;
        int value;

        while (iss >> value) {
            row.push_back(value);
        }

        temp_grid.push_back(row);
    }

    file.close();

    grid_height = temp_grid.size();
    grid_width = temp_grid.empty() ? 0 : temp_grid[0].size();

    space_grid = std::vector<std::vector<int>>(grid_height, std::vector<int>(grid_width, 0));
}


// Function to read the player from a file
void AsteroidDash::read_player(const string &player_file_name, const string &player_name) {
    std::ifstream file(player_file_name);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << player_file_name << std::endl;
        return;
    }

    std::string line;
    int row = 0, col = 0;
    vector<vector<bool>> spacecraft_shape;

    if (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> row >> col;
    }

    while (std::getline(file, line)) {
        std::vector<bool> shape_row;
        for (char ch : line) {
            if (ch == '1') shape_row.push_back(true);
            else if (ch == '0') shape_row.push_back(false);
        }
        spacecraft_shape.push_back(shape_row);
    }

    file.close();

    player = new Player(spacecraft_shape, row, col, player_name);
}

// Function to read celestial objects from a file
void AsteroidDash::read_celestial_objects(const std::string &input_file) {
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << input_file << std::endl;
        return;
    }

    std::string line;
    CelestialObject *last_object = nullptr;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        ObjectType type = (line[0] == '[') ? ASTEROID : LIFE_UP;

        std::vector<std::vector<bool>> shape;
        do {
            std::vector<bool> row;
            for (char ch : line) {
                if (ch == '1') row.push_back(true);
                else if (ch == '0') row.push_back(false);
            }
            shape.push_back(row);
        } while (std::getline(file, line) && (line[0] == '0' || line[0] == '1'));

        int start_row = 0, time_of_appearance = 0;
        if (!line.empty() && line[0] == 's') {
            start_row = std::stoi(line.substr(2));
            std::getline(file, line);
        }
        if (!line.empty() && line[0] == 't') {
            time_of_appearance = std::stoi(line.substr(2));
            std::getline(file, line);
        }
        if (!line.empty() && line[0] == 'e') {
            type = (line.substr(2) == "ammo") ? AMMO : LIFE_UP;
        }

        CelestialObject* new_object = new CelestialObject(shape, type, start_row, time_of_appearance);
        if (celestial_objects_list_head) {
            last_object->next_celestial_object = new_object;
            last_object->right_rotation->next_celestial_object = last_object->next_celestial_object;
            last_object->right_rotation->right_rotation->next_celestial_object = last_object->next_celestial_object;
            last_object->right_rotation->right_rotation->right_rotation->next_celestial_object = last_object->next_celestial_object;

        } else {
            celestial_objects_list_head = new_object;
        }
        last_object = new_object;
    }

    file.close();
}

void AsteroidDash::read_commands(const string &commands_file) {
    std::ifstream file(commands_file);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << commands_file << std::endl;
        return;
    }

    std::string command;
    while (std::getline(file, command)) {
        commands.push_back(command);
    }
    file.close();
}

void AsteroidDash::place_player(const vector<vector<bool>>& spacecraft_shape, int start_row, int start_col) {
    for (int i = 0; i < spacecraft_shape.size(); ++i) {
        for (int j = 0; j < spacecraft_shape[i].size(); ++j) {
            if (spacecraft_shape[i][j]) {
                space_grid[start_row + i][start_col + j] = 1;
            }
        }
    }
}

// Print the entire space grid
void AsteroidDash::print_space_grid() const {

    std::cout << "Tick: " << game_time -2<< std::endl;
    std::cout << "Lives: " << player->lives << std::endl;
    std::cout << "Ammo: " << player->current_ammo << std::endl;
    std::cout << "Score: " <<  std::endl;
    std::cout << "High Score: " << std::endl;

    for (int i = 0; i < grid_height; ++i) {
        for (int j = 0; j < grid_width; ++j) {
            if (space_grid[i][j] == 1 || space_grid[i][j] == 2) {
                std::cout << occupiedCellChar;
            } else {
                std::cout << unoccupiedCellChar;
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Function to update the space grid with player, celestial objects, and any other changes
// It is called in every game tick before moving on to the next tick.
void AsteroidDash::update_space_grid() {

    if (game_over) return;

    for (int i = 0; i < grid_height; ++i) {
        for (int j = 0; j < grid_width; ++j) {
            space_grid[i][j] = 0;
        }
    }

    place_player(player->spacecraft_shape, player->position_row, player->position_col);

    CelestialObject* celestial = celestial_objects_list_head;

    while (celestial != nullptr) {
        if (game_time >= celestial->time_of_appearance+1) {

            int elapsed_ticks = game_time - celestial->time_of_appearance-1;
            int start_column = grid_width - 1 - elapsed_ticks;
            int columns_to_display = std::min(elapsed_ticks + 1, static_cast<int>(celestial->shape[0].size()));
            bool collided_with_player = false;

            for (int i = 0; i < celestial->shape.size() && !collided_with_player; ++i) {
                for (int j = 0; j < columns_to_display && !collided_with_player; ++j) {
                    int row = celestial->starting_row + i;
                    int col = start_column + j;

                    if (row >= 0 && row < grid_height && col >= 0 && col < grid_width) {
                        if (celestial->shape[i][j] && space_grid[row][col] == 1) {
                            if (celestial->object_type == ASTEROID) {
                                player->lives--;
                                collided_with_player = true;

                                if (player->lives <= 0) {
                                    game_over = true;

                                    return;
                                }
                            } else if (celestial->object_type == AMMO) {
                                player->current_ammo=player->max_ammo;
                            } else if (celestial->object_type == LIFE_UP) {
                                player->lives++;
                            }
                            celestial->time_of_appearance = -1;
                        }
                    }
                }
            }

            if (!collided_with_player && !game_over) {
                for (int i = 0; i < celestial->shape.size(); ++i) {
                    for (int j = 0; j < columns_to_display; ++j) {
                        int row = celestial->starting_row + i;
                        int col = start_column + j;
                        if (row >= 0 && row < grid_height && col >= 0 && col < grid_width && celestial->shape[i][j]) {
                            space_grid[row][col] = (celestial->object_type == ASTEROID) ? 1 : 2;
                        }
                    }
                }
            }
        }

        celestial = celestial->next_celestial_object;
    }

    if (!game_over) {
        for (auto ammo_iterator = player->active_ammos.begin(); ammo_iterator != player->active_ammos.end();) {
            ammo_iterator->col += 1;

            int ammo_row = ammo_iterator->row;
            int ammo_col = ammo_iterator->col;
            if (ammo_col >= grid_width) {
                ammo_iterator = player->active_ammos.erase(ammo_iterator);
            } else if (ammo_row >= 0 && ammo_row < grid_height && ammo_col >= 0 && ammo_col < grid_width) {
                if (space_grid[ammo_row][ammo_col] == 1) {
                    ammo_iterator = player->active_ammos.erase(ammo_iterator);
                } else {
                    space_grid[ammo_row][ammo_col] = 1;
                    ++ammo_iterator;
                }
            } else {
                ++ammo_iterator;
            }
        }
    }

}

// Corresponds to the SHOOT command.
// It should shoot if the player has enough ammo.
// It should decrease the player's ammo
void AsteroidDash::shoot() {
    if (player->current_ammo > 0) {
        int middle_row = player->position_row + player->spacecraft_shape.size() / 2;

        int rightmost_col = -1;
        for (int col = player->spacecraft_shape[0].size() - 1; col >= 0; --col) {
            if (player->spacecraft_shape[middle_row - player->position_row][col]) {
                rightmost_col = col;
                break;
            }
        }

        if (rightmost_col != -1) {
            player->active_ammos.push_back({middle_row, player->position_col + rightmost_col});
            player->current_ammo--;
        }
    }
}


// Destructor. Remove dynamically allocated member variables here.
AsteroidDash::~AsteroidDash() {
    delete player;

    CelestialObject *current = celestial_objects_list_head;
    while (current != nullptr) {
        CelestialObject *next_object = current->next_celestial_object;

        CelestialObject *rotation = current;
        CelestialObject *first_rotation = current;
        do {
            CelestialObject *next_rotation = rotation->right_rotation;
            delete rotation;
            rotation = next_rotation;
        } while (rotation != first_rotation);

        current = next_object;
    }
}