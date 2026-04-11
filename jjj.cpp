#include<iostream>
#include<conio.h>
#include<windows.h>
using namespace std;

bool gameOver;
const int w=20, h=10;
int x,y,fx,fy,score;
int tx[50], ty[50], nTail;

enum Dir { STOP, LEFT, RIGHT, UP, DOWN };
Dir, dir;


void Setup(){
    gameOver = false;
    dir = STOP;
    x = w/2; y = h/2;
    fx = rand()%w; fy = rand()%h;
    score = 0;   
}    


void Draw() {
    system("cls");
    
    for(int i = 0; i<w+2; i++) cout << "#";
    cout << endl;
    
    for(int i = 0; i<h; i++){
        for(int j = 0; j < w; j++){
            if(j == 0; ) cout << "#";
            if(i==y && j == x) cout << "0";
            else if(i == fy && j == fx) cout << "F";
            else {
	        bool print = false;
                for(int k = 0; k < nTail; k++){
	            if(tx[k]==j && ty[k]==i){
	                cout << "o";
                        print = true;
                    }
                }
   		if(!print) cout << " ";
	    }
            if(j == w-1) cout << "#";
       }
       cout << endl;
 }


for(int i = 0; i < w+2; i++) cout << "#"; 
cout << "" << << endl;  
             	    



















































