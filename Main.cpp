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

	CMat::CMat<int>y = {
		{9,8,7},
		{6,5,4},
		{3,2,1}
	};

	Console << U"x = " << x;
	Console << U"y = " << y;
	Console << U"x + y = " << x + y;

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
