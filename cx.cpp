#include<bits/stdc++.h>
using namespace std;
int main(){
    int x;
    cin >> x;
    if (x >= 100 && x % 2 == 0){
        cout << 1 << endl;
    }
    else
        if (x >= 100 && x % 2 == 1)
            cout << 2 << endl;
	else      
            if (x < 100 && x % 2 == 0)
                cout << 3  << endl;
            else     
                cout << 4  << endl;
    return 0;
}
