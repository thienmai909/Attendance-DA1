#include <iostream>
#include <string>
#include <models.hpp>

#define ASSERT_TRUE(cond) if (!(cond)) return __LINE__;

int main(int argc, char* argv[]) {
    if (argc < 2) return -1;

    std::string test(argv[1]);

    // Tạo DateTime mẫu
    DateTime start(2, 6, 2025, 13, 0, 0);  // Ví dụ: 02/06/2025 13:00
    DateTime end(2, 6, 2025, 15, 30, 0);

    StudyPeriod sp(7, 9, start, end);

    if (test == "period") {
        ASSERT_TRUE(sp.getTietBatDau() == 7);
        ASSERT_TRUE(sp.getTietKetThuc() == 9);
        return 0;
    }

    if (test == "time_string") {
        ASSERT_TRUE(sp.getTGBatDau() == "13:00");
        ASSERT_TRUE(sp.getTGKetThuc() == "15:30");
        return 0;
    }

    if (test == "thu_ngay") {
        ASSERT_TRUE(sp.getThuNgay() == "Thứ 2, 02/06/2025");
        return 0;
    }

    if (test == "consistency") {
        ASSERT_TRUE(sp.getTietBatDau() < sp.getTietKetThuc());
        ASSERT_TRUE(sp.getTGBatDau() < sp.getTGKetThuc());
        return 0;
    }

    return -1;
}