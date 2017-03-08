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

  if (supernumber == 0)
  {
    ptr = NULL;
    return ZKONTROLUJ_STRUKTURU(ptr);
  }
  else if (supernumber == 1)
  {
    ptr = &struktura_neinit;
    return ZKONTROLUJ_STRUKTURU(ptr);
  }
  else
  {
    return ZKONTROLUJ_STRUKTURU(ptr);
  }
}

#if 0

int main(int argc, char** argv)
{
  int supernumber = -1;
  supernumber = supernumber + argc;

  main_struct struktura_init = {0};
  main_struct struktura_neinit;

  main_struct* ptr = &struktura_init;
  
  // nastav_strukturu
  ptr[0].vnorena.a = (char)'a';
  ptr->vnorena.b = (char)'b';

  if (supernumber == 0)
  {
    ptr = NULL;
    // zkontroluj_strukturu
    return (ptr->vnorena.a == (char)'a') & (ptr->vnorena.b == (char)'b');
  }
  else if (supernumber == 1)
  {
    ptr = &struktura_neinit;
    // zkontroluj_strukturu
    return (ptr->vnorena.a == (char)'a') & (ptr->vnorena.b == (char)'b');
  }
  else
  {
    // zkontroluj_strukturu
    return (ptr->vnorena.a == (char)'a') & (ptr->vnorena.b == (char)'b');
  }
}

#endif
