#include <DataType.hpp>

#include <sodium.h>

#include <string>
#include <stdexcept>
#include <chrono>
#include <sstream>
#include <iomanip>

// ================ Account ================
Account::Account(const std::string& username, const std::string& raw_password) : _username(username) {
    char hash[crypto_pwhash_STRBYTES];

    if (crypto_pwhash_str(
        hash,
        raw_password.c_str(),
        raw_password.size(),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE
    ) != 0) {
        throw std::runtime_error("Hashing failed");
    }

    _password_hash = hash;
}

bool Account::verifyPassword(const std::string& raw_password) const {
    return crypto_pwhash_str_verify(
        _password_hash.c_str(),
        raw_password.c_str(),
        raw_password.size()
    ) == 0;
}

const std::string& Account::getUsername() const {
    return _username;
}

// ================ Date ================
DateTime::DateTime() : _tp{std::chrono::system_clock::now()} {}

DateTime::DateTime(int day, int month, int year, int hour, int minute, int second) {
    std::chrono::year_month_day ymd {
        std::chrono::year {year},
        std::chrono::month {static_cast<unsigned int>(month)},
        std::chrono::day {static_cast<unsigned int>(day)}
    };
    
    if (!ymd.ok())
        throw std::invalid_argument("Invalid date");
    
    if (hour < 0 || hour > 23 ||
        minute < 0 || minute > 59 ||
        second < 0 || second > 59)
        throw std::invalid_argument("Invalid time");

    using namespace std::chrono;
    sys_days days {ymd};
    _tp = days + hours { hour } + minutes { minute} +   seconds {second};
}

DateTime DateTime::now()
{
    return DateTime();
}

int DateTime::weekday() const
{
    auto dp = floor<std::chrono::days>(_tp);
    return unsigned(std::chrono::weekday{dp}.c_encoding());
}

int DateTime::day() const
{
    auto dp = floor<std::chrono::days>(_tp);
    return unsigned(std::chrono::year_month_day{dp}.day());
}

int DateTime::month() const
{
    auto dp = floor<std::chrono::days>(_tp);
    return unsigned(std::chrono::year_month_day{dp}.month());
}

int DateTime::year() const
{
    auto dp = floor<std::chrono::days>(_tp);
    return int(std::chrono::year_month_day{dp}.year());
}

int DateTime::hour() const
{
    auto dp = floor<std::chrono::days>(_tp);
    std::chrono::hh_mm_ss hms{std::chrono::duration_cast<std::chrono::seconds>(_tp - dp)};
    return hms.hours().count();
}

int DateTime::minute() const
{
    auto dp = floor<std::chrono::days>(_tp);
    std::chrono::hh_mm_ss hms{std::chrono::duration_cast<std::chrono::seconds>(_tp - dp)};
    return hms.minutes().count();
}

int DateTime::second() const
{
    auto dp = floor<std::chrono::days>(_tp);
    std::chrono::hh_mm_ss hms{std::chrono::duration_cast<std::chrono::seconds>(_tp - dp)};
    return hms.seconds().count();
}

std::string DateTime::toTimeString() const
{
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hour()
        << ":"
        << std::setw(2) << std::setfill('0') << minute();
    return oss.str();
}

std::string DateTime::toString() const
{
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << day() << "/"
        << std::setw(2) << std::setfill('0') << month() << "/"
        << year() << " "
        << std::setw(2) << std::setfill('0') << hour() << ":"
        << std::setw(2) << std::setfill('0') << minute() << ":"
        << std::setw(2) << std::setfill('0') << second();
    return oss.str();
}

DateTime DateTime::addDays(int days) const
{
    DateTime result;
    result._tp = _tp + std::chrono::days { days };
    return result;
}

DateTime DateTime::addHours(int hours) const
{
    DateTime result;
    result._tp = _tp + std::chrono::hours { hours };
    return result;
}

long long DateTime::daysBetween(const DateTime &other) const
{
    auto d1 = floor<std::chrono::days>(_tp);
    auto d2 = floor<std::chrono::days>(other._tp);
    return (d2 - d1).count();
}

bool DateTime::operator==(const DateTime &other) const
{
    return _tp == other._tp;
}

bool DateTime::operator<(const DateTime &other) const
{
    return _tp < other._tp;
}

bool DateTime::operator>(const DateTime &other) const
{
    return _tp > other._tp;
}

// ================ Contact ================
Contact::Contact(std::string email, std::string phoneNumber) :
    _email(std::move(email)),
    _phoneNumber(std::move(phoneNumber))
{
}

const std::string &Contact::getEmail() const
{
    return _email;
}

const std::string &Contact::getPhoneNumber() const
{
    return _phoneNumber;
}

// ================ StudyPeriod ================
StudyPeriod::StudyPeriod(
    int tietBatDau,
    int tietKetThuc,
    DateTime tgBatDau,
    DateTime tgKetThuc) : 
        _starPeriod(tietBatDau), 
        _endPeriod(tietKetThuc), 
        _starTime(std::move(tgBatDau)), 
        _endTime(std::move(tgKetThuc))
{
}

int StudyPeriod::getTietBatDau() const
{
    return _starPeriod;
}

int StudyPeriod::getTietKetThuc() const
{
    return _endPeriod;
}

std::string StudyPeriod::getThuNgay() const
{
    std::string week[] = {"Chủ nhật", "Thứ 2", "Thứ 3", "Thứ 4", "Thứ 5", "Thứ 6", "Thứ 7"};

    std::ostringstream oss;
    oss << week[_starTime.weekday()] << ", "
        << std::setw(2) << std::setfill('0') << _starTime.day() << "/"
        << std::setw(2) << std::setfill('0') << _starTime.month() << "/"
        << _starTime.year();
    
    return oss.str();
}

std::string StudyPeriod::getTGBatDau() const
{
    return _starTime.toTimeString();
}

std::string StudyPeriod::getTGKetThuc() const
{
    return _endTime.toTimeString();
}

// ================ ClassRoom ================
ClassRoom::ClassRoom(std::string tenPhong, int succhua, RoomType loaiPhong) 
    : _maPhong(std::move(tenPhong)), _sucChua(succhua), _loaiPhong(loaiPhong)
{
}

std::string ClassRoom::getTenPhong() const
{
    return _maPhong;
}

int ClassRoom::getSucChua() const
{
    return _sucChua;
}

std::string ClassRoom::getLoaiPhong() const
{
    switch(_loaiPhong) {
        case RoomType::PhongLyThuyet:
            return "Phòng lý thuyết";
        case RoomType::PhongThucHanh:
            return "Phòng thực hành";
    }
    return "Không xác định";
}
