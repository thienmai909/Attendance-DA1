#include <iostream>
#include <format>
#include <models.hpp>

int main() {
    Account acc1 = Account("thienmai", "thien909");
    std::cout << "Username: " << acc1.getUsername() << std::endl;
    std::cout << "Xác thực: " << (acc1.verifyPassword("thien909") ? "Thành công" : "Thất bại") << std::endl;

    DateTime now = DateTime::now();
    std::cout << "Now: " << now.toString() << "\n";

    DateTime future = now.addDays(30);
    std::cout << "After 30 days: "
              << future.toString() << "\n";

    std::cout << "Days between: "
              << now.daysBetween(future) << "\n";
}
