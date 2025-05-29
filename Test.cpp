#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "Game.cpp"

TEST_CASE("Player basic operations") {
    Game game;
    Player player("TestPlayer", "TestRole", &game);
    
    CHECK_EQ(player.get_name(), "TestPlayer");
    CHECK_EQ(player.get_role(), "TestRole");
    CHECK_EQ(player.get_coins(), 0);
    CHECK_FALSE(player.is_eliminated());
    CHECK_FALSE(player.is_sanctioned());
    
    // Test adding coins
    player.add_coins(5);
    CHECK_EQ(player.get_coins(), 5);
    
    // Test removing coins
    player.remove_coins(3);
    CHECK_EQ(player.get_coins(), 2);
    
    // Test elimination
    player.eliminate();
    CHECK(player.is_eliminated());
}

TEST_CASE("Player coin operations exceptions") {
    Game game;
    Player player("TestPlayer", "TestRole", &game);
    
    // Test insufficient coins exception
    player.add_coins(2);
    CHECK_THROWS_AS(player.remove_coins(3), InsufficientCoinsException);
    
    // Test invalid coin amount
    CHECK_THROWS_AS(player.add_coins(-1), std::invalid_argument);
    CHECK_THROWS_AS(player.remove_coins(-1), std::invalid_argument);
}

TEST_CASE("Basic actions") {
    Game game;
    Governor* alice = new Governor("Alice", &game);
    Spy* bob = new Spy("Bob", &game);
    Baron* charlie = new Baron("Charlie", &game);
    General* diana = new General("Diana", &game);
    
    game.add_player(alice);
    game.add_player(bob);
    game.add_player(charlie);
    game.add_player(diana);
    
    // Test gather
    alice->gather();
    CHECK_EQ(alice->get_coins(), 1);
    
    // Test tax (Governor gets 3 coins)
    alice->tax();
    CHECK_EQ(alice->get_coins(), 4);
    
    // Test sanction
    alice->sanction(*bob);
    CHECK_EQ(alice->get_coins(), 1); // 4 - 3 = 1
    CHECK(bob->is_sanctioned());
    
    // Test sanctioned player can't gather
    CHECK_THROWS_AS(bob->gather(), SanctionedException);
    
    // Test arrest
    charlie->add_coins(1);
    diana->add_coins(1); // Make sure Diana has at least 1 coin
    diana->arrest(*charlie);
    CHECK_EQ(diana->get_coins(), 2); // 1 + 1 = 2
    CHECK_EQ(charlie->get_coins(), 0); // 1 - 1 = 0
    
    // Test consecutive arrest exception
    diana->add_coins(1);
    CHECK_THROWS_AS(diana->arrest(*charlie), ConsecutiveArrestException);
    
    // Clean up (Game destructor will handle this in real code)
    // No manual cleanup needed since game will clean up
}

TEST_CASE("Governor role abilities") {
    Game game;
    Governor governor("TestGovernor", &game);
    Player regular("TestPlayer", "Regular", &game);
    
    // Test governor gets 3 coins for tax
    governor.tax();
    CHECK_EQ(governor.get_coins(), 3);
    
    // Regular player gets 2 coins for tax
    regular.tax();
    CHECK_EQ(regular.get_coins(), 2);
}

TEST_CASE("Baron role abilities") {
    Game game;
    Baron baron("TestBaron", &game);
    
    // Test invest ability
    baron.add_coins(3);
    baron.invest();
    CHECK_EQ(baron.get_coins(), 6); // 3 invested to get 6
    
    // Test compensation when sanctioned
    baron.set_sanctioned(true);
    baron.compensate();
    CHECK_EQ(baron.get_coins(), 7); // 6 + 1 = 7
}

TEST_CASE("Spy role abilities") {
    Game game;
    Spy spy("TestSpy", &game);
    Player target("Target", "Regular", &game);
    
    // Test viewing coins
    target.add_coins(5);
    CHECK_EQ(spy.view_coins(target), 5);
}

TEST_CASE("General role abilities") {
    Game game;
    General general("TestGeneral", &game);
    Player attacker("Attacker", "Regular", &game);
    Player target("Target", "Regular", &game);
    
    // Test protection (costs 5 coins)
    general.add_coins(5);
    general.protect(target);
    CHECK_EQ(general.get_coins(), 0);
    
    // Test recovery from arrest
    general.add_coins(1); // General needs coins to be stolen
    attacker.add_coins(1); // Make sure attacker has at least 1 coin
    attacker.arrest(general);
    CHECK_EQ(attacker.get_coins(), 2);
    general.recover_arrest();
    CHECK_EQ(general.get_coins(), 1);
}

TEST_CASE("Judge role abilities") {
    Game game;
    Judge judge("TestJudge", &game);
    Player briber("Briber", "Regular", &game);
    
    // Setup for block_bribe (briber already paid 4 coins)
    briber.add_coins(5);
    briber.bribe();
    CHECK_EQ(briber.get_coins(), 1);
    
    // Judge blocks bribe
    judge.block_bribe(briber);
    // No coins are returned, briber loses the 4 coins
    CHECK_EQ(briber.get_coins(), 1);
}

TEST_CASE("Merchant role abilities") {
    Game game;
    Merchant merchant("TestMerchant", &game);
    
    // Test bonus when starting with 3+ coins
    merchant.add_coins(3);
    merchant.bonus();
    CHECK_EQ(merchant.get_coins(), 4); // 3 + 1 = 4
    
    // Test pay to pot when arrested
    Player attacker("Attacker", "Regular", &game);
    attacker.add_coins(1); // Make sure attacker has at least 1 coin
    attacker.arrest(merchant);
    CHECK_EQ(merchant.get_coins(), 3); // 4 - 1 = 3 (this is different from our mock implementation)
    CHECK_EQ(attacker.get_coins(), 2); // 1 + 1 = 2
}

TEST_CASE("Game mechanics") {
    Game game;
    Governor* alice = new Governor("Alice", &game);
    Spy* bob = new Spy("Bob", &game);
    Baron* charlie = new Baron("Charlie", &game);
    General* diana = new General("Diana", &game);
    
    game.add_player(alice);
    game.add_player(bob);
    game.add_player(charlie);
    game.add_player(diana);
    
    // Test turn rotation
    CHECK_EQ(game.turn(), "Alice");
    game.next_turn();
    CHECK_EQ(game.turn(), "Bob");
    
    // Test players list
    std::vector<std::string> player_names = game.players_list();
    CHECK_EQ(player_names.size(), 4);
    CHECK_EQ(player_names[0], "Alice");
    CHECK_EQ(player_names[1], "Bob");
    CHECK_EQ(player_names[2], "Charlie");
    CHECK_EQ(player_names[3], "Diana");
    
    // Test player elimination
    alice->add_coins(7); // Enough for coup
    alice->coup(*bob);
    player_names = game.players_list();
    CHECK_EQ(player_names.size(), 3);
    CHECK_EQ(player_names[0], "Alice");
    CHECK_EQ(player_names[1], "Charlie");
    CHECK_EQ(player_names[2], "Diana");
    
    // Test game over condition
    // Eliminate all but one player
    alice->add_coins(7);
    alice->coup(*charlie);
    alice->add_coins(7);
    alice->coup(*diana);
    
    // Game should be over with Alice as winner
    CHECK(game.is_game_over());
    CHECK_EQ(game.winner(), "Alice");
    
    // Should throw if we try to take another turn
    CHECK_THROWS_AS(game.next_turn(), GameOverException);
}

TEST_CASE("Coup action") {
    Game game;
    Player* player1 = new Player("Player1", "Regular", &game);
    Player* player2 = new Player("Player2", "Regular", &game);
    
    game.add_player(player1);
    game.add_player(player2);
    
    // Test insufficient coins for coup
    CHECK_THROWS_AS(player1->coup(*player2), InsufficientCoinsException);
    
    // Test successful coup
    player1->add_coins(7);
    player1->coup(*player2);
    CHECK_EQ(player1->get_coins(), 0);
    CHECK(player2->is_eliminated());
    CHECK(game.is_game_over());
    CHECK_EQ(game.winner(), "Player1");
}

TEST_CASE("Player with 10+ coins") {
    // This would be enforced by the game logic/UI
    // Here we just test that a player can perform a coup with 10 coins
    Game game;
    Player* player1 = new Player("Player1", "Regular", &game);
    Player* player2 = new Player("Player2", "Regular", &game);
    
    game.add_player(player1);
    game.add_player(player2);
    
    player1->add_coins(10);
    player1->coup(*player2);
    CHECK_EQ(player1->get_coins(), 3); // 10 - 7 = 3
    CHECK(player2->is_eliminated());
}