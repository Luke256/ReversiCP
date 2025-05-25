# include "Main.hpp"
# include "Game.hpp"
# include "CodeExpander.hpp"

# include "include/CMat/CMat.hpp"


void Main()
{
	CMat::Matrix<float> a(CMat::MatShape{ 1024, 1024 });
	CMat::Matrix<float> b(CMat::MatShape{ 1024, 1024 });
	CMat::Matrix<float> c(CMat::MatShape{ 1024, 1024 });

	auto start = std::chrono::high_resolution_clock::now();
	c = CMat::matmul(a, b);
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	Console << U"time: " << duration << U"ms";


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
