#include <ThongKeManager.hpp>
#include <algorithm>

ThongKeManager::ThongKeManager(
    LopHocPhanManager &lhpManager,
    SinhVienManager &svManager,
    DangKyHocManager &dkManager
) : _lhpManager(lhpManager)
  , _svManager(svManager)
  , _dkManager(dkManager)
{}

ThongKeSinhVien ThongKeManager::thongKeSV(
    const std::string &maLHP,
    const std::string &maSV
) const {
    const auto& lopHocPhan = timLop(maLHP);

    ThongKeSinhVien tkSV;
    tkSV.maSV = maSV;

    auto sinhVien = _svManager.timTheoMa(maSV);
    if (sinhVien.has_value())
        tkSV.tenSV = sinhVien->getTenSV();
    tinhChiTietSV(lopHocPhan, maSV, tkSV.soTietVang, tkSV.soTietMuon, tkSV.soTietCoMat);

    tkSV.tyLeVang = lopHocPhan.tyLeVang(maSV);
    tkSV.biCamThi = lopHocPhan.biCamThi(maSV);

    return tkSV;
}

std::vector<ThongKeSinhVien> ThongKeManager::thongKeToanLop(
    const std::string &maLHP
) const {
    auto dsMaSV = _dkManager.getDsMaSVTheoLop(maLHP);

    std::vector<ThongKeSinhVien> result;
    result.reserve(dsMaSV.size());
    for (const auto& maSV : dsMaSV)
        result.push_back(thongKeSV(maLHP, maSV));
    
    std::sort(result.begin(), result.end(),
        [](const ThongKeSinhVien& a, const ThongKeSinhVien& b) {
            return a.tyLeVang > b.tyLeVang;
        }
    );

    return result;
}

std::vector<ThongKeSinhVien> ThongKeManager::topSVVangNhieu(
    const std::string &maLHP,
    int topN
) const {
    auto all = thongKeToanLop(maLHP);
    if (static_cast<int>(all.size()) <= topN) return all;
    return { all.begin(), all.begin() + 5 };
}

std::vector<ThongKeSinhVien> ThongKeManager::dsSVBiCamThi(
    const std::string &maLHP
) const {
    auto all = thongKeToanLop(maLHP);
    std::vector<ThongKeSinhVien> result;
    for (const auto& tkSV : all)
        if (tkSV.biCamThi) result.push_back(tkSV);
    return result;
}

ThongKeBuoi ThongKeManager::thongKeBuoi(
    const std::string &maLHP,
    std::size_t buoiIndex
) const {
    const auto& lopHocPhan = timLop(maLHP);
    const auto& buoi = lopHocPhan.getDsBuoiDiemDanh().at(buoiIndex);

    ThongKeBuoi tkB;
    tkB.buoiIndex = buoiIndex;
    tkB.ngay = buoi.getNgayDiemDanhStr();
    tkB.ca = buoi.getCaDiemDanhStr();
    tkB.soTiet = buoi.getSoTiet();

    for (const auto& chiTiet : buoi.getDanhSachChiTiet()) {
        switch (chiTiet.getTrangThai()) {
            case Status::CO_MAT: ++tkB.soCoMat; break;
            case Status::VANG: ++tkB.soVang; break;
            case Status::MUON: ++tkB.soMuon; break;
            default: break;
        }
    }

    int tongSV = tkB.soCoMat + tkB.soVang + tkB.soMuon;
    tkB.tyLeCoMat = tongSV > 0
        ? static_cast<double>(tkB.soCoMat) / tongSV
        : 0.0;
    
    return tkB;
}

std::vector<ThongKeBuoi> ThongKeManager::thongKeTatCaBuoi(
    const std::string &maLHP
) const {
    const auto& lopHocPhan = timLop(maLHP);
    std::vector<ThongKeBuoi> result;
    for (
        std::size_t i = 0;
        i < lopHocPhan.getDsBuoiDiemDanh().size();
        ++i
    ) 
        result.push_back(thongKeBuoi(maLHP, i));
    return result;
}

std::optional<ThongKeBuoi> ThongKeManager::buoiVangCaoNhat(
    const std::string &maLHP
) const {
    auto all = thongKeTatCaBuoi(maLHP);
    if (all.empty()) return std::nullopt;

    return *std::max_element(all.begin(), all.end(),
                [](const ThongKeBuoi& a, const ThongKeBuoi& b)
                    { return a.tyLeCoMat > b.tyLeCoMat; }
            );
}

ThongKeLop ThongKeManager::thongKeLop(const std::string &maLHP) const
{
    const auto& lopHocPhan = timLop(maLHP);
    auto all = thongKeToanLop(maLHP);

    ThongKeLop tkL;
    tkL.maLHP = maLHP;
    tkL.tenLHP = lopHocPhan.getTenLHP();
    tkL.soSinhVien = static_cast<int>(all.size());
    tkL.soBuoiDaHoc = lopHocPhan.getSoBuoiDaHoc();
    tkL.soTietDaHoc = lopHocPhan.getSoTietDaHoc();

    if (!all.empty()) {
        double tongTyLe = 0.0;
        for (const auto& sinhVien : all) {
            tongTyLe += sinhVien.tyLeVang;
            if (sinhVien.biCamThi) ++tkL.soSVBiCamThi;
        }
        tkL.tyLeVangTrungBinh = tongTyLe / all.size();
    }
    
    return tkL;
}

std::vector<ThongKeLop> ThongKeManager::thongKeTatCaLop() const
{
    std::vector<ThongKeLop> result;
    for (const auto& lopHocPhan : _lhpManager.getAll())
        result.push_back(thongKeLop(lopHocPhan.getMaLHP()));
    
    std::sort(result.begin(), result.end(),
        [](const ThongKeLop& a, const ThongKeLop& b) {
            return a.tyLeVangTrungBinh > b.tyLeVangTrungBinh;
        }
    );

    return result;
}

std::optional<ThongKeLop> ThongKeManager::lopVangCaoNhat() const
{
    auto all = thongKeTatCaLop();
    if (all.empty()) return std::nullopt;
    return all.front();
}

std::map<CaHoc, double> ThongKeManager::tyLeVangTheoCa(
    const std::string &maLHP
) const {
    const auto& lopHocPhan = timLop(maLHP);
    std::map<CaHoc, int> tongSV, soVang;

    for (const auto& buoi : lopHocPhan.getDsBuoiDiemDanh()) {
        CaHoc caHoc = buoi.getCaDiemDanh();
        for (const auto& chiTiet : buoi.getDanhSachChiTiet()) {
            ++tongSV[caHoc];
            if (chiTiet.getTrangThai() == Status::VANG) ++soVang[caHoc];
        }
    }

    std::map<CaHoc, double> result;
    for (const auto& [caHoc, tong] : tongSV) 
        result[caHoc] = tong > 0 
            ? static_cast<double>(soVang[caHoc]) / tong 
            : 0.0;

    return result;
}

std::vector<std::pair<std::string, Status>> ThongKeManager::baoCaoBuoi(
    const std::string &maLHP,
    std::size_t buoiIndex
) const {
    const auto& buoi = timLop(maLHP).getDsBuoiDiemDanh().at(buoiIndex);
    std::vector<std::pair<std::string, Status>> result;
    for (const auto& chiTiet : buoi.getDanhSachChiTiet())
        result.emplace_back(chiTiet.getMaSV(), chiTiet.getTrangThai());
    return result;
}

std::vector<std::pair<std::string, int>> ThongKeManager::tongHopLop(
    const std::string &maLHP
) const {
    auto dsSV = thongKeToanLop(maLHP);
    std::vector<std::pair<std::string, int>> result;
    for (const auto& sinhVien : dsSV) 
        result.emplace_back(sinhVien.maSV, sinhVien.soTietVang);
    return result;
}

const LopHocPhan &ThongKeManager::timLop(const std::string &maLHP) const
{
    return _lhpManager.getLopRef(maLHP);
}

void ThongKeManager::tinhChiTietSV(
    const LopHocPhan &lhp,
    const std::string &maSV,
    int &soTietVang,
    int &soTietMuon,
    int &soTietCoMat
) const {
    soTietVang = soTietMuon = soTietCoMat = 0;
    for (const auto& buoi : lhp.getDsBuoiDiemDanh())
        for (const auto& chiTiet : buoi.getDanhSachChiTiet()) {
            if (chiTiet.getMaSV() != maSV) continue;
            switch(chiTiet.getTrangThai()) {
                case Status::VANG: soTietVang += buoi.getSoTiet(); break;
                case Status::MUON: soTietMuon += buoi.getSoTiet(); break;
                case Status::CO_MAT: soTietCoMat += buoi.getSoTiet(); break;
                default: break;
            }
        }
}
