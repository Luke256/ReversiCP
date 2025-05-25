# include "Main.hpp"
# include "Game.hpp"
# include "CodeExpander.hpp"

# include "include/CMat/CMat.hpp"
# include "include/NNCpp/NNCpp.hpp"


void Main()
{
	CMat::Matrix<float> a = CMat::Random::rand<float>(CMat::MatShape{ 20, 1024 });

	NNCpp::Modules::Dense<float>fc(1024, 20);
	NNCpp::Modules::ReLU<float>relu;

	fc.forward(a, a);
	relu.forward(a, a);

	a = CMat::ones<float>(CMat::MatShape{ 20, 20 });

	relu.backward(a, a);
	fc.backward(a, a);

	Console << a;

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
