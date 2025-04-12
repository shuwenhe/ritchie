#include <iostream>
using namespace std;

int main()
{
    //输入用户年龄,判断后显示欢迎信息
    cout << "请输入您的芳龄: " << endl;
    int age;
    cin >> age;

    //单分支和双分支
    //if (age >= 18)
    //{	    
    //  cout << "欢迎您,成年人! " << endl;
    //}	    
    //else
    //{
    //  cout << "本程序不欢迎未成年人!" << endl;
    //}	    

    //使用条件运算符进行实现 
    //age >= 18 ? cout << "欢迎您,成年人" << endl :  cout << "本程序不欢迎未成年人!" << endl;
    //cout << "(age >= 18 ? "欢迎您,成年人 ! " : "本程序不欢迎未成年人! " ) << endl;


    //嵌套分支 (多分支)

    if (age >= 18)
    {	    
      cout << "欢迎您,成年人! " << endl;

    }	    
    else
    {
      cout << "本程序不欢迎未成年人!" << endl;
    }	    
    return 0;
    return 0;
}	
