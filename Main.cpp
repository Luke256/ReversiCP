# include "Main.hpp"
# include "Game.hpp"
# include "CodeExpander.hpp"

# include "include/CMat/CMat.hpp"
# include "include/NNCpp/NNCpp.hpp"


void Main()
{
	CMat::Random::seed(0);

	CMat::Matrix<float> A = CMat::ones<float>(CMat::MatShape{ 100, 32 });
	NNCpp::Modules::Dense<float>fc1(32, 32);
	NNCpp::Modules::Dense<float>fc2(32, 1);
	NNCpp::Modules::ReLU<float>relu;
	NNCpp::Modules::MSELoss<float>loss;

	auto y = CMat::ones<float>(CMat::MatShape{ 100, 1 });

	auto p1 = fc1.parameters();
	auto p2 = fc2.parameters();
	p1.insert(p1.end(), p2.begin(), p2.end());

	NNCpp::Optim::SGD<float> optim(p1, 0.01);


	for (int32 i : step(10))
	{
		auto a = A;
		fc1.forward(a, a);
		relu.forward(a, a);
		fc2.forward(a, a);
		float l;
		loss.forward(a, y, l);

		Console << l;

		optim.zeroGrad();

		loss.backward(a);
		fc2.backward(a, a);
		relu.backward(a, a);
		fc1.backward(a, a);
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
