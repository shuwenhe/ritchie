#include<iostream>

using namespace std;

int main()
{
    // 1.算术运算符
    cout << "1 + 2 = " << 1 + 2 <<endl;
    cout << "1 + 2 - 3 * 4 = " <<1 + 2 - 3 * 4<< endl;
    
    int a = 20, b = 6; 
    cout << "a + b" << a + b << endl;
    cout << "a + 1" << a + 1 << endl; 
    
    cout << "a / b" << a / b << endl;
    short a2 = 3;
    long long b2 = 23435;
    cout << "a2 * b2 = " << a2 * b2 << endl;
    cout << "b2 / b = " <<b2 / b<< endl;

    float a3 = 20;
    cout << "a3 / b = " <<a3 / b << endl;
    
    cout << "a % b = " <<a % b<< endl;
    cout << "-a % b = " <<-a % b << endl;
    cout << "a3 % b = " <<a3 % b << endl;

    // 2.赋值运算符 
    a = 1;
    //1 = a   //错误, 1不是可修改的左值
    a = b + 5;
    //b + 5 = a;   //错误
    const int c = 10;
    //c = b;   //错误

    a = { 10 };
    int arr[] = {1,2,3,4,5};

    a = b = 5;

    //复合赋值运算符
    int  sum + a;
    //sum = sum + b;
    //sum = sum + c;
    sum += b;
    sum += c;

    cout << "a + b + c = " << sum << endl;

    //递增递减运算符
    cout << " ++a = " << ++a << endl;

    int i = 0, j;
    j = ++i;
    //j = ++i;
    cout << "i = " << i << endl;
    cout << "j = " << j << endl;

    cout << "---------------------------------------" << endl;

    // 3.关系和逻辑运算符
    // 关系
    cout << "1 < 2 :" << (1 < 2)<< endl; 
    cout << "a < b :" << (a < b) << endl;
    cout << "10 == 4 + 6 : " << (10 == 4 + 6) << endl;
    cout << " (10 == 4) + 6 : " << ((10 == 4) + 6) << endl;

    //逻辑
    cout << "1 < 2 && 3 >= 5 : " << (1 < 2 && 3 >= 5) << endl;
    cout << "!(1 < 2 || 3 >= 5): " <<! (1 < 2 || 3 >= 5) << endl;

    //短路求值
    i = 0;
    1 < 2 && ++i;
    cout << "i = " << i << endl;

    //条件
    cout << ((1 < 2 && ++i) ? "true" : "false") << endl;

    // 4.位运算符
    unsigned char bits = 0xb5;
    	    
    return 0;
}	
