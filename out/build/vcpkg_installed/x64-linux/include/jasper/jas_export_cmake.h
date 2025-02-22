
#ifndef JAS_EXPORT_CMAKE_H
#define JAS_EXPORT_CMAKE_H

#ifdef LIBJASPER_STATIC_DEFINE
#  define JAS_EXPORT
#  define JAS_LOCAL
#else
#  ifndef JAS_EXPORT
#    ifdef libjasper_EXPORTS
        /* We are building this library */
#      define JAS_EXPORT 
#    else
        /* We are using this library */
#      define JAS_EXPORT 
#    endif
#  endif

#  ifndef JAS_LOCAL
#    define JAS_LOCAL 
#  endif
#endif

#ifndef LIBJASPER_DEPRECATED
#  define LIBJASPER_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef LIBJASPER_DEPRECATED_EXPORT
#  define LIBJASPER_DEPRECATED_EXPORT JAS_EXPORT LIBJASPER_DEPRECATED
#endif

#ifndef LIBJASPER_DEPRECATED_NO_EXPORT
#  define LIBJASPER_DEPRECATED_NO_EXPORT JAS_LOCAL LIBJASPER_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LIBJASPER_NO_DEPRECATED
#    define LIBJASPER_NO_DEPRECATED
#  endif
#endif

#endif /* JAS_EXPORT_CMAKE_H */
