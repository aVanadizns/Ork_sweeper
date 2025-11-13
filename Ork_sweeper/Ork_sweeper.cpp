#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include <queue>

constexpr int ROWS = 8;
constexpr int COLS = 8;

constexpr int TILE_SIZE = 48;
constexpr int SCALE = 1;

std::array<std::array<int, COLS>, ROWS> field{};
std::array<std::array<bool, COLS>, ROWS> revealed{};
std::array<std::array<bool, COLS>, ROWS> flagged{};
bool gameOver = false;



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

		if (field[r][c] != MINE)
		{
			field[r][c] = MINE;
			placed++;
		}
	}
}

void calculateNumbers(std::array<std::array<int, COLS>, ROWS>& field)
{

	const int dir[8][2] =
	{
		{-1, -1}, {-1, 0}, {-1, 1},
		{ 0, -1},          { 0, 1},
		{ 1, -1}, { 1, 0}, { 1, 1}
	};

	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			if (field[r][c] == MINE)
				continue;

			int count = 0;


			for (auto& d : dir)
			{
				int nr = r + d[0];
				int nc = c + d[1];

				if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS)
				{
					if (field[nr][nc] == MINE)
						count++;
				}
			}

			field[r][c] = count;
		}
	}
}

void setTile(sf::Sprite& sprite, int value) {
	int sLocation = value * TILE_SIZE;
	sprite.setTextureRect({ {sLocation, 0},{TILE_SIZE, TILE_SIZE} });
}

//funkcija kas atklāj tukšos laukus (made by ChatGPT)
void revealZeros(int startRow, int startCol)
{
	if (field[startRow][startCol] != 0)
		return;

	std::queue<std::pair<int, int>> q;
	q.push({ startRow, startCol });
	revealed[startRow][startCol] = true;

	const int dir[8][2] =
	{
		{-1,-1}, {-1, 0}, {-1, 1},
		{ 0,-1},          { 0, 1},
		{ 1,-1}, { 1, 0}, { 1, 1}
	};

	while (!q.empty())
	{
		auto [r, c] = q.front();
		q.pop();

		for (auto& d : dir)
		{
			int nr = r + d[0];
			int nc = c + d[1];

			if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS)
			{
				if (!revealed[nr][nc]) // ja vēl neatklāts
				{
					revealed[nr][nc] = true;
					flagged[nr][nc] = false;

					// Ja nākamais arī ir 0, turpinām paplašinājumu
					if (field[nr][nc] == 0)
						q.push({ nr, nc });
				}
			}
		}
	}
}

//ieliekam karodziņu, ja var
void toggleFlag(int row, int col)
{

	if (revealed[row][col])
		return;

	flagged[row][col] = !flagged[row][col];
}


int main()
{
	sf::RenderWindow window(sf::VideoMode({ COLS * TILE_SIZE , ROWS * TILE_SIZE }), "Ork_sweeper");

	sf::Texture tilesTexture;
	if (!tilesTexture.loadFromFile("Ork_sweeper_sprite_sheet.png"))
		return 1;

	sf::Sprite sprite(tilesTexture);

	sprite.setTextureRect({ { 480, 0},{ TILE_SIZE, TILE_SIZE } });

	sprite.setScale({ SCALE, SCALE });

	placeMines(field);
	calculateNumbers(field);

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
			if (gameOver)
				continue;

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
						if (!flagged[row][col])
						{
							if (field[row][col] == 9) // 💣 MĪNA
							{
								revealed[row][col] = true;
								gameOver = true;
							}
							else if (field[row][col] == 0)
								revealZeros(row, col);
							else
								revealed[row][col] = true;
						}
					}
				}
				if (mouse->button == sf::Mouse::Button::Right)
				{
					int col = mouse->position.x / TILE_SIZE;
					int row = mouse->position.y / TILE_SIZE;

					if (row >= 0 && row < ROWS && col >= 0 && col < COLS)
					{
						toggleFlag(row, col);
					}
				}


			}
		}

		if (gameOver)
		{
			for (int r = 0; r < ROWS; r++)
			{
				for (int c = 0; c < COLS; c++)
				{
					if (field[r][c] == MINE)
					{
						revealed[r][c] = true; 
					}
				}
			}
			
		}
		
		window.clear(sf::Color::Black);


		for (int row = 0; row < ROWS; row++)
		{
			for (int col = 0; col < COLS; col++)
			{
				if (flagged[row][col])
				{
					setTile(sprite, 11);
				}
				else if (revealed[row][col])
				{
					setTile(sprite, field[row][col]);

					if (revealed[row][col])
					{
						setTile(sprite, field[row][col]);

						if (gameOver && field[row][col] == MINE)
							sprite.setColor(sf::Color::Red); 
						else
							sprite.setColor(sf::Color::White);
					}

				}
				else
				{
					setTile(sprite, 10);
				}

				sprite.setPosition(sf::Vector2f(static_cast<float>(col * TILE_SIZE), static_cast<float>(row * TILE_SIZE)));
				window.draw(sprite);
			}
		}
		
		window.display();
		
		
	}

	return 0;
}
