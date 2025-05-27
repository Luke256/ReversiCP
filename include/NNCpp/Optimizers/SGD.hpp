# pragma once

# include "Optimizer.hpp"
# include "../Utils/ParamInfo.hpp"

namespace NNCpp::Optim
{
	template<class _Dty>
	class SGD : public Optimizer
	{
	private:
		Utils::ParamInfoList<_Dty> m_params;
		_Dty m_lr;
	public:
		SGD(const Utils::ParamInfoList<_Dty>& params, _Dty lr = 1e-4) : m_params(params), m_lr(lr)
		{}

		void zeroGrad() override
		{
			for (const Utils::ParamInfo<_Dty>& param : m_params)
			{
				_Dty* ptr = param.grad;
				_Dty* end = ptr + param.size;
				while (ptr != end)
				{
					*ptr++ = static_cast<_Dty>(0);
				}
			}
		}

		void step() override
		{
			for (const Utils::ParamInfo<_Dty>& param : m_params)
			{
				_Dty* dptr = param.data;
				_Dty* gptr = param.grad;
				_Dty* dend = dptr + param.size;
				while (dptr != dend)
				{
					*dptr++ -= *gptr++ * m_lr;
				}
			}
		}
	};
}
