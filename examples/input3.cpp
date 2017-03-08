#include <cstdlib>
#include <cstring>

int globalni_int;

int intova_funkce(int a, int b){
    ++globalni_int;
    return a + b;
}

void voidova_funkce(){
    ++globalni_int;
}

int (*func_ptr)(int, int);

int main(int argc, char** argv)
{
    intova_funkce(10,20);
    voidova_funkce();

    func_ptr = intova_funkce;
    func_ptr(20,60);

    int x = 1;
    int y = 2;
    x += 1;
    bool xy = x > y;

    float k = 2.0;
    float l = 3.5;
    k += 1.0;
    bool kl = k > l;

    return 0;
}

