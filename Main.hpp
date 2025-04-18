#pragma once
# include <Siv3D.hpp> // OpenSiv3D v0.6.15

struct GameData
{

};

using MyApp = SceneManager<String, GameData>;

namespace AppData
{
	const int32 Width = 1280;
	const int32 Height = 720;
};
