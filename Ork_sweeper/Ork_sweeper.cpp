#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>

constexpr int ROWS = 8;
constexpr int COLS = 8;

constexpr int TILE_SIZE = 48;      // tavā sprite sheet katra bilde 16x16 px
constexpr int SCALE = 1;           // palielināsim, lai logā izskatās lielāks (32x32 px)

std::array<std::array<int, COLS>, ROWS> field{};

constexpr int MINE = 9;
constexpr int MINE_COUNT = 10;

// nejauši izkārto mīnas laukā
void placeMines(std::array<std::array<int, COLS>, ROWS>& field)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distRow(0, ROWS - 1);
    std::uniform_int_distribution<> distCol(0, COLS - 1);

    int placed = 0;
    while (placed < MINE_COUNT)
    {
        int r = distRow(gen);
        int c = distCol(gen);

        if (field[r][c] != MINE)  // neliekam vairākas mīnas vienā vietā
        {
            field[r][c] = MINE;
            placed++;
        }
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({ COLS * TILE_SIZE , ROWS * TILE_SIZE  }), "Ork_sweeper");

    sf::Texture tilesTexture;
    if (!tilesTexture.loadFromFile("Ork_sweeper_sprite_sheet.png"))  // ieliec to blakus .exe
        return 1;

    sf::Sprite sprite(tilesTexture);

    // Hidden tile atrodas sprite sheet pozīcijā (pēdējais kvadrāts)
    // Ja vajag citu, nomaini šeit X koordināti
    sprite.setTextureRect({ { 480, 0},{ TILE_SIZE, TILE_SIZE }});  // piemērs (X=144px)

    sprite.setScale({ SCALE, SCALE });

    placeMines(field);

    //parāda terminālī mīnu atrašanās vietu
    for (auto& row : field)
    {
        for (int cell : row)
            std::cout << (cell == MINE ? "*" : ".") << " ";
        std::cout << "\n";
    }

    while (window.isOpen())
    {
        
        while (const std::optional event = window.pollEvent())
        {
            // Window closed or escape key pressed: exit
            if (event->is<sf::Event::Closed>() ||
                (event->is<sf::Event::KeyPressed>() &&
                    event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape))
                window.close();

            if (event->is<sf::Event::MouseButtonPressed>())
            {
                auto mouse = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouse->button == sf::Mouse::Button::Left)
                {
                    int col = mouse->position.x / TILE_SIZE;
                    int row = mouse->position.y / TILE_SIZE;

                    if (row >= 0 && row < ROWS && col >= 0 && col < COLS)
                    {
                     if (field[row][col] == MINE )
                    {
                         field[row][col] = 2;
                    }
                     else field[row][col] = 1;  // atzīmējam kā atklātu
                    }
                }
            }
        }

        window.clear(sf::Color::Black);

        // zīmē laukumu
        for (int row = 0; row < ROWS; row++)
        {
            for (int col = 0; col < COLS; col++)
            {
                if (field[row][col] == 0 || field[row][col] == MINE)
                {
                    // neatklāts (tava bilde 480px X-offsetā)
                    sprite.setTextureRect({ { 480, 0 }, { TILE_SIZE, TILE_SIZE } });
                }else if (field[row][col] == 2)
                {
                    sprite.setTextureRect({ { 432, 0 }, { TILE_SIZE, TILE_SIZE } }); // mīna (pieskaņo sprite pozīciju)
                }
                else
                {
                    // atklāts tile (X=0 pozīcija sprite sheetā)
                    sprite.setTextureRect({ { 0, 0 }, { TILE_SIZE, TILE_SIZE } });
                }

                sprite.setPosition(sf::Vector2f(static_cast<float>(col * TILE_SIZE), static_cast<float>(row * TILE_SIZE)));
                window.draw(sprite);
            }
        }
    

        window.display();
    }

    return 0;
}
