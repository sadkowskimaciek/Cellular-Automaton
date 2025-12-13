#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>

// Rozmiar przestrzeni symulacji
const int GRID_WIDTH = 200;
const int GRID_HEIGHT = 200;
const int CELL_SIZE = 4; // Rozmiar komórki w pikselach
const int WINDOW_WIDTH = GRID_WIDTH * CELL_SIZE;
const int WINDOW_HEIGHT = GRID_HEIGHT * CELL_SIZE + 100; // +100 na panel informacyjny

// Enum dla warunków granicznych
enum BoundaryCondition {
    PERIODIC,
    REFLECTIVE
};


// Enum dla reguł
enum Rule {
    CONWAY,      // Standardowa reguła Conway'a
    CUSTOM       // Własna reguła
};

class GameOfLife {
private:
    std::vector<std::vector<bool>> grid;
    std::vector<std::vector<bool>> nextGrid;
    BoundaryCondition boundaryCondition;
    Rule currentRule;
    int generation;

public:
    GameOfLife() : generation(0), boundaryCondition(PERIODIC), currentRule(CONWAY) {
        grid.resize(GRID_HEIGHT, std::vector<bool>(GRID_WIDTH, false));
        nextGrid.resize(GRID_HEIGHT, std::vector<bool>(GRID_WIDTH, false));
    }

    void clear() {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                grid[y][x] = false;
            }
        }
        generation = 0;
    }

    void setCell(int x, int y, bool alive) {
        if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
            grid[y][x] = alive;
        }
    }

    bool getCell(int x, int y) const {
        if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
            return grid[y][x];
        }
        return false;
    }

    void setBoundaryCondition(BoundaryCondition bc) {
        boundaryCondition = bc;
    }

    void setRule(Rule rule) {
        currentRule = rule;
    }

    // Zliczanie sąsiadów z uwzględnieniem warunków granicznych
    int countNeighbors(int x, int y) {
        int count = 0;

        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;

                int nx = x + dx;
                int ny = y + dy;

                if (boundaryCondition == PERIODIC) {
                    // Warunek periodyczny - zapętlenie
                    nx = (nx + GRID_WIDTH) % GRID_WIDTH;
                    ny = (ny + GRID_HEIGHT) % GRID_HEIGHT;
                    if (grid[ny][nx]) count++;
                } else if (boundaryCondition == REFLECTIVE) {
                    // Warunek odbijający - traktuj brzeg jako martwy
                    if (nx >= 0 && nx < GRID_WIDTH && ny >= 0 && ny < GRID_HEIGHT) {
                        if (grid[ny][nx]) count++;
                    }
                }
            }
        }

        return count;
    }

    // Standardowa reguła Conway'a
    bool applyConwayRule(bool alive, int neighbors) {
        if (alive) {
            return neighbors == 2 || neighbors == 3;
        } else {
            return neighbors == 3;
        }
    }

    // Własna reguła: B36/S23 (HighLife)
    // Komórka rodzi się przy 3 lub 6 sąsiadach
    // Komórka przeżywa przy 2 lub 3 sąsiadach
    bool applyCustomRule(bool alive, int neighbors) {
        if (alive) {
            return neighbors == 2 || neighbors == 3;
        } else {
            return neighbors == 3 || neighbors == 6;
        }
    }

    void update() {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                int neighbors = countNeighbors(x, y);
                bool alive = grid[y][x];

                if (currentRule == CONWAY) {
                    nextGrid[y][x] = applyConwayRule(alive, neighbors);
                } else {
                    nextGrid[y][x] = applyCustomRule(alive, neighbors);
                }
            }
        }

        // Zamień siatki
        std::swap(grid, nextGrid);
        generation++;
    }

    // Wzorce początkowe

    // 1. Glider
    void initGlider(int startX, int startY) {
        clear();
        setCell(startX + 1, startY, true);
        setCell(startX + 2, startY + 1, true);
        setCell(startX, startY + 2, true);
        setCell(startX + 1, startY + 2, true);
        setCell(startX + 2, startY + 2, true);
    }

    // 2. Niezmienny (Block)
    void initBlock(int startX, int startY) {
        clear();
        setCell(startX, startY, true);
        setCell(startX + 1, startY, true);
        setCell(startX, startY + 1, true);
        setCell(startX + 1, startY + 1, true);
    }


   // 3. Pentadecathlon - oscylator z okresem 15
    void initPentadecathlon(int startX, int startY) {
        clear();
        // Pionowa konfiguracja
        for (int i = 0; i < 10; i++) {
            if (i == 2 || i == 7) {

                setCell(startX - 1, startY + i, true);
                setCell(startX + 1, startY + i, true);
            } else {

                setCell(startX, startY + i, true);
            }
        }

        clear();
        setCell(startX, startY, true);
        setCell(startX, startY + 1, true);
        setCell(startX - 1, startY + 2, true);
        setCell(startX + 1, startY + 2, true);
        setCell(startX, startY + 3, true);
        setCell(startX, startY + 4, true);
        setCell(startX, startY + 5, true);
        setCell(startX, startY + 6, true);
        setCell(startX - 1, startY + 7, true);
        setCell(startX + 1, startY + 7, true);
        setCell(startX, startY + 8, true);
        setCell(startX, startY + 9, true);
    }

    // 4. R-pentomino - wzór chaotyczny
    void initRPentomino(int startX, int startY) {
        clear();
        setCell(startX + 1, startY, true);
        setCell(startX + 2, startY, true);
        setCell(startX, startY + 1, true);
        setCell(startX + 1, startY + 1, true);
        setCell(startX + 1, startY + 2, true);
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (grid[y][x]) {
                    cell.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                    cell.setFillColor(sf::Color::Green);
                    window.draw(cell);
                }
            }
        }
    }

    int getGeneration() const { return generation; }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                            "Gra w Życie - Automat Komórkowy 2D");
    window.setFramerateLimit(10);

    GameOfLife game;

    // Inicjalizacja
    int currentPattern = 0;
    game.initGlider(50, 50);

    sf::Font font;
    // Próba załadowania systemowej czcionki
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        // Jeśli nie znajdzie, spróbuj Windows
        if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
            std::cout << "Nie można załadować czcionki - tekst nie będzie wyświetlany" << std::endl;
        }
    }

    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(14);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(10, GRID_HEIGHT * CELL_SIZE + 10);

    bool paused = true;
    bool stepMode = false;

    std::cout << "=== STEROWANIE ===" << std::endl;
    std::cout << "SPACJA - Start/Stop symulacji" << std::endl;
    std::cout << "S - Wykonaj jeden krok (gdy zatrzymane)" << std::endl;
    std::cout << "1 - Glider" << std::endl;
    std::cout << "2 - Block (niezmienny)" << std::endl;
    std::cout << "3 - Pentadecathlon (oscylator 15)" << std::endl;
    std::cout << "4 - R-pentomino" << std::endl;
    std::cout << "P - Warunek periodyczny" << std::endl;
    std::cout << "R - Warunek odbijajacy" << std::endl;
    std::cout << "C - Reguła Conway" << std::endl;
    std::cout << "H - Reguła HighLife (wlasna)" << std::endl;
    std::cout << "ESC - Wyjście" << std::endl;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                else if (event.key.code == sf::Keyboard::Space) {
                    paused = !paused;
                }
                else if (event.key.code == sf::Keyboard::S && paused) {
                    stepMode = true;
                }
                else if (event.key.code == sf::Keyboard::Num1) {
                    game.initGlider(50, 50);
                    currentPattern = 0;
                    paused = true;
                }
                else if (event.key.code == sf::Keyboard::Num2) {
                    game.initBlock(100, 100);
                    currentPattern = 1;
                    paused = true;
                }
                else if (event.key.code == sf::Keyboard::Num3) {
                    game.initPentadecathlon(100, 95);
                    currentPattern = 2;
                    paused = true;
                }
                else if (event.key.code == sf::Keyboard::Num4) {
                    game.initRPentomino(100, 100);
                    currentPattern = 3;
                    paused = true;
                }
                else if (event.key.code == sf::Keyboard::P) {
                    game.setBoundaryCondition(PERIODIC);
                }
                else if (event.key.code == sf::Keyboard::R) {
                    game.setBoundaryCondition(REFLECTIVE);
                }
                else if (event.key.code == sf::Keyboard::C) {
                    game.setRule(CONWAY);
                }
                else if (event.key.code == sf::Keyboard::H) {
                    game.setRule(CUSTOM);
                }
            }
        }

        if (!paused || stepMode) {
            game.update();
            stepMode = false;
        }

        window.clear(sf::Color::Black);
        game.draw(window);

        // Wyświetl informacje
        std::string patternName[] = {"Glider", "Block", "Pentadecathlon", "R-pentomino"};
        std::string info = "Generacja: " + std::to_string(game.getGeneration()) +
                          " | Wzor: " + patternName[currentPattern] +
                          " | Status: " + (paused ? "PAUZA" : "DZIALA") +
                          " | [SPACJA-start/stop] [S-krok] [1-4-wzory] [P/R-brzegi] [C/H-reguly]";
        infoText.setString(info);
        window.draw(infoText);

        window.display();
    }

    return 0;
}