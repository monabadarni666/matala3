#include "Game.cpp"
#include <iostream>

// Helper function to display game state
void display_game_state(const Game& game) {
    std::cout << "\n=== Game State ===" << std::endl;
    
    std::cout << "Current turn: " << game.turn() << std::endl;
    
    std::cout << "Active players:" << std::endl;
    for (const std::string& player_name : game.players_list()) {
        Player* player = game.get_player_by_name(player_name);
        std::cout << "  - " << player->get_name() 
                  << " (" << player->get_role() 
                  << ") - " << player->get_coins() << " coins"
                  << (player->is_sanctioned() ? " [SANCTIONED]" : "")
                  << std::endl;
    }
    
    std::cout << "==================\n" << std::endl;
}

int main() {
    std::cout << "=== Welcome to Coup Game Demo ===" << std::endl;
    
    // Create a new game
    Game game;
    
    // Add players with different roles
    game.add_player(new Governor("Alice", &game));
    game.add_player(new Spy("Bob", &game));
    game.add_player(new Baron("Charlie", &game));
    game.add_player(new General("Diana", &game));
    game.add_player(new Judge("Ethan", &game));
    game.add_player(new Merchant("Fiona", &game));
    
    std::cout << "Game started with " << game.players_list().size() << " players" << std::endl;
    
    // Initial game state
    display_game_state(game);
    
    // Simulate a few rounds
    try {
        // Round 1: Everyone takes an action
        std::cout << "=== Round 1 ===" << std::endl;
        
        // Alice (Governor) takes tax (3 coins)
        std::cout << "Alice (Governor) uses tax and gets 3 coins" << std::endl;
        dynamic_cast<Governor*>(game.get_player_by_name("Alice"))->tax();
        game.next_turn();
        
        // Bob (Spy) views Charlie's coins
        std::cout << "Bob (Spy) views Charlie's coins: " 
                  << dynamic_cast<Spy*>(game.get_player_by_name("Bob"))->view_coins(*game.get_player_by_name("Charlie"))
                  << std::endl;
        game.get_player_by_name("Bob")->gather(); // Also gather a coin
        game.next_turn();
        
        // Charlie (Baron) gathers a coin
        std::cout << "Charlie (Baron) gathers a coin" << std::endl;
        game.get_player_by_name("Charlie")->gather();
        game.next_turn();
        
        // Diana (General) gathers a coin
        std::cout << "Diana (General) gathers a coin" << std::endl;
        game.get_player_by_name("Diana")->gather();
        game.next_turn();
        
        // Ethan (Judge) gathers a coin
        std::cout << "Ethan (Judge) gathers a coin" << std::endl;
        game.get_player_by_name("Ethan")->gather();
        game.next_turn();
        
        // Fiona (Merchant) gathers a coin
        std::cout << "Fiona (Merchant) gathers a coin" << std::endl;
        game.get_player_by_name("Fiona")->gather();
        game.next_turn();
        
        display_game_state(game);
        
        // Round 2: More complex actions
        std::cout << "=== Round 2 ===" << std::endl;
        
        // Alice (Governor) taxes again
        std::cout << "Alice (Governor) uses tax and gets 3 coins" << std::endl;
        dynamic_cast<Governor*>(game.get_player_by_name("Alice"))->tax();
        game.next_turn();
        
        // Bob (Spy) arrests Charlie
        std::cout << "Bob (Spy) arrests Charlie" << std::endl;
        game.get_player_by_name("Bob")->arrest(*game.get_player_by_name("Charlie"));
        game.next_turn();
        
        // Charlie (Baron) invests (first add coins since he lost one to arrest)
        std::cout << "Charlie (Baron) gets extra coins and invests 3 coins to get 6" << std::endl;
        game.get_player_by_name("Charlie")->add_coins(3); // Give him enough coins
        dynamic_cast<Baron*>(game.get_player_by_name("Charlie"))->invest();
        game.next_turn();
        
        // Diana (General) sanctions Ethan
        std::cout << "Diana (General) gets extra coins and sanctions Ethan" << std::endl;
        game.get_player_by_name("Diana")->add_coins(2); // Give her enough coins
        game.get_player_by_name("Diana")->sanction(*game.get_player_by_name("Ethan"));
        game.next_turn();
        
        // Ethan (Judge) tries to gather while sanctioned
        std::cout << "Ethan (Judge) tries to gather while sanctioned" << std::endl;
        try {
            game.get_player_by_name("Ethan")->gather();
        } catch (const SanctionedException& e) {
            std::cout << "Exception: " << e.what() << std::endl;
        }
        game.next_turn();
        
        // Fiona (Merchant) uses tax
        std::cout << "Fiona (Merchant) uses tax" << std::endl;
        game.get_player_by_name("Fiona")->tax();
        game.next_turn();
        
        display_game_state(game);
        
        // Round 3: Coup demonstration
        std::cout << "=== Round 3 ===" << std::endl;
        
        // Give Alice enough coins for a coup
        std::cout << "Giving Alice enough coins for a coup" << std::endl;
        game.get_player_by_name("Alice")->add_coins(4); // Now has 10 coins
        
        // Alice (Governor) must coup with 10 coins
        std::cout << "Alice (Governor) coups Bob" << std::endl;
        game.get_player_by_name("Alice")->coup(*game.get_player_by_name("Bob"));
        game.next_turn();
        
        // Skip Bob's turn as he's eliminated
        
        // Charlie (Baron) gathers
        std::cout << "Charlie (Baron) gathers a coin" << std::endl;
        game.get_player_by_name("Charlie")->gather();
        game.next_turn();
        
        // Diana (General) gathers
        std::cout << "Diana (General) gathers a coin" << std::endl;
        game.get_player_by_name("Diana")->gather();
        game.next_turn();
        
        // Ethan (Judge) is no longer sanctioned and gathers
        std::cout << "Ethan (Judge) is no longer sanctioned and gathers" << std::endl;
        game.get_player_by_name("Ethan")->gather();
        game.next_turn();
        
        // Fiona (Merchant) gathers
        std::cout << "Fiona (Merchant) gathers and gets bonus coin" << std::endl;
        game.get_player_by_name("Fiona")->gather();
        game.next_turn();
        
        display_game_state(game);
        
        // Continue the game until there's a winner
        std::cout << "\n=== Fast forward to end game ===" << std::endl;
        
        // Simulating more coups to speed up the game
        std::cout << "Giving Charlie enough coins for a coup" << std::endl;
        game.get_player_by_name("Charlie")->add_coins(5); // Now has enough for coup
        
        std::cout << "Charlie (Baron) coups Diana" << std::endl;
        game.get_player_by_name("Charlie")->coup(*game.get_player_by_name("Diana"));
        game.next_turn();
        
        std::cout << "Giving Ethan enough coins for a coup" << std::endl;
        game.get_player_by_name("Ethan")->add_coins(6); // Now has enough for coup
        
        std::cout << "Ethan (Judge) coups Fiona" << std::endl;
        game.get_player_by_name("Ethan")->coup(*game.get_player_by_name("Fiona"));
        game.next_turn();
        
        std::cout << "Giving Alice enough coins for a coup" << std::endl;
        game.get_player_by_name("Alice")->add_coins(7); // Now has enough for coup
        
        std::cout << "Alice (Governor) coups Charlie" << std::endl;
        game.get_player_by_name("Alice")->coup(*game.get_player_by_name("Charlie"));
        game.next_turn();
        
        std::cout << "Giving Ethan enough coins for a coup" << std::endl;
        game.get_player_by_name("Ethan")->add_coins(7); // Now has enough for coup
        
        std::cout << "Final coup: Ethan (Judge) coups Alice" << std::endl;
        game.get_player_by_name("Ethan")->coup(*game.get_player_by_name("Alice"));
        
        // Game is over, Ethan is the winner
        std::cout << "\n=== Game Over ===" << std::endl;
        std::cout << "Winner: " << game.winner() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}