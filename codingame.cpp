#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <bit>
#include "ReversiAgents/AlphaBetaAgent.hpp"

using namespace std;

int main()
{
	int id; // id of your player.
	cin >> id; cin.ignore();
	int board_size;
	cin >> board_size; cin.ignore();

	assert(board_size == 8);

	AlphaBetaAgent agent;
	Reversi::ReversiEngine engine;

	// game loop
	while (1) {
		uint64_t blacks = 0, whites = 0, mask = 0x8000000000000000;
		for (int i = 0; i < board_size; i++) {
			string line; // rows from top to bottom (viewer perspective).
			cin >> line; cin.ignore();
			for (char c : line)
			{
				if (c == '0') blacks |= mask;
				if (c == '1') whites |= mask;
				mask >>= 1;
			}
		}
		int action_count; // number of legal actions for this turn.
		cin >> action_count; cin.ignore();
		for (int i = 0; i < action_count; i++) {
			string action; // the action
			cin >> action; cin.ignore();
		}

		engine.setState(blacks, whites, id == 0);
		auto action = agent.play(engine);

		cout << char('a' + action.first) << char('1' + action.second) << endl;
	}
}
