#pragma once

#ifdef XTD_STATIC
	#define DLL /**/
#else
	#ifdef DLL_EXPORT
		#define DLL __declspec(dllexport)
	#else
		#define DLL __declspec(dllimport)
	#endif
#endif