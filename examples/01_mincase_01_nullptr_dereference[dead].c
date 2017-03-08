#include <stdlib.h>
#include <stddef.h>

#define USE_FUNCTIONS 0
#include "01_mincase.h"

int main(int argc, char** argv)
{
  int supernumber = -1;
  supernumber = supernumber + argc;

  main_struct struktura_init = {0};
  main_struct struktura_neinit;

  main_struct* ptr = &struktura_init;
  
  // The following offsets obtained by offsetoff are defined behaviour,
  // and should be handled in special way in the compiler.
  // We do not expect this to produce any getelementptr instructions in LLVM IR.

  // size_t offset_a = offsetof(main_struct, vnorena);
  // size_t offset_b = offset_a + offsetof(struct vnor_struct_s, b);

  // works

  NASTAV_STRUKTURU(ptr);
  ptr = NULL;
  return ZKONTROLUJ_STRUKTURU(ptr);
}
