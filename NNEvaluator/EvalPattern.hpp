# pragma once

# include <bit>

# include "../include/NNCpp/NNCpp.hpp"

namespace NNEvaluator
{
	struct EvalPattern
	{
		uint64_t mask;
		NNCpp::Modules::SimpleNet<float, NNCpp::Modules::ReLU<float>>net;

		EvalPattern(uint64_t mask_) : mask(mask_), net(std::popcount(mask_), 32, 1)
		{}
	};
}
