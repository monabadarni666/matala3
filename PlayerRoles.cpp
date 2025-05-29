#include "Player.cpp"

// Governor: Takes 3 coins instead of 2 when performing tax, can block tax actions
class Governor : public Player {
public:
    Governor(const std::string& name, Game* game)
        : Player(name, "Governor", game) {}
    
    // Override tax to take 3 coins instead of 2
    void tax() override {
        if (is_sanctioned()) {
            throw SanctionedException("Player is sanctioned and cannot tax");
        }
        add_coins(3); // Governor takes 3 coins instead of 2
    }
    
    // Special ability: Block another player's tax action
    void block_tax(Player& target) {
        // Implementation would depend on how we track actions
        // This is a placeholder for the actual implementation
        std::cout << get_name() << " blocked " << target.get_name() << "'s tax action" << std::endl;
    }
};

// Spy: Can view another player's coin count and block their arrest action
class Spy : public Player {
public:
    Spy(const std::string& name, Game* game)
        : Player(name, "Spy", game) {}
    
    // Special ability: View target player's coin count
    int view_coins(Player& target) {
        return target.get_coins();
    }
    
    // Special ability: Block a future arrest attempt
    void block_arrest(Player& target) {
        // Implementation would depend on how we track actions
        // This is a placeholder for the actual implementation
        std::cout << get_name() << " blocked " << target.get_name() << "'s arrest action" << std::endl;
    }
};

// Baron: Can "invest" coins and gets compensation when sanctioned
class Baron : public Player {
public:
    Baron(const std::string& name, Game* game)
        : Player(name, "Baron", game) {}
    
    // Special ability: Invest coins
    void invest() {
        if (get_coins() < 3) {
            throw InsufficientCoinsException("Not enough coins to invest");
        }
        
        remove_coins(3);
        add_coins(6); // Return on investment: 6 coins
    }
    
    // Special ability: Get compensation when sanctioned
    void compensate() {
        if (is_sanctioned()) {
            add_coins(1); // Get 1 coin as compensation
        }
    }
};

// General: Can protect against coups and recover from arrests
class General : public Player {
public:
    General(const std::string& name, Game* game)
        : Player(name, "General", game) {}
    
    // Special ability: Protect against coup
    void protect(Player& target) {
        if (get_coins() < 5) {
            throw InsufficientCoinsException("Not enough coins to protect");
        }
        
        remove_coins(5);
        // Logic to prevent the coup would be implemented by the game
        std::cout << get_name() << " protected " << target.get_name() << " from a coup" << std::endl;
    }
    
    // Special ability: Regain coin lost from being arrested
    void recover_arrest() {
        add_coins(1); // Regain the coin lost from being arrested
    }
};

// Judge: Can block bribes and penalize sanctions
class Judge : public Player {
public:
    Judge(const std::string& name, Game* game)
        : Player(name, "Judge", game) {}
    
    // Special ability: Block bribe and cause the player to lose coins
    void block_bribe(Player& target) {
        // The target has already paid 4 coins, we're not adding them back
        std::cout << get_name() << " blocked " << target.get_name() << "'s bribe" << std::endl;
    }
    
    // Special ability: Force sanctioning player to pay extra coin
    void penalize_sanction(Player& target) {
        if (target.get_coins() < 1) {
            throw InvalidActionException("Target player has no coins to penalize");
        }
        
        target.remove_coins(1);
        std::cout << get_name() << " forced " << target.get_name() << " to pay an extra coin for sanctioning" << std::endl;
    }
};

// Merchant: Gets bonus coins and pays pot instead of other players
class Merchant : public Player {
public:
    Merchant(const std::string& name, Game* game)
        : Player(name, "Merchant", game) {}
    
    // Special ability: Get bonus coin at start of turn
    void bonus() {
        if (get_coins() >= 3) {
            add_coins(1); // Get 1 extra coin if starting with 3+ coins
        }
    }
    
    // Override arrest to pay pot instead of another player
    void arrest(Player& target) override {
        if (get_coins() < 1) {
            throw InsufficientCoinsException("Not enough coins to arrest");
        }
        
        if (get_game()->get_last_arrested() == &target) {
            throw ConsecutiveArrestException("Cannot arrest the same player in consecutive turns");
        }
        
        // Merchant pays 2 coins to the pot when arrested
        remove_coins(2);
        
        get_game()->set_last_arrested(&target);
        std::cout << get_name() << " paid 2 coins to the pot instead of giving to " << target.get_name() << std::endl;
    }
};