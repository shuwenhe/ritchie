#include<bits/stdc++.h>

using namespace std;

int main(){
    int ans = 0;
    int a[10];
    int h;
    for(int i = 0; i <10; i++){
        cin >> a[i];	
    }


    cin >> h;


    for(int i = 0; i < 10; i++){
        if(a[i] <= h+30){
	   ans ++;
	}
    }
    cout << ans << endl;

    return 0;
}
