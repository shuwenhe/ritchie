#include<iostream>
int main() {
    int sum = 0;
    for (int i = 1; i <= 100; i++) {
     	sum += i;
    }
    std::cout << "1加到100的结果是: "<< sum << std::endl;
    return 0;
}    
