# include <Siv3D.hpp>
# include "ReversiEngine.hpp"

void Main()
{
	Scene::SetBackground(Palette::Lightblue);

	Array<int32> board(64);
	Array<int32> availables(64);
	Reversi::ReversiEngine engine;
	engine.reset();

	while (System::Update())
	{

		engine.getBoard(board);
		Reversi::bit2boad(engine.getLegals(), availables);

		for (int32 i : step(64))
		{
			Vec2 pos{ 50 + (i % 8) * 32, 50 + (i / 8) * 32 };
			RectF{ pos, 32 }.drawFrame();
			if (availables[i] == 1)
			{
				RectF{ pos, 32 }.draw(Palette::Red.withA(120));
			}
			if (board[i] == 1)
			{
				Circle{ pos + Vec2{16, 16}, 12 }.draw(Palette::Black);
			}
			if (board[i] == -1)
			{
				Circle{ pos + Vec2{16, 16}, 12 }.draw(Palette::White);
			}
		}

		if (MouseL.down())
		{
			engine.place(
				(Cursor::Pos().x - 50) / 32,
				(Cursor::Pos().y - 50) / 32
			);
		}

		if (KeySpace.down())
		{
			engine.pass();
		}
	}
}
