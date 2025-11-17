#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include <queue>
using namespace std;
using namespace sf;

constexpr int ROWS = 8;
constexpr int COLS = 8;
constexpr int TILE_SIZE = 48;
constexpr int SCALE = 1;
array<array<int, COLS>, ROWS> field{};
array<array<bool, COLS>, ROWS> revealed{};
array<array<bool, COLS>, ROWS> flagged{};
bool gameOver = false;
bool gameWon = false;
bool firstClick = true;
constexpr int MINE = 9;
constexpr int MINE_COUNT = 10;

// nejauši izkārto mīnas laukā
void placeMines(array<array<int, COLS>, ROWS>& field, int safeRow, int safeCol)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distRow(0, ROWS - 1);
	uniform_int_distribution<> distCol(0, COLS - 1);

	int placed = 0;
	while (placed < MINE_COUNT)
	{
		int r = distRow(gen);
		int c = distCol(gen);

		if (field[r][c] != MINE &&
			!(r == safeRow && c == safeCol))
		{
			field[r][c] = MINE;
			placed++;
		}
	}

	//parāda terminālī mīnu atrašanās vietu
	for (auto& row : field)
	{
		for (int cell : row)
			cout << (cell == MINE ? "*" : ".") << " ";
		cout << "\n";
	}
}

void calculateNumbers(array<array<int, COLS>, ROWS>& field)
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

void setTile(Sprite& sprite, int value) {
	int sLocation = value * TILE_SIZE;
	sprite.setTextureRect({ {sLocation, 0},{TILE_SIZE, TILE_SIZE} });
}

//funkcija kas atklāj tukšos laukus (made by ChatGPT)
void revealZeros(int startRow, int startCol)
{
	if (field[startRow][startCol] != 0)
		return;

	queue<pair<int, int>> q;
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

bool checkWin()
{
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{

			if (!revealed[r][c] && field[r][c] != 9)
				return false;
		}
	}
	return true;
}

void restartGame()
{
	gameOver = false;
	gameWon = false;

	for (auto& row : revealed) fill(row.begin(), row.end(), false);
	for (auto& row : flagged) fill(row.begin(), row.end(), false);
	for (auto& row : field) fill(row.begin(), row.end(), 0);

	firstClick = true;
}

int showEndWindow(bool win)
{
	RenderWindow popup(VideoMode({ 300, 180 }), "Game Result", Style::Titlebar | Style::Close);
	const Font font("ArtemisRegular.ttf");

	Text message(font);
	message.setString(win ? "You Win!" : "Game Over");
	message.setCharacterSize(20);
	message.setFillColor(win ? Color::Green : Color::Red);
	message.setPosition(Vector2f(70, 40));

	RectangleShape playAgainButton({ 160, 40 });
	playAgainButton.setFillColor(Color(80, 80, 80));
	playAgainButton.setPosition(Vector2f(70, 100));
	message.setPosition(Vector2f(70, 40));

	Text playAgainText(font);
	playAgainText.setString("Play Again");
	playAgainText.setCharacterSize(22);
	playAgainText.setFillColor(Color::White);
	playAgainText.setPosition(Vector2f(95, 107));

	while (popup.isOpen())
	{
		while (const optional event = popup.pollEvent())
		{
			if (event->is<Event::Closed>())
				return 1;

			if (event->is<Event::MouseButtonPressed>())
			{
				auto mouse = event->getIf<Event::MouseButtonPressed>();
				if (mouse->button == Mouse::Button::Left)
				{
					Vector2f mousePos(mouse->position.x, mouse->position.y);
					if (playAgainButton.getGlobalBounds().contains(mousePos))
						return 0;
				}
			}
		}

		popup.clear(Color(40, 40, 40));
		popup.draw(message);
		popup.draw(playAgainButton);
		popup.draw(playAgainText);
		popup.display();
	}
	return 1;
}

void revealAllMines()
{
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			if (field[r][c] == MINE)
				revealed[r][c] = true;
		}
	}
}

int main()
{
	RenderWindow window(VideoMode({ COLS * TILE_SIZE , ROWS * TILE_SIZE }), "Ork_sweeper");

	Texture tilesTexture;
	if (!tilesTexture.loadFromFile("Ork_sweeper_sprite_sheet.png"))
		return 1;

	Sprite sprite(tilesTexture);

	sprite.setTextureRect({ { 480, 0},{ TILE_SIZE, TILE_SIZE } });

	sprite.setScale({ SCALE, SCALE });


	while (window.isOpen())
	{
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

			int action = showEndWindow(gameWon);

			if (action == 0)
			{
				restartGame();
			}
			else if (action == 1)
			{
				window.close();
			}
		}

		while (const optional event = window.pollEvent())
		{
			if (gameOver)
				continue;

			if (event->is<Event::Closed>() ||
				(event->is<Event::KeyPressed>() &&
					event->getIf<Event::KeyPressed>()->code == Keyboard::Key::Escape))
				window.close();

			if (event->is<Event::MouseButtonPressed>())
			{
				auto mouse = event->getIf<Event::MouseButtonPressed>();
				if (mouse->button == Mouse::Button::Left)
				{
					int col = mouse->position.x / TILE_SIZE;
					int row = mouse->position.y / TILE_SIZE;

					if (row >= 0 && row < ROWS && col >= 0 && col < COLS)
					{
						if (field[row][col] == MINE)
						{
							revealed[row][col] = true;
							revealAllMines();
							gameOver = true;
						}

						if (firstClick)
						{
							placeMines(field, row, col);
							calculateNumbers(field);
							firstClick = false;
						}

						if (!flagged[row][col])
						{
							if (field[row][col] == 9)
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

				if (mouse->button == Mouse::Button::Right)
				{
					int col = mouse->position.x / TILE_SIZE;
					int row = mouse->position.y / TILE_SIZE;

					if (row >= 0 && row < ROWS && col >= 0 && col < COLS)
					{
						toggleFlag(row, col);
					}
				}

				if (checkWin() && !gameOver)
				{
					gameWon = true;
					gameOver = true;
				}
			}
		}

		window.clear(Color::Black);

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

					}
				}
				else
				{
					setTile(sprite, 10);
				}

				sprite.setPosition(Vector2f(static_cast<float>(col * TILE_SIZE), static_cast<float>(row * TILE_SIZE)));
				window.draw(sprite);
			}
		}
		window.display();
	}

	return 0;
}
