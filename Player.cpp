#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// Forward declarations
class Player;
class Game;

// Exception class declarations
class InvalidActionException : public std::runtime_error {
public:
    InvalidActionException(const std::string& message) : std::runtime_error(message) {}
};

class InsufficientCoinsException : public InvalidActionException {
public:
    InsufficientCoinsException(const std::string& message) : InvalidActionException(message) {}
};

class SanctionedException : public InvalidActionException {
public:
    SanctionedException(const std::string& message) : InvalidActionException(message) {}
};

class ConsecutiveArrestException : public InvalidActionException {
public:
    ConsecutiveArrestException(const std::string& message) : InvalidActionException(message) {}
};

class GameOverException : public InvalidActionException {
public:
    GameOverException(const std::string& message) : InvalidActionException(message) {}
};

class NotPlayerTurnException : public InvalidActionException {
public:
    NotPlayerTurnException(const std::string& message) : InvalidActionException(message) {}
};

// Base Player class
class Player {
private:
    std::string name;
    std::string role;
    int coins;
    bool active;
    bool sanctioned;
    Player* last_arrested;
    Game* game;

public:
    // Constructor
    Player(const std::string& name, const std::string& role, Game* game)
        : name(name), role(role), coins(0), active(true), sanctioned(false), last_arrested(nullptr), game(game) {}

    // Rule of Three
    // Copy constructor
    Player(const Player& other)
        : name(other.name), role(other.role), coins(other.coins), active(other.active), 
          sanctioned(other.sanctioned), last_arrested(other.last_arrested), game(other.game) {}

    // Copy assignment operator
    Player& operator=(const Player& other) {
        if (this != &other) {
            name = other.name;
            role = other.role;
            coins = other.coins;
            active = other.active;
            sanctioned = other.sanctioned;
            last_arrested = other.last_arrested;
            game = other.game;
        }
        return *this;
    }

    // Destructor
    virtual ~Player() {}

    // Basic actions
    virtual void gather();
    virtual void tax();
    virtual void bribe();
    virtual void arrest(Player& target);
    virtual void sanction(Player& target);
    virtual void coup(Player& target);

    // Utility methods
    bool is_sanctioned() const { return sanctioned; }
    void set_sanctioned(bool value) { sanctioned = value; }
    int get_coins() const { return coins; }
    
    void add_coins(int amount) {
        if (amount < 0) {
            throw std::invalid_argument("Cannot add negative coins");
        }
        coins += amount;
    }
    
    void remove_coins(int amount) {
        if (amount < 0) {
            throw std::invalid_argument("Cannot remove negative coins");
        }
        if (coins < amount) {
            throw InsufficientCoinsException("Not enough coins");
        }
        coins -= amount;
    }
    
    std::string get_name() const { return name; }
    std::string get_role() const { return role; }
    bool is_eliminated() const { return !active; }
    void eliminate() { active = false; }
    
    Player* get_last_arrested() const { return last_arrested; }
    void set_last_arrested(Player* player) { last_arrested = player; }
    
    Game* get_game() const { return game; }
    void set_game(Game* g) { game = g; }
};

// Forward declaration of Game class
class Game {
private:
    std::vector<Player*> players;
    size_t current_player_index;
    Player* last_arrested;

public:
    Game() : current_player_index(0), last_arrested(nullptr) {}
    
    // Rule of Three
    Game(const Game& other);
    Game& operator=(const Game& other);
    ~Game();
    
    void add_player(Player* player);
    std::string turn() const;
    std::vector<std::string> players_list() const;
    std::string winner() const;
    void next_turn();
    bool is_game_over() const;
    void eliminate_player(Player& player);
    Player* get_player_by_name(const std::string& name) const;
    Player* get_current_player() const;
    
    Player* get_last_arrested() const { return last_arrested; }
    void set_last_arrested(Player* player) { last_arrested = player; }
};

// Implementation of Player methods
void Player::gather() {
    if (is_sanctioned()) {
        throw SanctionedException("Player is sanctioned and cannot gather coins");
    }
    add_coins(1);
}

void Player::tax() {
    if (is_sanctioned()) {
        throw SanctionedException("Player is sanctioned and cannot tax");
    }
    add_coins(2);
}

void Player::bribe() {
    if (get_coins() < 4) {
        throw InsufficientCoinsException("Not enough coins to bribe");
    }
    remove_coins(4);
    // Logic for extra action would be implemented by the game
}

void Player::arrest(Player& target) {
    if (get_coins() < 1) {
        throw InsufficientCoinsException("Not enough coins to arrest");
    }
    
    if (get_game()->get_last_arrested() == &target) {
        throw ConsecutiveArrestException("Cannot arrest the same player in consecutive turns");
    }
    
    if (target.get_coins() < 1) {
        throw InvalidActionException("Target player has no coins to steal");
    }
    
    target.remove_coins(1);
    add_coins(1);
    get_game()->set_last_arrested(&target);
}

void Player::sanction(Player& target) {
    if (get_coins() < 3) {
        throw InsufficientCoinsException("Not enough coins to sanction");
    }
    
    remove_coins(3);
    target.set_sanctioned(true);
}

void Player::coup(Player& target) {
    if (get_coins() < 7) {
        throw InsufficientCoinsException("Not enough coins to coup");
    }
    
    remove_coins(7);
    get_game()->eliminate_player(target);
}