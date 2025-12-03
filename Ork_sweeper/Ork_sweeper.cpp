#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include <queue>
#include <SFML/System/Clock.hpp>
using namespace std;
using namespace sf;

constexpr int ROWS = 8;
constexpr int COLS = 8;
constexpr int TILE_SIZE = 48;
constexpr int SCALE = 1;
constexpr int TOP_BAR_HEIGHT = 80;

class Tile {
public:
	int value;
	bool revealed;
	bool flagged;

	Tile() : value(0), revealed(false), flagged(false) {}
};

array<array<Tile, COLS>, ROWS> field;
bool gameOver = false;
bool gameWon = false;
bool firstClick = true;
constexpr int MINE = 9;
constexpr int MINE_COUNT = 10;

// nejauši izkārto mīnas laukā
void placeMines(int safeRow, int safeCol)
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

		if (field[r][c].value != MINE &&
			!(r == safeRow && c == safeCol))
		{
			field[r][c].value = MINE;
			placed++;
		}
	}

	//parāda terminālī mīnu atrašanās vietu
	for (auto& row : field)
	{
		for (auto cell : row)
			cout << (cell.value == MINE ? "*" : ".") << " ";
		cout << "\n";
	}
}

void calculateNumbers()
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
			if (field[r][c].value == MINE)
				continue;
			int count = 0;

			for (auto& d : dir)
			{
				int nr = r + d[0];
				int nc = c + d[1];
				if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS)
				{
					if (field[nr][nc].value == MINE)
						count++;
				}
			}
			field[r][c].value = count;
		}
	}
}

void setTile(Sprite& sprite, int value) {
	sprite.setTextureRect({ {value * TILE_SIZE, 0},{TILE_SIZE, TILE_SIZE} });
}

//funkcija kas atklāj tukšos laukus (made by ChatGPT)
void revealZeros(int startRow, int startCol)
{
	if (field[startRow][startCol].value != 0)
		return;

	queue<pair<int, int>> q;
	q.push({ startRow, startCol });
	field[startRow][startCol].revealed = true;

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
				Tile& t = field[nr][nc];
				if (!t.revealed) // ja vēl neatklāts
				{
					t.revealed = true;
					t.flagged = false;

					// Ja nākamais arī ir 0, turpinām paplašinājumu
					if (t.value == 0)
						q.push({ nr, nc });
				}
			}
		}
	}
}

//ieliekam karodziņu, ja var
void toggleFlag(int row, int col)
{
	Tile& t = field[row][col];
	if (t.revealed)
		return;

	t.flagged = !t.flagged;
}

bool checkWin()
{
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{

			if (!field[r][c].revealed && field[r][c].value != MINE)
				return false;
		}
	}
	return true;
}

void restartGame()
{
	gameOver = false;
	gameWon = false;
	firstClick = true;

	for (auto& row : field) 
		for(Tile& t : row) 
			t=Tile();
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
	for (auto& row : field)
		for (Tile& t : row)
			if (t.value == MINE)
				t.revealed = true;
}

String countFlags()
{
	int flags = 0;
	for (int r = 0; r < ROWS; r++)
		for (int c = 0; c < COLS; c++)
			if (field[r][c].flagged)
				flags++;

	String mine_display = to_string(MINE_COUNT - flags);

	if (flags < 0) mine_display = "000";
	if (flags > 999) mine_display = "999";

	while (mine_display.getSize() < 3)
		mine_display = "0" + mine_display;


	return mine_display;
}


int main()
{
	Clock clock;
	int seconds = 0;
	RenderWindow window(VideoMode({ COLS * TILE_SIZE , ROWS * TILE_SIZE + TOP_BAR_HEIGHT }), "Ork_sweeper");

	Texture tilesTexture;
	if (!tilesTexture.loadFromFile("Ork_sweeper_sprite_sheet.png"))
		return 1;

	Texture boardTexture;
	if (!boardTexture.loadFromFile("Board.png"))
		cout << "Nevar ieladet Board.png\n";

	Sprite boardSprite(boardTexture);
	boardSprite.setPosition(Vector2f(0, 0));
	boardSprite.setScale(
		Vector2f(
			static_cast<float>(COLS * TILE_SIZE) / boardTexture.getSize().x,
			static_cast<float>(80) / boardTexture.getSize().y
		)
	);

	const Font clockFont("clock_font.otf");
	Text mines(clockFont);
	mines.setCharacterSize(48);
	mines.setFillColor(Color::Red);
	mines.setPosition(Vector2f(20, 16));

	Text time(clockFont);
	time.setCharacterSize(48);
	time.setFillColor(Color::Red);
	time.setString("000");
	time.setPosition(Vector2f(COLS * TILE_SIZE - 90, 16));

	Sprite sprite(tilesTexture);
	sprite.setTextureRect({ { 480, 0},{ TILE_SIZE, TILE_SIZE } });
	sprite.setScale({ SCALE, SCALE });


	while (window.isOpen())
	{
		if (gameOver)
		{
	
			revealAllMines();

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
			mines.setString(countFlags());

			if (gameOver)
				continue;

			if (!gameOver && !firstClick)
			{
				if (!clock.isRunning())
					seconds = 0;
				seconds = static_cast<int>(round(clock.getElapsedTime().asSeconds()));
				time.setString(std::to_string(seconds));
			}

			if (event->is<Event::Closed>() ||
				(event->is<Event::KeyPressed>() &&
					event->getIf<Event::KeyPressed>()->code == Keyboard::Key::Escape))
				window.close();

			if (event->is<Event::MouseButtonPressed>())
			{
				auto mouse = event->getIf<Event::MouseButtonPressed>();
				if (mouse->button == Mouse::Button::Left)
				{
					int col = mouse->position.x / TILE_SIZE ;
					int row = (mouse->position.y - TOP_BAR_HEIGHT) / TILE_SIZE ;

					if (mouse->position.y < TOP_BAR_HEIGHT)
						continue; // klikšķis augšējā panelī


					if (row >= 0 && row < ROWS && col >= 0 && col < COLS)
					{

						Tile& t = field[row][col];

						if (field[row][col].value == MINE)
						{
							t.revealed = true;
							revealAllMines();
							gameOver = true;
							clock.restart();
						}

						if (firstClick)
						{
							placeMines(row, col);
							calculateNumbers();
							firstClick = false;
						}

						if (!t.flagged)
						{
							if (field[row][col].value == MINE)
							{
								t.revealed = true;
								gameOver = true;
							}
							else if (field[row][col].value == 0)
								revealZeros(row, col);
							else
								t.revealed = true;
						}
					}
				}

				if (mouse->button == Mouse::Button::Right)
				{
					int col = mouse->position.x / TILE_SIZE ;
					int row = (mouse->position.y - TOP_BAR_HEIGHT) / TILE_SIZE ;

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

				Tile& t = field[row][col];

				if (t.flagged)
				{
					setTile(sprite, 11);
				}
				else if (t.revealed)
				{
					setTile(sprite, t.value);

					if (t.revealed)
					{
						setTile(sprite, t.value);

					}
				}
				else
				{
					setTile(sprite, 10);
				}

				sprite.setPosition(Vector2f(static_cast<float>(col * TILE_SIZE), static_cast<float>(row * TILE_SIZE + TOP_BAR_HEIGHT)));
				window.draw(sprite);
				window.draw(boardSprite);
				window.draw(mines);
				window.draw(time);
			}
		}
		window.display();
	}

	return 0;
}
