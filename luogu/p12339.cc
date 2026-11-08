#include<bits/stdc++.h>
using namespace std;
int main(){
    int n;
    cin >>n;
    int sum = 0;
    for(int i =0; i<= 24; i++){
        sum += n+i;
    }
        cout << sum << endl;		
    return 0;
}
