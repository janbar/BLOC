#ifndef DECLSPEC_H_
#define DECLSPEC_H_

#if (defined(_WIN32) || defined(_WIN64))
#define LIBBLOC_DLL_EXPORT __declspec(dllexport)
#define LIBBLOC_DLL_IMPORT __declspec(dllimport)
#define LIBBLOC_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define LIBBLOC_DLL_EXPORT __attribute__ ((visibility ("default")))
#define LIBBLOC_DLL_IMPORT __attribute__ ((visibility ("default")))
#define LIBBLOC_DLL_LOCAL __attribute__ ((visibility ("hidden")))
#else
#define LIBBLOC_DLL_EXPORT
#define LIBBLOC_DLL_IMPORT
#define LIBBLOC_DLL_LOCAL
#endif
#endif

#ifdef LIB_DLL_EXPORTS /* defined if we are building the dll (instead of using it) */
#define LIBBLOC_API LIBBLOC_DLL_EXPORT
#else
#define LIBBLOC_API LIBBLOC_DLL_IMPORT
#endif

#endif /* DECLSPEC_H_ */
