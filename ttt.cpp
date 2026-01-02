#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

int main(){

    srand((unsigned int)time (0));

    int target = rand() % 100 + 1;
    int guess;
    cout << "猜数字游戏开始! 请猜1-100之间的数: " << endl;
    
    while (true){
	cin >> guess;
        if (guess > target){
            cout << "猜大了，再试试!" << endl;
	} else if (guess < target){
	    cout << "猜小了,再试试!" << endl;	    
	} else {
            cout << "恭喜你, 猜对了!" << endl;
            break;
        }
	
    }
    return 0;
} 
