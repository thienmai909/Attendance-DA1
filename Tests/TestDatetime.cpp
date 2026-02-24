#include <iostream>
#include <stdexcept>
#include <models.hpp>

int main(int argc, char* argv[]) {
    if (argc < 2) return -1;

    std::string test(argv[1]);

    try {
        if (test == "valid") {
            DateTime dt(1, 5, 2025, 12, 30, 45);
            return !(dt.weekday() == 4 &&
                     dt.day() == 1 &&
                     dt.month() == 5 &&
                     dt.year() == 2025 &&
                     dt.hour() == 12 &&
                     dt.minute() == 30 &&
                     dt.second() == 45);
        }

        else if (test == "invalid_date") {
            try {
                DateTime(31, 2, 2025);
                return 1; // should throw
            } catch (const std::invalid_argument&) {
                return 0;
            }
        }

        else if (test == "invalid_time") {
            try {
                DateTime(1, 1, 2025, 25, 0, 0);
                return 1;
            } catch (const std::invalid_argument&) {
                return 0;
            }
        }

        else if (test == "now") {
            DateTime::now();
            return 0;
        }

        else if (test == "add_days") {
            DateTime dt(1, 1, 2025);
            DateTime future = dt.addDays(30);
            return !(future.day() == 31 && future.month() == 1);
        }

        else if (test == "add_days_month") {
            DateTime dt(31, 1, 2025);
            DateTime future = dt.addDays(1);
            return !(future.day() == 1 && future.month() == 2);
        }

        else if (test == "add_days_year") {
            DateTime dt(31, 12, 2025);
            DateTime future = dt.addDays(1);
            return !(future.day() == 1 &&
                     future.month() == 1 &&
                     future.year() == 2026);
        }

        else if (test == "days_between") {
            DateTime d1(1, 1, 2025);
            DateTime d2(11, 1, 2025);
            return d1.daysBetween(d2) != 10;
        }

        else if (test == "compare") {
            DateTime d1(1, 1, 2025);
            DateTime d2(2, 1, 2025);

            return !(d1 < d2 &&
                     !(d1 > d2) &&
                     !(d2 < d1) &&
                     (d2 > d1) &&
                     !(d1 == d2));
        }

    } catch (...) {
        return 1;
    }

    return -1;
}