#pragma once

#include <Windows.h>
#include <fstream>

namespace ip
{
	class IPDebug
	{
	public:
		static void GetDetailError(DWORD code, char* buf, size_t cbDest);
	};

	class Safe_fstream
	{
	public:
		Safe_fstream(std::fstream* pfstream) : m_pfstream(pfstream) {}
		~Safe_fstream()
		{
			if (m_pfstream->is_open())
				m_pfstream->close();
		}
	private:
		std::fstream* m_pfstream;
	};

	class Safe_ofstream
	{
	public:
		Safe_ofstream(std::ofstream* pofstream) : m_pofstream(pofstream) {}
		~Safe_ofstream()
		{
			if (m_pofstream->is_open())
				m_pofstream->close();
		}
	private:
		std::ofstream* m_pofstream;
	};

	class Safe_ifstream
	{
	public:
		Safe_ifstream(std::ifstream* pifstream) : m_pifstream(pifstream) {}
		~Safe_ifstream()
		{
			if (m_pifstream->is_open())
				m_pifstream->close();
		}
	private:
		std::ifstream* m_pifstream;
	};
}
