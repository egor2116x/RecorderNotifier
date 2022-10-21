#pragma once
#include <string>

namespace utils {
	bool findProcessByName(std::wstring procName);
	bool getProcessNameById(unsigned long processId, std::wstring& processName);
}

