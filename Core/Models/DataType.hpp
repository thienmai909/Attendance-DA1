#pragma once

#include <string>
#include <chrono>

enum class RoomType {
    PhongLyThuyet,
    PhongThucHanh
};

enum class HocKi {
    I, II, He
};

enum class Degree {
    NONE = 0, CUNHAN, THACSI, TIENSI, PGSTS, GS
};

class Account {
    std::string _username;
    std::string _password_hash;

    Account(const std::string& username, const std::string& raw_password, bool isHash);
    
public:
    Account(const std::string& username, const std::string& raw_password);

    bool verifyPassword(const std::string& raw_password) const;
    const std::string& getUsername() const;
    const std::string& getPasswordHash() const;

    static Account loadFromHash(const std::string& username, const std::string& hash);
};

class DateTime {
    std::chrono::system_clock::time_point _tp;

public:
    DateTime();
    DateTime(int day, int month, int year, int hour = 0, int minute = 0, int second = 0);

    static DateTime now();

    int weekday() const;
    int day() const;
    int month() const;
    int year() const;
    int hour() const;
    int minute() const;
    int second() const;

    std::string toTimeString() const;
    std::string toString() const;

    DateTime addDays(int days) const;
    DateTime addHours(int hours) const;

    long long daysBetween(const DateTime& other) const;

    bool operator==(const DateTime& other) const;
    bool operator<(const DateTime& other) const;
    bool operator>(const DateTime& other) const;

};

class Contact {
    std::string _email;
    std::string _phoneNumber;
public:
    Contact(std::string email, std::string phoneNumber);

    const std::string& getEmail() const;
    const std::string& getPhoneNumber() const;
};

class StudyPeriod {
    int _starPeriod;
    int _endPeriod;
    DateTime _starTime;
    DateTime _endTime;

public:
    StudyPeriod(int tietBatDau, int tietKetThuc, DateTime tgBatDau, DateTime tgKetThuc);

    int getTietBatDau() const;
    int getTietKetThuc() const;

    std::string getThuNgay() const;

    std::string getTGBatDau() const;
    std::string getTGKetThuc() const;
};

class ClassRoom {
    std::string _maPhong;
    int _sucChua;
    RoomType _loaiPhong;

public:
    ClassRoom(std::string tenPhong, int succhua, RoomType loaiPhong);

    std::string getTenPhong() const;
    int getSucChua() const;
    std::string getLoaiPhong() const;
};