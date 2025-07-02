# pragma once

# include <vector>
# include <cmath>

# include "Optimizer.hpp"
# include "../Utils/ParamInfo.hpp"

namespace NNCpp::Optim
{
	template<class _Dty>
	class Adam : public Optimizer
	{
	private:
		Utils::ParamInfoList<_Dty> m_params;
		std::vector<double>v, s;
		double m_lr, m_beta1, m_beta2;
	public:
		Adam(): m_lr(0.0), m_beta1(0), m_beta2(0) {}
		Adam(const Utils::ParamInfoList<_Dty>& params, double lr = 1e-4, double beta1 = 0.99, double beta2 = 0.999) : m_params(params), m_lr(lr), m_beta1(beta1), m_beta2(beta2)
		{
			size_t size = 0;
			for (const Utils::ParamInfo<_Dty>& p : m_params)
			{
				size += p.size;
			}
			v.assign(size, 0);
			s.assign(size, 0);
		}

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
			double *v_ptr = v.data(), *s_ptr = s.data();
			for (const Utils::ParamInfo<_Dty>& param : m_params)
			{
				_Dty* dptr = param.data;
				_Dty* gptr = param.grad;
				_Dty* dend = dptr + param.size;
				while (dptr != dend)
				{
					*v_ptr = m_beta1 * (*v_ptr) + (1 - m_beta1) * (*gptr);
					*s_ptr = m_beta2 * (*s_ptr) + (1 - m_beta2) * (*gptr) * (*gptr);
					*dptr -= static_cast<_Dty>(m_lr * *v_ptr * std::exp(-0.5 * std::log(1e-6 + *s_ptr)));

					dptr++;
					gptr++;
					v_ptr++;
					s_ptr++;
				}
			}
		}
	};
}
