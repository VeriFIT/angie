#pragma once

#define __VERIFIER_plot(...) __ANGIE_DIAGNOSTICS_PLOT_MEMORY(__VA_ARGS__)
#define __VERIFIER_nondet_int(...) __ANGIE_CREATE_UNKNOWN_INT(__VA_ARGS__)
#define __VERIFIER_nondet_int(...) __ANGIE_CREATE_UNKNOWN_INT(__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

#if 1
  /* declare built-ins */
  void __ANGIE_DIAGNOSTICS_PLOT_MEMORY(const char *name, ...);
  char __ANGIE_CREATE_UNKNOWN_CHAR   (void);
  int  __ANGIE_CREATE_UNKNOWN_INT    (void);
#else
#endif

#ifdef __cplusplus
}
#endif
