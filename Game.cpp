#include "PlayerRoles.cpp"
#include <algorithm>

// Implementation of Game methods
Game::Game(const Game& other) : current_player_index(other.current_player_index), last_arrested(other.last_arrested) {
    // Deep copy of players
    for (const auto& player : other.players) {
        Player* new_player = nullptr;
        
        if (player->get_role() == "Governor") {
            new_player = new Governor(player->get_name(), this);
        } else if (player->get_role() == "Spy") {
            new_player = new Spy(player->get_name(), this);
        } else if (player->get_role() == "Baron") {
            new_player = new Baron(player->get_name(), this);
        } else if (player->get_role() == "General") {
            new_player = new General(player->get_name(), this);
        } else if (player->get_role() == "Judge") {
            new_player = new Judge(player->get_name(), this);
        } else if (player->get_role() == "Merchant") {
            new_player = new Merchant(player->get_name(), this);
        } else {
            new_player = new Player(*player);
            new_player->set_game(this);
        }
        
        players.push_back(new_player);
    }
}

Game& Game::operator=(const Game& other) {
    if (this != &other) {
        // Clean up existing players
        for (auto player : players) {
            delete player;
        }
        players.clear();
        
        // Copy other game's players
        for (const auto& player : other.players) {
            Player* new_player = nullptr;
            
            if (player->get_role() == "Governor") {
                new_player = new Governor(player->get_name(), this);
            } else if (player->get_role() == "Spy") {
                new_player = new Spy(player->get_name(), this);
            } else if (player->get_role() == "Baron") {
                new_player = new Baron(player->get_name(), this);
            } else if (player->get_role() == "General") {
                new_player = new General(player->get_name(), this);
            } else if (player->get_role() == "Judge") {
                new_player = new Judge(player->get_name(), this);
            } else if (player->get_role() == "Merchant") {
                new_player = new Merchant(player->get_name(), this);
            } else {
                new_player = new Player(*player);
                new_player->set_game(this);
            }
            
            players.push_back(new_player);
        }
        
        current_player_index = other.current_player_index;
        last_arrested = other.last_arrested;
    }
    return *this;
}

Game::~Game() {
    // Clean up all player objects
    for (auto player : players) {
        delete player;
    }
}

void Game::add_player(Player* player) {
    players.push_back(player);
}

std::string Game::turn() const {
    if (players.empty()) {
        throw std::runtime_error("No players in the game");
    }
    
    if (is_game_over()) {
        throw GameOverException("Game is already over");
    }
    
    return players[current_player_index]->get_name();
}

std::vector<std::string> Game::players_list() const {
    std::vector<std::string> player_names;
    for (const auto& player : players) {
        if (!player->is_eliminated()) {
            player_names.push_back(player->get_name());
        }
    }
    return player_names;
}

std::string Game::winner() const {
    if (!is_game_over()) {
        throw std::runtime_error("Game is not over yet");
    }
    
    // Find the single non-eliminated player
    for (const auto& player : players) {
        if (!player->is_eliminated()) {
            return player->get_name();
        }
    }
    
    throw std::runtime_error("No winner found");
}

void Game::next_turn() {
    if (is_game_over()) {
        throw GameOverException("Game is already over");
    }
    
    // Reset sanction status of current player before moving to next
    players[current_player_index]->set_sanctioned(false);
    
    // Special case: Merchant gets bonus coin at start of turn if they have 3+ coins
    if (players[current_player_index]->get_role() == "Merchant") {
        Merchant* merchant = dynamic_cast<Merchant*>(players[current_player_index]);
        if (merchant) {
            merchant->bonus();
        }
    }
    
    // Special case: Baron gets compensation if sanctioned
    if (players[current_player_index]->get_role() == "Baron" && 
        players[current_player_index]->is_sanctioned()) {
        Baron* baron = dynamic_cast<Baron*>(players[current_player_index]);
        if (baron) {
            baron->compensate();
        }
    }
    
    // Move to next active player
    do {
        current_player_index = (current_player_index + 1) % players.size();
    } while (players[current_player_index]->is_eliminated());
    
    // Check if current player has 10+ coins - must perform coup
    if (players[current_player_index]->get_coins() >= 10) {
        std::cout << players[current_player_index]->get_name() 
                  << " has 10+ coins and must perform a coup!" << std::endl;
    }
}

bool Game::is_game_over() const {
    // Count active players
    int active_players = 0;
    for (const auto& player : players) {
        if (!player->is_eliminated()) {
            active_players++;
        }
    }
    
    // Game is over when only one player remains
    return active_players <= 1;
}

void Game::eliminate_player(Player& player) {
    if (player.is_eliminated()) {
        throw std::runtime_error("Player is already eliminated");
    }
    
    player.eliminate();
}

Player* Game::get_player_by_name(const std::string& name) const {
    for (const auto& player : players) {
        if (player->get_name() == name) {
            return player;
        }
    }
    return nullptr;
}

Player* Game::get_current_player() const {
    if (players.empty()) {
        throw std::runtime_error("No players in the game");
    }
    
    return players[current_player_index];
}