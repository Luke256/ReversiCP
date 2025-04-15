#pragma once
# include <Siv3D.hpp>

struct GameData
{

};

using MyApp = SceneManager<String, GameData>;

namespace AppData
{
	const int32 Width = 1280;
	const int32 Height = 720;
};
