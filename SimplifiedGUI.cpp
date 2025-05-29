#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QRadioButton>
#include <QTextEdit>
#include <QMessageBox>
#include <QScrollBar>
#include <QList>
#include <vector>
#include <string>
#include "Game.cpp"

// Simple game logger class
class GameLogger {
private:
    std::vector<std::string> history;

public:
    void log(const std::string& action) {
        history.push_back(action);
    }

    std::vector<std::string> get_history() const {
        return history;
    }

    void clear() {
        history.clear();
    }
};

// Forward declaration
class GameWindow;

// Widget to display player info
class PlayerWidget : public QGroupBox {
private:
    Player* player;
    QLabel* roleLabel;
    QLabel* coinsLabel;
    QLabel* statusLabel;
    bool isHighlighted;

public:
    PlayerWidget(Player* player, QWidget* parent = nullptr) 
        : QGroupBox(QString::fromStdString(player->get_name()), parent), player(player), isHighlighted(false) {
        
        QVBoxLayout* layout = new QVBoxLayout(this);
        
        roleLabel = new QLabel(QString("Role: %1").arg(QString::fromStdString(player->get_role())));
        coinsLabel = new QLabel(QString("Coins: %1").arg(player->get_coins()));
        
        std::string statusText = player->is_sanctioned() ? "SANCTIONED" : "";
        statusLabel = new QLabel(QString::fromStdString(statusText));
        statusLabel->setStyleSheet("color: red;");
        
        layout->addWidget(roleLabel);
        layout->addWidget(coinsLabel);
        layout->addWidget(statusLabel);
        
        setLayout(layout);
        setMinimumWidth(150);
        setMinimumHeight(100);
    }

    void update() {
        coinsLabel->setText(QString("Coins: %1").arg(player->get_coins()));
        std::string statusText = player->is_sanctioned() ? "SANCTIONED" : "";
        statusLabel->setText(QString::fromStdString(statusText));
        
        if (player->is_eliminated()) {
            setStyleSheet("background-color: #ffcccc; border: 1px solid gray;");
            setTitle(QString::fromStdString(player->get_name() + " (ELIMINATED)"));
        } else if (isHighlighted) {
            setStyleSheet("background-color: #ccffcc; border: 2px solid green;");
            setTitle(QString::fromStdString(player->get_name() + " (CURRENT)"));
        } else {
            setStyleSheet("background-color: white; border: 1px solid gray;");
            setTitle(QString::fromStdString(player->get_name()));
        }
    }

    void highlight(bool highlight) {
        isHighlighted = highlight;
        update();
    }

    Player* getPlayer() const {
        return player;
    }
};

// Widget for game actions
class ActionPanel : public QGroupBox {
private:
    Game* game;
    GameLogger* logger;
    QComboBox* playerSelector;
    QRadioButton* gatherAction;
    QRadioButton* taxAction;
    QRadioButton* bribeAction;
    QRadioButton* arrestAction;
    QRadioButton* sanctionAction;
    QRadioButton* coupAction;
    QRadioButton* specialAction;
    QPushButton* executeButton;
    GameWindow* gameWindow;

public:
    ActionPanel(Game* game, GameLogger* logger, GameWindow* gameWindow, QWidget* parent = nullptr);
    void executeAction();
    void updateSpecialActionLabel();
};

// Main game window
class GameWindow : public QMainWindow {
private:
    Game game;
    GameLogger logger;
    std::vector<PlayerWidget*> playerWidgets;
    ActionPanel* actionPanel;
    QTextEdit* historyDisplay;
    QPushButton* nextTurnButton;
    QLabel* currentPlayerLabel;
    QLabel* gameStateLabel;

public:
    GameWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Coup Game");
        setMinimumSize(800, 600);
        
        // Create central widget and main layout
        QWidget* centralWidget = new QWidget(this);
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        
        // Game state section
        QHBoxLayout* gameStateLayout = new QHBoxLayout();
        gameStateLabel = new QLabel("Game Active");
        currentPlayerLabel = new QLabel("Current Player: ");
        gameStateLayout->addWidget(gameStateLabel);
        gameStateLayout->addWidget(currentPlayerLabel);
        gameStateLayout->addStretch();
        
        // Players section
        QGroupBox* playersGroup = new QGroupBox("Players");
        QHBoxLayout* playersLayout = new QHBoxLayout(playersGroup);
        
        // Initialize game and create players first
        initializeGame(playersLayout);
        
        // Action panel
        actionPanel = new ActionPanel(&game, &logger, this, centralWidget);
        
        // History display
        QGroupBox* historyGroup = new QGroupBox("Game History");
        QVBoxLayout* historyLayout = new QVBoxLayout(historyGroup);
        historyDisplay = new QTextEdit();
        historyDisplay->setReadOnly(true);
        historyLayout->addWidget(historyDisplay);
        
        // Next turn button
        nextTurnButton = new QPushButton("Next Turn");
        connect(nextTurnButton, &QPushButton::clicked, this, &GameWindow::nextTurn);
        
        // Add sections to main layout
        mainLayout->addLayout(gameStateLayout);
        mainLayout->addWidget(playersGroup);
        mainLayout->addWidget(actionPanel);
        mainLayout->addWidget(historyGroup);
        mainLayout->addWidget(nextTurnButton);
        
        // Set central widget
        setCentralWidget(centralWidget);
        
        // Update UI for start
        updateHistory();
        updateGameState();
    }
    
    void initializeGame(QHBoxLayout* playersLayout) {
        // Create players
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
        
        // Create player widgets
        playerWidgets.push_back(new PlayerWidget(alice));
        playerWidgets.push_back(new PlayerWidget(bob));
        playerWidgets.push_back(new PlayerWidget(charlie));
        playerWidgets.push_back(new PlayerWidget(diana));
        playerWidgets.push_back(new PlayerWidget(ethan));
        playerWidgets.push_back(new PlayerWidget(fiona));
        
        // Add widgets directly to layout
        for (auto widget : playerWidgets) {
            playersLayout->addWidget(widget);
        }
        
        // Log game start
        logger.log("Game started with 6 players");
    }

    void updateGameState() {
        // Update player widgets
        for (auto widget : playerWidgets) {
            bool isCurrent = (widget->getPlayer()->get_name() == game.turn());
            widget->highlight(isCurrent);
            widget->update();
        }
        
        // Update current player label
        currentPlayerLabel->setText(QString("Current Player: %1").arg(QString::fromStdString(game.turn())));
        
        // Update game state label
        if (game.is_game_over()) {
            gameStateLabel->setText(QString("Game Over - Winner: %1").arg(QString::fromStdString(game.winner())));
            gameStateLabel->setStyleSheet("font-weight: bold; color: green;");
            nextTurnButton->setEnabled(false);
            actionPanel->setEnabled(false);
        } else {
            gameStateLabel->setText("Game Active");
            gameStateLabel->setStyleSheet("");
        }
    }

    void updateHistory() {
        historyDisplay->clear();
        for (const auto& entry : logger.get_history()) {
            historyDisplay->append(QString::fromStdString(entry));
        }
        // Scroll to bottom
        historyDisplay->verticalScrollBar()->setValue(historyDisplay->verticalScrollBar()->maximum());
    }

    Game* getGame() {
        return &game;
    }

    std::vector<PlayerWidget*> getPlayerWidgets() {
        return playerWidgets;
    }
    
    void nextTurn() {
        try {
            Player* current = game.get_current_player();
            game.next_turn();
            logger.log(current->get_name() + "'s turn ended. Now " + game.turn() + "'s turn.");
            updateHistory();
            updateGameState();

            // Update the target player selection with the new list of players
            if (actionPanel) {
                // Get the combobox from the action panel
                QComboBox* playerSelector = actionPanel->findChild<QComboBox*>("playerSelector");
                if (playerSelector) {
                    playerSelector->clear();

                    // Add all players except the current player
                    std::vector<std::string> playerNames = game.players_list();
                    for (const auto& name : playerNames) {
                        if (name != game.turn()) { // Don't add current player as target
                            playerSelector->addItem(QString::fromStdString(name));
                        }
                    }
                }

                // Update special action label for the new player
                QRadioButton* specialAction = actionPanel->findChild<QRadioButton*>("Special Ability");
                if (specialAction && specialAction->isChecked()) {
                    actionPanel->updateSpecialActionLabel();
                }

                // Auto-select gather action for the new player
                QRadioButton* gatherAction = actionPanel->findChild<QRadioButton*>("Gather (1 coin)");
                if (gatherAction) {
                    gatherAction->setChecked(true);
                }

                // Check if current player has 10+ coins
                Player* currentPlayer = game.get_current_player();
                if (currentPlayer && currentPlayer->get_coins() >= 10) {
                    QMessageBox::warning(this, "Must Coup",
                        QString("%1 has 10+ coins and must perform a coup!")
                        .arg(QString::fromStdString(currentPlayer->get_name())));
                }
            }

        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Error", e.what());
        }
    }

    void logAction(const std::string& action) {
        logger.log(action);
        updateHistory();
        updateGameState();
    }
};

// ActionPanel implementation - needs to be after GameWindow because it references it
ActionPanel::ActionPanel(Game* game, GameLogger* logger, GameWindow* gameWindow, QWidget* parent)
    : QGroupBox("Actions", parent), game(game), logger(logger), gameWindow(gameWindow) {

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Target player selection
    QHBoxLayout* targetLayout = new QHBoxLayout();
    QLabel* targetLabel = new QLabel("Target Player:");
    playerSelector = new QComboBox();
    playerSelector->setObjectName("playerSelector"); // Set object name for findChild

    // Populate with players
    std::vector<std::string> playerNames = game->players_list();
    for (const auto& name : playerNames) {
        if (name != game->turn()) { // Don't add current player as target
            playerSelector->addItem(QString::fromStdString(name));
        }
    }

    targetLayout->addWidget(targetLabel);
    targetLayout->addWidget(playerSelector);
    mainLayout->addLayout(targetLayout);

    // Action selection
    QGroupBox* actionGroup = new QGroupBox("Select Action");
    QVBoxLayout* actionLayout = new QVBoxLayout(actionGroup);

    gatherAction = new QRadioButton("Gather (1 coin)");
    gatherAction->setObjectName("Gather (1 coin)"); // Set object name for findChild

    taxAction = new QRadioButton("Tax (2 coins)");
    bribeAction = new QRadioButton("Bribe (pay 4 coins)");
    arrestAction = new QRadioButton("Arrest (steal 1 coin)");
    sanctionAction = new QRadioButton("Sanction (pay 3 coins)");
    coupAction = new QRadioButton("Coup (pay 7 coins)");
    specialAction = new QRadioButton("Special Ability");
    specialAction->setObjectName("Special Ability"); // Set object name for findChild

    gatherAction->setChecked(true);

    actionLayout->addWidget(gatherAction);
    actionLayout->addWidget(taxAction);
    actionLayout->addWidget(bribeAction);
    actionLayout->addWidget(arrestAction);
    actionLayout->addWidget(sanctionAction);
    actionLayout->addWidget(coupAction);
    actionLayout->addWidget(specialAction);

    mainLayout->addWidget(actionGroup);

    // Execute button
    executeButton = new QPushButton("Execute Action");
    mainLayout->addWidget(executeButton);

    // Connect signals
    connect(executeButton, &QPushButton::clicked, this, &ActionPanel::executeAction);
    connect(specialAction, &QRadioButton::toggled, this, &ActionPanel::updateSpecialActionLabel);

    // Set initial special action label
    updateSpecialActionLabel();
}

void ActionPanel::updateSpecialActionLabel() {
    if (!specialAction->isChecked()) {
        return;
    }
    
    // Get current player's role
    Player* currentPlayer = game->get_current_player();
    std::string role = currentPlayer->get_role();
    
    if (role == "Governor") {
        specialAction->setText("Special: Block Tax");
    } else if (role == "Spy") {
        specialAction->setText("Special: View Coins");
    } else if (role == "Baron") {
        specialAction->setText("Special: Invest 3 coins for 6");
    } else if (role == "General") {
        specialAction->setText("Special: Protect from Coup (5 coins)");
    } else if (role == "Judge") {
        specialAction->setText("Special: Block Bribe");
    } else if (role == "Merchant") {
        specialAction->setText("Special: Get Bonus Coin");
    } else {
        specialAction->setText("Special: None Available");
        specialAction->setEnabled(false);
    }
}

void ActionPanel::executeAction() {
    try {
        Player* currentPlayer = game->get_current_player();
        std::string currentPlayerName = currentPlayer->get_name();

        // Check if this player has 10+ coins and trying to do something other than coup
        if (currentPlayer->get_coins() >= 10 && !coupAction->isChecked()) {
            QMessageBox::warning(gameWindow, "Must Coup", "You have 10 or more coins and must perform a coup!");
            return;
        }

        // Get target player if needed
        Player* targetPlayer = nullptr;
        std::string targetName;

        if (playerSelector->count() > 0) {
            targetName = playerSelector->currentText().toStdString();
            targetPlayer = game->get_player_by_name(targetName);
        }

        std::string action;
        bool performed = false;

        // Execute selected action
        if (gatherAction->isChecked()) {
            currentPlayer->gather();
            action = currentPlayerName + " gathered 1 coin";
            performed = true;
        }
        else if (taxAction->isChecked()) {
            currentPlayer->tax();
            if (currentPlayer->get_role() == "Governor") {
                action = currentPlayerName + " (Governor) taxed 3 coins";
            } else {
                action = currentPlayerName + " taxed 2 coins";
            }
            performed = true;
        }
        else if (bribeAction->isChecked()) {
            currentPlayer->bribe();
            action = currentPlayerName + " paid 4 coins to bribe";
            performed = true;
        }
        else if (arrestAction->isChecked()) {
            if (!targetPlayer) {
                throw std::runtime_error("Must select a target player for arrest");
            }
            currentPlayer->arrest(*targetPlayer);
            action = currentPlayerName + " arrested " + targetName + " and stole 1 coin";
            performed = true;
        }
        else if (sanctionAction->isChecked()) {
            if (!targetPlayer) {
                throw std::runtime_error("Must select a target player for sanction");
            }
            currentPlayer->sanction(*targetPlayer);
            action = currentPlayerName + " sanctioned " + targetName;
            performed = true;
        }
        else if (coupAction->isChecked()) {
            if (!targetPlayer) {
                throw std::runtime_error("Must select a target player for coup");
            }
            currentPlayer->coup(*targetPlayer);
            action = currentPlayerName + " performed a coup on " + targetName + " and eliminated them";
            performed = true;
        }
        else if (specialAction->isChecked()) {
            std::string role = currentPlayer->get_role();

            if (role == "Governor") {
                if (!targetPlayer) {
                    throw std::runtime_error("Must select a target player for block tax");
                }
                dynamic_cast<Governor*>(currentPlayer)->block_tax(*targetPlayer);
                action = currentPlayerName + " (Governor) blocked " + targetName + "'s tax action";
                performed = true;
            }
            else if (role == "Spy") {
                if (!targetPlayer) {
                    throw std::runtime_error("Must select a target player to view coins");
                }
                int coins = dynamic_cast<Spy*>(currentPlayer)->view_coins(*targetPlayer);
                action = currentPlayerName + " (Spy) viewed that " + targetName + " has " + std::to_string(coins) + " coins";
                performed = true;

                // Show a message box with the coin count
                QMessageBox::information(gameWindow, "Spy Action",
                                        QString("%1 has %2 coins").arg(QString::fromStdString(targetName)).arg(coins));
            }
            else if (role == "Baron") {
                dynamic_cast<Baron*>(currentPlayer)->invest();
                action = currentPlayerName + " (Baron) invested 3 coins to get 6 coins";
                performed = true;
            }
            else if (role == "General") {
                if (!targetPlayer) {
                    throw std::runtime_error("Must select a target player to protect");
                }
                dynamic_cast<General*>(currentPlayer)->protect(*targetPlayer);
                action = currentPlayerName + " (General) protected " + targetName + " from a coup";
                performed = true;
            }
            else if (role == "Judge") {
                if (!targetPlayer) {
                    throw std::runtime_error("Must select a target player to block bribe");
                }
                dynamic_cast<Judge*>(currentPlayer)->block_bribe(*targetPlayer);
                action = currentPlayerName + " (Judge) blocked " + targetName + "'s bribe";
                performed = true;
            }
            else if (role == "Merchant") {
                dynamic_cast<Merchant*>(currentPlayer)->bonus();
                action = currentPlayerName + " (Merchant) received a bonus coin";
                performed = true;
            }
        }

        if (performed) {
            // Log action
            gameWindow->logAction(action);

            // After action, automatically advance to next player's turn
            gameWindow->nextTurn();

            // If the game is over after this action, disable the execute button
            if (game->is_game_over()) {
                executeButton->setEnabled(false);
            }
        }

    } catch (const std::exception& e) {
        QMessageBox::warning(gameWindow, "Action Error", e.what());
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    GameWindow window;
    window.show();
    
    return app.exec();
}