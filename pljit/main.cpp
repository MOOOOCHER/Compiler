#include <iostream>
#include "Milestone6/Pljit.h"
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
int main() {
    cout << "use for experiments" << endl;
    pljit::Pljit jit;
    auto handle = jit.registerFunction("VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1 ;RETURN ab;ab := 1 ;ab := 1 ;ab := 1  END.");
    cout << handle().value() << endl;
}
//---------------------------------------------------------------------------
