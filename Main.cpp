# include "Main.hpp"
# include "Game.hpp"
# include "CodeExpander.hpp"

# include "include/CMat/CMat.hpp"
# include "include/NNCpp/NNCpp.hpp"

# include "NNEvaluator/Learner.hpp"

void Main()
{
	CMat::Random::seed(std::time(nullptr));

	Reversi::ReversiEngine engine;
	engine.reset();

	auto learner = NNEvaluator::Learner();
	Console << learner.eval(engine);

	//Window::Resize(AppData::Width, AppData::Height);
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
