#ifndef DEFS_H
#define DEFS_H

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
# define _grtk_export __declspec(dllexport)
# define _grtk_import __declspec(dllimport)
#elif defined(__GNUC__) && (__GNUC__ >= 4)
# define _grtk_export __attribute__((__visibility__("default")))
# define _grtk_import
#else
# define _grtk_export
# define _grtk_import
#endif

#ifdef GRAPH_TK_COMPILATION
# define _grtk_api _grtk_export
#else
# define _grtk_api _grtk_import
#endif

#define grtk_public _grtk_api extern

#endif
