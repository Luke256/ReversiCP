# pragma once

namespace NNCpp::Optim
{
	class Optimizer
	{
	public:
		virtual void zeroGrad() = 0;
		virtual void step() = 0;
	};
}
