# Coup Game Implementation

## Overview

This project is an implementation of the card game "Coup," a strategic game of influence, manipulation, and deception. Players take on different roles and compete to be the last one standing.

In this implementation, we've created:
- A core game engine with all game mechanics
- Role-specific player classes with special abilities
- A graphical user interface to play the game
- Comprehensive unit tests

## Game Rules

At the start, each player assumes a role from the deck. In the center of the table is a coin pot. Each turn, players can take actions according to their role and may collect coins. The goal is to execute *coups* and eliminate other players. The last player with a role wins.

### Basic Actions

Each player has a name, a role, and a number of coins. In their turn, regardless of role, a player may perform one of the following actions:

* **Gather** – Take 1 coin from the pot. This action is free but can be blocked by *Sanction*.
* **Tax** – Take 2 coins from the pot. This action is free but may be blocked by certain roles/actions.
* **Bribe** – Pay 4 coins to perform an extra action during the same turn.
* **Arrest** – Steal 1 coin from another player. Cannot target the same player two turns in a row.
* **Sanction** – Prevent another player from using economic actions (*gather*, *tax*) until their next turn. Costs 3 coins.
* **Coup** – Eliminate another player from the game. Costs 7 coins and can only be blocked in certain conditions.

### Role Abilities

The game includes various roles, each with unique abilities:

* **Governor**
  * Takes 3 coins instead of 2 when performing *tax*.
  * Can block *tax* actions by others.

* **Spy**
  * Can view another player's coin count.
  * Can block *arrest* actions.

* **Baron**
  * Can "invest" 3 coins to receive 6 coins.
  * Gets 1 coin as compensation if *sanctioned*.

* **General**
  * Can pay 5 coins to protect players from *coup* actions.
  * Regains coins lost from being *arrested*.

* **Judge**
  * Can block *bribe* actions, causing the player to lose the 4 coins.
  * Forces players who *sanction* them to pay an extra coin.

* **Merchant**
  * Gets 1 extra coin when starting with 3+ coins.
  * Pays 2 coins to pot when *arrested* instead of 1 to another player.

### Special Rules

* A player who starts a turn with **10 coins** **must** perform a *coup* that turn.
* The game progresses in turns, with each player taking one action per turn.
* Players are eliminated when they are the target of a successful *coup*.
* The last player remaining wins the game.

## Implementation Details

### Class Structure

- **Player** (Base class): Contains basic player functionality and actions
- **Role-specific classes** (Derived classes): Implement special abilities for each role
- **Game**: Manages game state, player turns, and win conditions
- **Exception classes**: Handle illegal game actions

### Design Principles Applied

- **Inheritance**: Role-specific classes inherit from the Player base class
- **Rule of Three**: Proper memory management with copy constructor, copy assignment operator, and destructor
- **Exception Handling**: Specific exceptions for different illegal actions

## How to Use

### Building and Running

The project includes a Makefile with several targets:

```bash
# Compile and run the main demo
make Main

# Run the unit tests
make test

# Check for memory leaks
make valgrind

# Run the graphical user interface
make gui

# Clean up generated files
make clean
```

### Playing the Game

The graphical interface provides a complete game experience:
- View player status (coins, role, etc.)
- Select actions to perform
- Target other players for certain actions
- Track game history and state
- Play until a winner is determined

## GUI Implementation

The game includes a fully-functional graphical user interface built with Qt. The GUI features:

- Player information displays for all players
- Action selection interface
- Target player selection
- Game history logging
- Turn tracking
- Special role abilities
- Visual indicators for player status

To run the GUI:
```bash
make gui
./gui
```

## Testing

The project includes comprehensive unit tests using the doctest framework. Tests verify:
- Basic game mechanics
- Role-specific abilities
- Exception handling
- Edge cases

Run the tests with:
```bash
make test
```

## Memory Management

The implementation follows the Rule of Three for proper memory management:
- Copy constructor
- Copy assignment operator
- Destructor

Memory leaks are checked using valgrind:
```bash
make valgrind
```