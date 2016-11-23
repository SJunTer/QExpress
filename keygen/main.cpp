#include <iostream>
#include <string>
#include "../activate/activate.h"
using namespace std;

int main()
{
    Activate act;
    cout << "S/N" << endl;
    cout << act.ReturnId() << endl;
    cout << "本机密钥" << endl;
    cout << act.ReturnKey() << endl;
}
