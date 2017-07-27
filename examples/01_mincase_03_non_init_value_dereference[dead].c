#include <stdlib.h>

#define USE_FUNCTIONS 0
#include "01_mincase.h"

int main(int argc, char** argv)
{
  int supernumber = -1;
  supernumber = supernumber + argc;

  main_struct struktura_init = {0};
  main_struct struktura_neinit;

  main_struct* ptr = &struktura_init;

  NASTAV_STRUKTURU(ptr);

  ptr = &struktura_neinit;
  return ZKONTROLUJ_STRUKTURU(ptr);
}
