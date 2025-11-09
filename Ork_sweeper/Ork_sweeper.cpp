#include <SFML/Graphics.hpp>

constexpr int ROWS = 8;
constexpr int COLS = 8;

constexpr int TILE_SIZE = 48;      // tavā sprite sheet katra bilde 16x16 px
constexpr int SCALE = 1;           // palielināsim, lai logā izskatās lielāks (32x32 px)

std::array<std::array<int, COLS>, ROWS> field{};

int main()
{
    sf::RenderWindow window(sf::VideoMode({ COLS * TILE_SIZE , ROWS * TILE_SIZE  }), "Minesweeper - SFML3");

    sf::Texture tilesTexture;
    if (!tilesTexture.loadFromFile("minesweeper_sprite_sheet.png"))  // ieliec to blakus .exe
        return 1;

    sf::Sprite sprite(tilesTexture);

    // Hidden tile atrodas sprite sheet pozīcijā (pēdējais kvadrāts)
    // Ja vajag citu, nomaini šeit X koordināti
    sprite.setTextureRect({ { 480, 0},{ TILE_SIZE, TILE_SIZE }});  // piemērs (X=144px)

    sprite.setScale({ SCALE, SCALE });

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
                        field[row][col] = 1;  // atzīmējam kā atklātu
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
                if (field[row][col] == 0)
                {
                    // neatklāts (tava bilde 480px X-offsetā)
                    sprite.setTextureRect({ { 480, 0 }, { TILE_SIZE, TILE_SIZE } });
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
