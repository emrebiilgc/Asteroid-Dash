#include "Leaderboard.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

// Read the stored leaderboard status from the given file such that the "head_leaderboard_entry" member
// variable will point to the highest all-times score, and all other scores will be reachable from it
// via the "next_leaderboard_entry" member variable pointer.
void Leaderboard::read_from_file(const std::string &filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string player_name;
        unsigned long score;
        time_t last_played_time;

        if (iss >> player_name >> score) {
            if (!(iss >> last_played_time)) {
                last_played_time = time(nullptr);
            }

            LeaderboardEntry *new_entry = new LeaderboardEntry(score, last_played_time, player_name);
            new_entry->next = head_leaderboard_entry;
            head_leaderboard_entry = new_entry;
        }
    }
    infile.close();
}

// Write the latest leaderboard status to the given file in the format specified in the PA instructions
void Leaderboard::write_to_file(const std::string &filename) {
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    LeaderboardEntry *current = head_leaderboard_entry;
    while (current != nullptr) {
        outfile << current->player_name << " " << current->score << " " << current->last_played << std::endl;
        current = current->next;
    }
    outfile.close();
}

// Print the current leaderboard status to the standard output in the format specified in the PA instructions
void Leaderboard::print_leaderboard() {
    std::cout << "Leaderboard\n";
    std::cout << "-----------\n";

    LeaderboardEntry *current = head_leaderboard_entry;
    int rank = 1;

    while (current != nullptr) {
        std::tm *time_info = std::localtime(&current->last_played);
        std::cout << rank << ". " << current->player_name << " " << current->score << " "
                  << std::put_time(time_info, "%H:%M:%S/%d.%m.%Y") << std::endl;

        current = current->next;
        rank++;
    }
}

//  Insert a new LeaderboardEntry instance into the leaderboard, such that the order of the high-scores
//  is maintained, and the leaderboard size does not exceed 10 entries at any given time (only the
//  top 10 all-time high-scores should be kept in descending order by the score).
void Leaderboard::insert(LeaderboardEntry *new_entry) {
    if (head_leaderboard_entry == nullptr) {
        head_leaderboard_entry = new_entry;
        return;
    }

    if (new_entry->score > head_leaderboard_entry->score) {
        new_entry->next = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
    } else {
        LeaderboardEntry *current = head_leaderboard_entry;
        while (current->next != nullptr && current->next->score >= new_entry->score) {
            current = current->next;
        }

        new_entry->next = current->next;
        current->next = new_entry;
    }

    int count = 1;
    LeaderboardEntry *current = head_leaderboard_entry;

    while (current->next != nullptr) {
        count++;
        if (count == MAX_LEADERBOARD_SIZE) {
            if (current->next != nullptr) {
                LeaderboardEntry *to_delete = current->next;
                current->next = nullptr;
                delete to_delete;
            }
            break;
        }
        current = current->next;
    }
}

// Free dynamically allocated memory used for storing leaderboard entries
Leaderboard::~Leaderboard() {
    LeaderboardEntry *current = head_leaderboard_entry;

    while (current != nullptr) {
        LeaderboardEntry *to_delete = current;
        current = current->next;
        delete to_delete;
    }
}
