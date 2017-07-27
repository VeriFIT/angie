#include "02_heap.h"

void body(void);

int main(int argc, char** argv)
{
  body();
  return 0;
}

void body()
{
  const int count = 3;
  single_list_struct segmentsHead;

  {
    single_list_struct* segment = &segmentsHead;
    int i = 0;
    for (; i < count; ++i)
    {
      segment->next = malloc(sizeof(single_list_struct));
      segment = segment->next;
      segment->data.first = i; // we are leaving data.second uninitialized
    }
    segment->next = NULL;
  }
  for (single_list_struct* ptr = segmentsHead.next; ptr != NULL; ptr = ptr->next)
  {
    set_inner_data(&ptr->data);
  }

  for (single_list_struct* ptr = segmentsHead.next; ptr != NULL; ptr = ptr->next)
  {
    check_inner_data(&ptr->data);
  }
}

// This example allocates nodes of single linked list separately and then links them together
