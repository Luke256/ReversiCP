# include "Main.hpp"
# include "Game.hpp"
# include "CodeExpander.hpp"

# include "include/CMat/CMat.hpp"
# include "include/NNCpp/NNCpp.hpp"

# include "NNEvaluator/Learner.hpp"

void Main()
{
	CMat::Random::seed(0);

	auto a = NNEvaluator::Learner();

	CMat::Matrix<float> A = CMat::Random::norm<float>(CMat::MatShape{ 100, 32 });
	CMat::Matrix<float> y = CMat::ones<float>(CMat::MatShape{ 100, 1 });

	auto model = NNCpp::Modules::SimpleNet<float, NNCpp::Modules::ReLU<float>>(32, 32, 1);

	auto loss = NNCpp::Modules::MSELoss<float>();

	NNCpp::Optim::SGD<float> optim(model.parameters(), 0.01);

	for (int32 i : step(200))
	{
		auto a = A;
		float l;
		model.forward(a, a);
		loss.forward(a, y, l);

		Console << l;

		optim.zeroGrad();

		loss.backward(a);
		model.backward(a, a);

		optim.step();
	}

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
