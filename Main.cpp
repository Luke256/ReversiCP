# include "Main.hpp"
# include "Game.hpp"

void Main()
{
	Window::Resize(AppData::Width, AppData::Height);
	Scene::SetBackground(Palette::Lightblue);

	MyApp app;

	app.add<Game>(U"Game");

	FontAsset::Register(U"font", FontMethod::MSDF, 48);
	FontAsset::Register(U"bold", FontMethod::MSDF, 48, Typeface::Bold);

	while (System::Update())
	{
		if (!app.update()) break;
	}
}
