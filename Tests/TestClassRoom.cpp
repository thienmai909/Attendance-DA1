#include <string>
#include <models.hpp>

int main(int argc, char* argv[])
{
    if (argc < 2) return -1;

    std::string test(argv[1]);

    if (test == "lecture")
    {
        ClassRoom room("A101", 50, RoomType::PhongLyThuyet);

        if (room.getTenPhong() != "A101") return 1;
        if (room.getSucChua() != 50) return 1;
        if (room.getLoaiPhong() != "Phòng lý thuyết") return 1;

        return 0;
    }

    if (test == "lab")
    {
        ClassRoom room("B202", 30, RoomType::PhongThucHanh);

        if (room.getTenPhong() != "B202") return 1;
        if (room.getSucChua() != 30) return 1;
        if (room.getLoaiPhong() != "Phòng thực hành") return 1;

        return 0;
    }
    
    return -1;
}