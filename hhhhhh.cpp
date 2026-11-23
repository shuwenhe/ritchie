#include<bits/stdc++.h>

using namespace std;

int main(){
    int k;
    cin >> k;
    int ans = 0;
    for (int i = 10000; i <= 30000; i++){
         int sub1 =i/100;
         int sub2 = (i /10) % 1000;
         int sub3 =  i % 1000;
    if (sub1 % k == 0 && sub2 % k == 0 && sub3 % k == 0){
       cout << i << endl;
       ans ++;
    }	 
   }
   if (ans == 0){
       cout << "No" << endl;
   }
    return 0;
}
