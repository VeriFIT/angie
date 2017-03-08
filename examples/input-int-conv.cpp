#include <cstdlib>
#include <cstring>

int main()
{
    int int32 = -1;
    unsigned uint32 = -1;

    long long int64 = -1;
    unsigned long long uint64 = -1;

    long long a = int32 + uint32;
    int b = int32 + uint32;

    int c = int64 + uint32;
    int d = int32 + uint64;

    unsigned e = int64 + uint32;
    unsigned f = int32 + uint64;

    long long g = int64 + uint32;
    long long h = int32 + uint64;

    unsigned long long i = int64 + uint32;
    unsigned long long j = int32 + uint64;

    return 0;
}

