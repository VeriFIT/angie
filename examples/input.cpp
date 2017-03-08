#include <vector>
#include <stdexcept>

void fce2(){
    int x;
    throw std::runtime_error("");
}

int main(int argc, char** argv){
    int supernumber = 1;
    supernumber = supernumber + argc;
    if(supernumber <= 2)
        supernumber++;
    else
    {
        try {fce2();}
        catch(std::runtime_error& e){throw;}
        catch(std::logic_error& e){return 0;}
    }
    return supernumber;
}

