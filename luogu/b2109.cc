#include<iostream>
#include<string>
using namespace std;
	
int main() {      	
	string s;
	getline(cin,s);
	int cnt = 0;
	for(char c : s){
		if(c >= '0' && c <= '9'){
			cnt++;
			

		} 	
	}
        cout << cnt << endl;
	return 0;	
}
