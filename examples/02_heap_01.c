#include "02_heap.h"

void body(void);

int main(int argc, char** argv)
{
  body();
  return 0;
}

void body()
{
  const int count = 4;
  single_list_struct *segments = malloc(sizeof(single_list_struct) * count);

  for (int i = 0; i < count; ++i)
  {
    segments[i].data.first = i; // we are leaving data.second uninitialized
    segments[i].next = &segments[i + 1];
  }
  segments[count - 1].next = NULL;

  for (single_list_struct* ptr = segments; ptr != NULL; ptr = ptr->next)
  {
    set_inner_data(&ptr->data);
  }

  for (single_list_struct* ptr = segments; ptr != NULL; ptr = ptr->next)
  {
    check_inner_data(&ptr->data);
  }
}
