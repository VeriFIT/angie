#include <cstdlib>
#include <cstring>

int main(int argc, char** argv)
{
  int* mojexxx = new int();
  int* mojezzz = (int*)malloc(sizeof(int));
  *mojexxx = 64;
  memset(mojezzz, 0, sizeof(int));
  if (argc <= 2)
    *mojexxx = *mojexxx + argc;
  else
    *mojexxx = *mojexxx - argc;

  return *mojexxx;
}

