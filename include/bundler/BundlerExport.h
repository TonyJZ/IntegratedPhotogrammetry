#ifndef _Bundler_exprot_define_
#define _Bundler_exprot_define_ 

//#include "BundleAdd.h"

#ifdef BUNDLERDLL_EXPORTS
#define _Bunder_dll_ __declspec(dllexport)
#else
#define _Bunder_dll_ __declspec(dllimport)
#endif

#ifndef BUNDLERDLL_EXPORTS
#ifdef _DEBUG
#pragma comment(lib,"bundlerDLL.lib")
#else
#pragma comment(lib,"bundlerDLL.lib")
#endif
#endif
//double _Bunder_dll_ sum(double a, double b);


//class  _Bunder_dll_ BundlerApp ;

#endif