#include <string>
#include <models.hpp>

int main(int argc, char* argv[]) {
    if (argc < 2) return -1;

    std::string test(argv[1]);

    Account acc("thienmai", "123456");

    if (test == "username")
        return acc.getUsername() != "thienmai";

    else if (test == "correct_password")
        return !acc.verifyPassword("123456");

    else if (test == "wrong_password")
        return acc.verifyPassword("wrong");

    return -1;
}
