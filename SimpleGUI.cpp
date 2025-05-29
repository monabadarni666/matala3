#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Game.cpp"

// Simple console-based UI for the Coup game
class ConsoleUI {
private:
    Game game;
    std::vector<std::string> history;

    void addToHistory(const std::string& action) {
        history.push_back(action);
        if (history.size() > 10) {
            history.erase(history.begin());
        }
    }

    void displayHistory() {
        std::cout << "\n===== Game History =====\n";
        for (const auto& entry : history) {
            std::cout << "- " << entry << std::endl;
        }
        std::cout << "========================\n";
    }

    void displayPlayerInfo(Player* player, bool isCurrent) {
        std::string prefix = isCurrent ? "-> " : "   ";
        std::string status = player->is_eliminated() ? " [ELIMINATED]" : 
                             player->is_sanctioned() ? " [SANCTIONED]" : "";
        
        std::cout << prefix << player->get_name() << " (" 
                  << player->get_role() << ") - " 
                  << player->get_coins() << " coins" 
                  << status << std::endl;
    }

    void displayGameState() {
        std::cout << "\n===== Game State =====\n";
        
        if (game.is_game_over()) {
            std::cout << "Game Over! Winner: " << game.winner() << std::endl;
        } else {
            std::cout << "Current Player: " << game.turn() << std::endl;
        }
        
        std::cout << "\nPlayers:\n";
        for (const auto& name : game.players_list()) {
            Player* player = game.get_player_by_name(name);
            bool isCurrent = (name == game.turn());
            displayPlayerInfo(player, isCurrent);
        }
        
        std::cout << "=====================\n";
    }

    std::string getPlayerRoleSpecialAbility(const std::string& role) {
        if (role == "Governor") return "Block Tax";
        if (role == "Spy") return "View Coins";
        if (role == "Baron") return "Invest (3 coins -> 6 coins)";
        if (role == "General") return "Protect from Coup (5 coins)";
        if (role == "Judge") return "Block Bribe";
        if (role == "Merchant") return "Get Bonus Coin";
        return "None";
    }

    Player* selectTarget() {
        std::vector<std::string> playerNames = game.players_list();
        std::string currentPlayer = game.turn();
        
        // Remove current player from the list
        playerNames.erase(std::remove(playerNames.begin(), playerNames.end(), currentPlayer), playerNames.end());
        
        if (playerNames.empty()) {
            std::cout << "No other players to target!" << std::endl;
            return nullptr;
        }
        
        std::cout << "Select target player:\n";
        for (size_t i = 0; i < playerNames.size(); i++) {
            std::cout << (i + 1) << ". " << playerNames[i] << std::endl;
        }
        
        int choice;
        std::cout << "Enter choice (1-" << playerNames.size() << "): ";
        std::cin >> choice;
        
        if (choice < 1 || choice > static_cast<int>(playerNames.size())) {
            std::cout << "Invalid choice!" << std::endl;
            return nullptr;
        }
        
        return game.get_player_by_name(playerNames[choice - 1]);
    }

    void performSpecialAbility(Player* player) {
        std::string role = player->get_role();
        
        if (role == "Governor") {
            Player* target = selectTarget();
            if (target) {
                dynamic_cast<Governor*>(player)->block_tax(*target);
                addToHistory(player->get_name() + " (Governor) blocked " + target->get_name() + "'s tax action");
            }
        } 
        else if (role == "Spy") {
            Player* target = selectTarget();
            if (target) {
                int coins = dynamic_cast<Spy*>(player)->view_coins(*target);
                addToHistory(player->get_name() + " (Spy) viewed that " + target->get_name() + " has " + std::to_string(coins) + " coins");
            }
        }
        else if (role == "Baron") {
            try {
                dynamic_cast<Baron*>(player)->invest();
                addToHistory(player->get_name() + " (Baron) invested 3 coins to get 6 coins");
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << std::endl;
            }
        }
        else if (role == "General") {
            Player* target = selectTarget();
            if (target) {
                try {
                    dynamic_cast<General*>(player)->protect(*target);
                    addToHistory(player->get_name() + " (General) protected " + target->get_name() + " from a coup");
                } catch (const std::exception& e) {
                    std::cout << "Error: " << e.what() << std::endl;
                }
            }
        }
        else if (role == "Judge") {
            Player* target = selectTarget();
            if (target) {
                dynamic_cast<Judge*>(player)->block_bribe(*target);
                addToHistory(player->get_name() + " (Judge) blocked " + target->get_name() + "'s bribe");
            }
        }
        else if (role == "Merchant") {
            dynamic_cast<Merchant*>(player)->bonus();
            addToHistory(player->get_name() + " (Merchant) received a bonus coin");
        }
    }

public:
    ConsoleUI() {
        // Create players with different roles
        Player* alice = new Governor("Alice", &game);
        Player* bob = new Spy("Bob", &game);
        Player* charlie = new Baron("Charlie", &game);
        Player* diana = new General("Diana", &game);
        Player* ethan = new Judge("Ethan", &game);
        Player* fiona = new Merchant("Fiona", &game);
        
        // Add players to game
        game.add_player(alice);
        game.add_player(bob);
        game.add_player(charlie);
        game.add_player(diana);
        game.add_player(ethan);
        game.add_player(fiona);
        
        addToHistory("Game started with 6 players");
    }
    
    void run() {
        while (!game.is_game_over()) {
            displayGameState();
            displayHistory();
            
            Player* currentPlayer = game.get_current_player();
            std::string currentPlayerName = currentPlayer->get_name();
            std::string currentPlayerRole = currentPlayer->get_role();
            
            // Display available actions
            std::cout << "\nAvailable Actions:\n";
            std::cout << "1. Gather (take 1 coin)\n";
            std::cout << "2. Tax (take 2-3 coins)\n";
            std::cout << "3. Bribe (pay 4 coins)\n";
            std::cout << "4. Arrest (steal 1 coin from another player)\n";
            std::cout << "5. Sanction (prevent player from economic actions, costs 3 coins)\n";
            std::cout << "6. Coup (eliminate player, costs 7 coins)\n";
            std::cout << "7. Special: " << getPlayerRoleSpecialAbility(currentPlayerRole) << "\n";
            std::cout << "8. Next Turn\n";
            std::cout << "0. Exit Game\n";
            
            // Get player choice
            int choice;
            std::cout << "\nEnter choice (0-8): ";
            std::cin >> choice;
            
            try {
                switch (choice) {
                    case 0: // Exit
                        std::cout << "Exiting game..." << std::endl;
                        return;
                        
                    case 1: // Gather
                        currentPlayer->gather();
                        addToHistory(currentPlayerName + " gathered 1 coin");
                        break;
                        
                    case 2: // Tax
                        currentPlayer->tax();
                        if (currentPlayerRole == "Governor") {
                            addToHistory(currentPlayerName + " (Governor) taxed 3 coins");
                        } else {
                            addToHistory(currentPlayerName + " taxed 2 coins");
                        }
                        break;
                        
                    case 3: // Bribe
                        currentPlayer->bribe();
                        addToHistory(currentPlayerName + " paid 4 coins to bribe");
                        break;
                        
                    case 4: { // Arrest
                        Player* target = selectTarget();
                        if (target) {
                            currentPlayer->arrest(*target);
                            addToHistory(currentPlayerName + " arrested " + target->get_name() + " and stole 1 coin");
                        }
                        break;
                    }
                    
                    case 5: { // Sanction
                        Player* target = selectTarget();
                        if (target) {
                            currentPlayer->sanction(*target);
                            addToHistory(currentPlayerName + " sanctioned " + target->get_name());
                        }
                        break;
                    }
                    
                    case 6: { // Coup
                        Player* target = selectTarget();
                        if (target) {
                            currentPlayer->coup(*target);
                            addToHistory(currentPlayerName + " performed a coup on " + target->get_name() + " and eliminated them");
                        }
                        break;
                    }
                    
                    case 7: // Special ability
                        performSpecialAbility(currentPlayer);
                        break;
                        
                    case 8: // Next turn
                        game.next_turn();
                        addToHistory(currentPlayerName + "'s turn ended. Now " + game.turn() + "'s turn.");
                        break;
                        
                    default:
                        std::cout << "Invalid choice!" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << std::endl;
                std::cout << "Press Enter to continue...";
                std::cin.ignore();
                std::cin.get();
            }
            
            // Check if game is over after each action
            if (game.is_game_over()) {
                displayGameState();
                displayHistory();
                std::cout << "\nGame Over! " << game.winner() << " is the winner!" << std::endl;
                break;
            }
        }
    }
};

int main() {
    std::cout << "=== Welcome to Coup Game ===\n" << std::endl;
    
    ConsoleUI ui;
    ui.run();
    
    std::cout << "\nThanks for playing!" << std::endl;
    return 0;
}