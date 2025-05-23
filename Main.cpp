# include "Main.hpp"
# include "Game.hpp"
# include "CodeExpander.hpp"
# include "lib/CMat/CMat.hpp"


void Main()
{
	CMat::CMat<int>x(CMat::MatShape{ 1000, 1000 });
	CMat::CMat<int>y(CMat::MatShape{ 1000, 1000 });

	auto start = std::chrono::high_resolution_clock::now();
	CMat::CMat<int>z = CMat::matmul(x, y);
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	Console << U"CMat: " << duration.count() << U" microseconds";

	Window::Resize(AppData::Width, AppData::Height);
	Scene::SetBackground(Palette::Lightblue);

	x = { {1,2,3},{4,5,6},{7,8,9} };
	y = { {9,8,7},{6,5,4},{3,2,1} };
	Console << CMat::matmul(x, y);

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
