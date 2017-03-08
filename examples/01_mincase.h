#include <stdlib.h>

typedef struct main_struct_s main_struct;
struct main_struct_s {
  int prvni_int;
  int druhy_int;
  struct vnor_struct_s {
    char a;
    char b;
  } vnorena;
};

// dirty macro!
/* deliberate use of different addresing methods to see any difference in LLVM IR*/
#define NASTAV_STRUKTURU_M(ptr) \
  ptr[0].vnorena.a = (char)'a'; \
  ptr->vnorena.b = (char)'b';

#define ZKONTROLUJ_STRUKTURU_M(ptr) \
  ((ptr->vnorena.a == (char)'a') & (ptr->vnorena.b == (char)'b'))

#if(USE_FUNCTIONS)
void nastav_strukturu(main_struct* ptr)
{
  NASTAV_STRUKTURU_M(ptr)
}

int zkontroluj_strukturu(main_struct* ptr)
{
  return ZKONTROLUJ_STRUKTURU_M(ptr);
}
#define NASTAV_STRUKTURU(ptr) nastav_strukturu(ptr)
#define ZKONTROLUJ_STRUKTURU(ptr) zkontroluj_strukturu(ptr)
#else
#define NASTAV_STRUKTURU(ptr) NASTAV_STRUKTURU_M(ptr)
#define ZKONTROLUJ_STRUKTURU(ptr) ZKONTROLUJ_STRUKTURU_M(ptr)
#endif
