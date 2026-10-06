#include<bits/stdc++.h>

using namespace std;

int main(){
    int n;
    cin >> n;
    int a,b;
    a = n / 10;
    b = n % 10;
    if (a == 7)
        cout << "yes" << endl;	
    else 
        if(b == 7)
            cout << "yes" << endl;	
        else	
	    cout << "no" << endl;    
    return 0;
}
