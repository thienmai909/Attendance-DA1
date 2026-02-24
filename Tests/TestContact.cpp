#include <string>
#include <models.hpp>

int main(int argc, char* argv[]) {
    if (argc < 2) return -1;

    std::string test(argv[1]);

    if (test == "getter") {
        Contact contact("thienmai0637@gmail.com", "0394023174");

        if (contact.getEmail() != "thienmai0637@gmail.com")
            return 1;

        if (contact.getPhoneNumber() != "0394023174")
            return 2;

        return 0;
    }

    if (test == "empty") {
        Contact c("", "");

        if (c.getEmail() != "")
            return 1;

        if (c.getPhoneNumber() != "")
            return 2;

        return 0;
    }

    return -1;
}
