#include <stdlib.h>
#include <stdbool.h>

typedef struct data_struct_s data_struct;
struct data_struct_s {
  int first;
  int second;
  struct inner_struct_s {
    char a;
    char b;
  } vnorena;
};

typedef struct ptr_struct_s ptr_struct;
struct ptr_struct_s {
  data_struct* ptr;
};

typedef struct single_list_struct_s {
  struct single_list_struct_s* next;
  data_struct data;
} single_list_struct;

typedef struct double_list_struct_s double_list_struct;
struct double_list_struct_s {
  double_list_struct* next;
  double_list_struct* prev;
  data_struct data;
};

void set_inner_data(data_struct* ptr)
{
  ptr[0].vnorena.a = (char)'a';
  ptr->vnorena.b = (char)'b';
}

int check_inner_data(data_struct* ptr)
{
  bool ret = true;
  ret &= ptr->vnorena.a == (char)'a';
  ret &= ptr->vnorena.b == (char)'b';
  return ret;
}
