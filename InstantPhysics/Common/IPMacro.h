#pragma once

namespace ip
{
#define IPSafeDelete(p)\
{\
	if (p != nullptr)\
	{\
		delete p;\
		p = nullptr;\
	}\
}

#define IPSafeDeleteArray(p)\
{\
	if (p != nullptr)\
	{\
		delete[] p;\
		p = nullptr;\
	}\
}
}