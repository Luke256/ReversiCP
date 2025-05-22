# include "Main.hpp"
# include "Game.hpp"
# include "CodeExpander.hpp"
# include "lib/CMat/CMat.hpp"

void Main()
{
	CMat::CMat<int>x = {
		{1,2,3},
		{4,5,6},
		{7,8,9},
	};

	Console << x;

	Window::Resize(AppData::Width, AppData::Height);
	Scene::SetBackground(Palette::Lightblue);

	MyApp app;

	app.add<Game>(U"Game");

	FontAsset::Register(U"font", FontMethod::MSDF, 48);
	FontAsset::Register(U"bold", FontMethod::MSDF, 48, Typeface::Bold);

	String targetFile = FileSystem::CurrentDirectory();
	targetFile = FileSystem::ParentPath(targetFile);
	targetFile = FileSystem::PathAppend(targetFile, U"codingame.cpp");

	CodeExpander::Expand(targetFile);

	while (System::Update())
	{
		if (!app.update()) break;
	}
}
