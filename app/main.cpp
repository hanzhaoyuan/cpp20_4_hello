#include <format>
#include <iostream>
#include <string>

class Player {
public: // 外部可以访问
    std::string name;
    int health;

    // 成员函数：在类内部定义行为
    void takeDamage(int amount) {
        health -= amount;
        if (health < 0) {
            health = 0;
        }
    }
};

int main() {
    Player p;
    p.name = "llq";
    p.health = 20;
    std::cout << std::format("name: {}, health: {}", p.name, p.health) << '\n';

    p.takeDamage(30);
    std::cout << std::format("name: {}, health: {}", p.name, p.health) << '\n';

    return 0;
}
