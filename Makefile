CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose

# Qt specific flags
QTFLAGS = -fPIC $(shell pkg-config --cflags Qt5Widgets Qt5Core)
QTLIBS = $(shell pkg-config --libs Qt5Widgets Qt5Core)
QT_MOC = moc

.PHONY: Main test valgrind gui clean

# Main target - run the demo
Main: Demo.cpp Player.cpp PlayerRoles.cpp Game.cpp
	$(CXX) $(CXXFLAGS) -o main Demo.cpp
	./main

# Test targets - compile and run the tests
test: basictest roletest

basictest: Test.cpp Player.cpp PlayerRoles.cpp Game.cpp
	$(CXX) $(CXXFLAGS) -o basictest Test.cpp
	./basictest

roletest: RoleTest.cpp Player.cpp PlayerRoles.cpp Game.cpp
	$(CXX) $(CXXFLAGS) -o roletest RoleTest.cpp
	./roletest

# Valgrind target - run valgrind on the tests
valgrind: Test.cpp RoleTest.cpp Player.cpp PlayerRoles.cpp Game.cpp
	$(CXX) $(CXXFLAGS) -g -o basictest Test.cpp
	$(CXX) $(CXXFLAGS) -g -o roletest RoleTest.cpp
	$(VALGRIND) ./basictest
	$(VALGRIND) ./roletest

# GUI target - Qt-based graphical interface
gui: SimplifiedGUI.cpp Player.cpp PlayerRoles.cpp Game.cpp
	$(CXX) $(CXXFLAGS) $(QTFLAGS) -o gui SimplifiedGUI.cpp $(QTLIBS)
	@echo "GUI built successfully. Run with ./gui"

# Clean up compiled files
clean:
	rm -f main basictest roletest gui