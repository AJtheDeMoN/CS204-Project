#include <iostream>
#include "./Headers/Common.h"
using namespace std;


int main(){
    Instruction a,b;
    a.funct3=3, a.funct7=20;
    b=a;
    Instruction c(a);
    cout<<a.funct3<<' '<<a.funct7<<'\n';
    cout<<b.funct3<<' '<<b.funct7<<'\n';
    cout<<c.funct3<<' '<<c.funct7<<'\n';
    return 0;
}