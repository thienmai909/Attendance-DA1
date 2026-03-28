#include <DiemDanhManager.hpp>

DiemDanhManager::DiemDanhManager(LopHocPhanManager &lhpManager)
    : _lhpManager(lhpManager)
{}

void DiemDanhManager::diemDanh(
    const std::string &maLHP,
    std::size_t buoiIndex,
    const std::string &maSV,
    const DateTime &gio,
    Status trangThai,
    const std::string &ghiChu)
{
    auto& buoi = timLop(maLHP).getBuoi(buoiIndex);
    buoi.themChiTiet(maSV, gio, trangThai, ghiChu);
    _lhpManager.markDirty();
}

void DiemDanhManager::diemDanhHangLoat(
    const std::string &maLHP,
    std::size_t buoiIndex,
    const std::vector<std::string> &dsMaSV,
    Status tranThaiMacDinh
) {
    auto& buoi = _lhpManager.getLopRef(maLHP).getBuoi(buoiIndex);
    DateTime now;
    for (const auto& maSV : dsMaSV)
        try {
            buoi.themChiTiet(maSV, now, tranThaiMacDinh, "");
        } catch (const std::invalid_argument& e) {

        }
    _lhpManager.markDirty();
}

void DiemDanhManager::khoaBuoiVaAutoVang(
    const std::string &maLHP,
    std::size_t buoiIndex,
    const std::vector<std::string> &dsMaSVTrongLop
) {
    auto& lhp = _lhpManager.getLopRef(maLHP);
    auto& buoi = lhp.getBuoi(buoiIndex);

    // Tìm SV chưa điểm danh -> tự động VANG
    const auto& dsChiTiet = buoi.getDanhSachChiTiet();
    DateTime now;

    for (const auto& maSV : dsMaSVTrongLop) {
        bool daDiemDanh = false;
        for (const auto& chiTiet : dsChiTiet)
            if (chiTiet.getMaSV() == maSV) {
                daDiemDanh = true;
                break;
            }
        if (!daDiemDanh)
            buoi.themChiTiet(maSV, now, Status::VANG, "Tự động - Chưa điểm danh");
    }

    lhp.ghiNhanBuoiHoc(buoi.getSoTiet());
    buoi.khoaBuoi();
    _lhpManager.markDirty();
}

void DiemDanhManager::capNhatTrangThai(
    const std::string &maLHP,
    std::size_t buoiIndex,
    const std::string &maSV,
    Status trangThai
) {
    auto& buoi = timLop(maLHP).getBuoi(buoiIndex);
    buoi.capNhatTrangThai(maSV, trangThai);
    _lhpManager.markDirty();
}

void DiemDanhManager::khoaBuoi(const std::string &maLHP, std::size_t buoiIndex)
{
    timLop(maLHP).getBuoi(buoiIndex).khoaBuoi();
    _lhpManager.markDirty();

}

int DiemDanhManager::soTietVang(
    const std::string &maLHP,
    const std::string &maSV
) const {
    const auto& lopHocPhan = timLopConst(maLHP);
    int tong = 0;
    for (const auto& buoi : lopHocPhan.getDsBuoiDiemDanh())
        for (const auto& chiTiet : buoi.getDanhSachChiTiet())
            if (chiTiet.getMaSV() == maSV && chiTiet.getTrangThai() == Status::VANG)
                tong += buoi.getSoTiet();
    return tong;
}

double DiemDanhManager::tiLeVang(
    const std::string &maLHP,
    const std::string &maSV
) const {
    const auto& lopHocPhan = timLopConst(maLHP);
    int tongTiet = lopHocPhan.getSoBuoiDaHoc();
    if (tongTiet == 0) return 0.0;
    return static_cast<double>(soTietVang(maLHP, maSV)) / tongTiet;
}

bool DiemDanhManager::biCamThi(
    const std::string &maLHP,
    const std::string &maSV
) const {
    return timLopConst(maLHP).biCamThi(maSV);
}

std::vector<std::string> DiemDanhManager::dsSVBiCamThi(
    const std::string &maLHP
) const {
    const auto& lopHocPhan = timLopConst(maLHP);
    std::vector<std::string> result;
    // Thu thập tất cả maSV đã điểm danh trong lớp
    std::vector<std::string> dsMaSV;
    for (const auto& buoi : lopHocPhan.getDsBuoiDiemDanh())
        for (const auto& chiTiet : buoi.getDanhSachChiTiet()) {
            auto it = std::find(dsMaSV.begin(), dsMaSV.end(), chiTiet.getMaSV());
            if (it == dsMaSV.end())
                dsMaSV.push_back(chiTiet.getMaSV());
        }
    
    for (const auto& maSV : dsMaSV) 
        if (lopHocPhan.biCamThi(maSV))
            result.push_back(maSV);
    return result;
}

std::vector<std::string> DiemDanhManager::dsSVChuaDiemDanh(
    const std::string &maLHP,
    std::size_t buoiIndex,
    const std::vector<std::string> &dsMaSVTrongLop
) const {
    const auto& buoi = timLopConst(maLHP).getDsBuoiDiemDanh().at(buoiIndex);
    const auto& dsChiTiet = buoi.getDanhSachChiTiet();

    std::vector<std::string> result;
    for (const auto& maSV : dsMaSVTrongLop) {
        bool daDiemDanh = false;
        for (const auto& chiTiet : dsChiTiet)
            if (chiTiet.getMaSV() == maSV) {
                daDiemDanh = true;
                break;
            }
        if (!daDiemDanh)
            result.push_back(maSV);
    }
    return result;
}

std::vector<std::pair<std::string, Status>> DiemDanhManager::baoCaoBuoi(
    const std::string &maLHP,
    std::size_t buoiIndex
) const {
    const auto& buoi = timLopConst(maLHP).getDsBuoiDiemDanh().at(buoiIndex);
    std::vector<std::pair<std::string, Status>> result;
    for (const auto& chiTiet : buoi.getDanhSachChiTiet())
        result.emplace_back(chiTiet.getMaSV(), chiTiet.getTrangThai());
    return result;
}

std::vector<std::pair<std::string, int>> DiemDanhManager::tongHopLop(
    const std::string &maLHP
) const {
    const auto& lopHocPhan = timLopConst(maLHP);
    std::vector<std::pair<std::string, int>> result;
    
    // Thu thập danh sách maSV duy nhất
    std::vector<std::string> dsMaSV;
    for (const auto& buoi : lopHocPhan.getDsBuoiDiemDanh())
        for (const auto& chiTiet : buoi.getDanhSachChiTiet()) {
            if (std::find(dsMaSV.begin(), dsMaSV.end(), chiTiet.getMaSV()) == dsMaSV.end())
                dsMaSV.push_back(chiTiet.getMaSV());
        }
    for (const auto& maSV : dsMaSV)
        result.emplace_back(maSV, soTietVang(maLHP, maSV));
    
    return result;
}

LopHocPhan &DiemDanhManager::timLop(const std::string &maLHP)
{
    return _lhpManager.getLopRef(maLHP);
}

const LopHocPhan &DiemDanhManager::timLopConst(const std::string &maLHP) const
{
    auto result = _lhpManager.timTheoMa(maLHP);
    if (!result.has_value())
        throw std::invalid_argument("Không tìm thấy lớp: " + maLHP);
    return _lhpManager.getLopRef(maLHP);
}
