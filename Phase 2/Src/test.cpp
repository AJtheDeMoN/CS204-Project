#include <iostream>
#include <sstream>
#include "./Headers/Common.h"
#include "./Headers/Execute.h"
#include "./Headers/Decode.h"
#include "./Headers/Pipeline.h"
#include "./Headers/Predictor.h"
using namespace std;


int main(int argv, char **argc){
    for(int i=0; i<argv; i++)
        cout<<argc[i]<<" \n"[i==argv-1];
    string a="69";
    cout<<stoi(a)<<'\n';
    return 0;
}