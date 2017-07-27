#include <stdlib.h>

#define USE_FUNCTIONS 1
#include "01_mincase.h"

int main(int argc, char** argv)
{
  int supernumber = -1;
  supernumber = supernumber + argc;

  main_struct struktura_init = {0};
  main_struct struktura_neinit;
  main_struct* ptr_non_init;

  main_struct* ptr = &struktura_init;

  NASTAV_STRUKTURU(ptr);

  if (supernumber == 0)
  {
    ptr = NULL;
    return ZKONTROLUJ_STRUKTURU(ptr);
  }
  else if (supernumber == 1)
  {
    ptr = ptr_non_init;
    return ZKONTROLUJ_STRUKTURU(ptr);
  }
  else if (supernumber == 2)
  {
    ptr = &struktura_neinit;
    return ZKONTROLUJ_STRUKTURU(ptr);
  }
  else
  {
    return ZKONTROLUJ_STRUKTURU(ptr);
  }
}
