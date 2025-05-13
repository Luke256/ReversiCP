# include "Main.hpp"
# include "Game.hpp"
# include "Integrator.hpp"

void Main()
{
	Window::Resize(AppData::Width, AppData::Height);
	Scene::SetBackground(Palette::Lightblue);

	MyApp app;

	app.add<Game>(U"Game");

	FontAsset::Register(U"font", FontMethod::MSDF, 48);
	FontAsset::Register(U"bold", FontMethod::MSDF, 48, Typeface::Bold);

	String targetFile = FileSystem::CurrentDirectory();
	targetFile = FileSystem::ParentPath(targetFile);
	targetFile = FileSystem::PathAppend(targetFile, U"codingame.cpp");

	Console << targetFile;

	CodeExpander::Expand(targetFile);

	while (System::Update())
	{
		if (!app.update()) break;
	}
}
