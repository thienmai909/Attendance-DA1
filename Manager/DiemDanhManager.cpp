#include <DiemDanhManager.hpp>
#include <unordered_set>

DiemDanhManager::DiemDanhManager(LopHocPhanManager &lhpManager)
    : _lhpManager(lhpManager) {}

void DiemDanhManager::diemDanh(const std::string &maLHP, std::size_t buoiIndex,
                               const std::string &maSV, const DateTime &gio,
                               Status trangThai, const std::string &ghiChu) {
  auto &buoi = timLop(maLHP).getBuoi(buoiIndex);
  buoi.themChiTiet(maSV, gio, trangThai, ghiChu);
  _lhpManager.markDirty();
}

void DiemDanhManager::diemDanhHangLoat(const std::string &maLHP,
                                       std::size_t buoiIndex,
                                       const std::vector<std::string> &dsMaSV,
                                       Status tranThaiMacDinh) {
  auto &buoi = _lhpManager.getLopRef(maLHP).getBuoi(buoiIndex);
  DateTime now;
  for (const auto &maSV : dsMaSV)
    try {
      buoi.themChiTiet(maSV, now, tranThaiMacDinh, "");
    } catch (const std::invalid_argument &e) {
    }
  _lhpManager.markDirty();
}

void DiemDanhManager::khoaBuoiVaAutoVang(
    const std::string &maLHP, std::size_t buoiIndex,
    const std::vector<std::string> &dsMaSVTrongLop) {
  auto &lhp = _lhpManager.getLopRef(maLHP);
  auto &buoi = lhp.getBuoi(buoiIndex);

  // Tìm SV chưa điểm danh -> tự động VANG
  DateTime now;

  for (const auto &maSV : dsMaSVTrongLop) {
    if (!buoi.daDiemDanh(maSV))
      buoi.themChiTiet(maSV, now, Status::VANG, "Tự động - Chưa điểm danh");
  }

  lhp.ghiNhanBuoiHoc(buoi.getSoTiet());
  buoi.khoaBuoi();
  _lhpManager.markDirty();
}

void DiemDanhManager::capNhatTrangThai(const std::string &maLHP,
                                       std::size_t buoiIndex,
                                       const std::string &maSV,
                                       Status trangThai) {
  auto &buoi = timLop(maLHP).getBuoi(buoiIndex);
  buoi.capNhatTrangThai(maSV, trangThai);
  _lhpManager.markDirty();
}

void DiemDanhManager::khoaBuoi(const std::string &maLHP,
                               std::size_t buoiIndex) {
  timLop(maLHP).getBuoi(buoiIndex).khoaBuoi();
  _lhpManager.markDirty();
}

void DiemDanhManager::moKhoaBuoi(const std::string &maLHP,
                                 std::size_t buoiIndex) {
  auto &buoi = _lhpManager.getLopRef(maLHP).getBuoi(buoiIndex);
  if (!buoi.isKhoaDiemDanh())
    throw std::runtime_error("Buổi chưa bị khóa!");
  buoi.moKhoa();
  _lhpManager.markDirty();
}

void DiemDanhManager::xoaBuoi(const std::string &maLHP, std::size_t buoiIndex) {
  auto &lhp = _lhpManager.getLopRef(maLHP);
  if (buoiIndex >= lhp.getDsBuoiDiemDanh().size())
    throw std::out_of_range("Index buổi không hợp lệ!");
  lhp.xoaBuoiTaiIndex(buoiIndex);
  _lhpManager.markDirty();
}

int DiemDanhManager::soTietVang(const std::string &maLHP,
                                const std::string &maSV) const {
  const auto &lopHocPhan = timLopConst(maLHP);
  int tong = 0;
  for (const auto &buoi : lopHocPhan.getDsBuoiDiemDanh())
    if (auto *ct = buoi.findChiTiet(maSV))
      if (ct->getTrangThai() == Status::VANG)
        tong += buoi.getSoTiet();
  return tong;
}

double DiemDanhManager::tiLeVang(const std::string &maLHP,
                                 const std::string &maSV) const {
  const auto &lopHocPhan = timLopConst(maLHP);
  int tongTiet = lopHocPhan.getSoBuoiDaHoc();
  if (tongTiet == 0)
    return 0.0;
  return static_cast<double>(soTietVang(maLHP, maSV)) / tongTiet;
}

bool DiemDanhManager::biCamThi(const std::string &maLHP,
                               const std::string &maSV) const {
  return timLopConst(maLHP).biCamThi(maSV);
}

std::vector<std::string>
DiemDanhManager::dsSVBiCamThi(const std::string &maLHP) const {
  const auto &lopHocPhan = timLopConst(maLHP);
  std::vector<std::string> result;
  std::unordered_set<std::string> seen;
  for (const auto &buoi : lopHocPhan.getDsBuoiDiemDanh())
    for (const auto &chiTiet : buoi.getDanhSachChiTiet())
      seen.insert(chiTiet.getMaSV());

  for (const auto &maSV : seen)
    if (lopHocPhan.biCamThi(maSV))
      result.push_back(maSV);
  return result;
}

std::vector<std::string> DiemDanhManager::dsSVChuaDiemDanh(
    const std::string &maLHP, std::size_t buoiIndex,
    const std::vector<std::string> &dsMaSVTrongLop) const {
  const auto &buoi = timLopConst(maLHP).getDsBuoiDiemDanh().at(buoiIndex);

  std::vector<std::string> result;
  for (const auto &maSV : dsMaSVTrongLop)
    if (!buoi.daDiemDanh(maSV))
      result.push_back(maSV);
  return result;
}

std::vector<std::pair<std::string, Status>>
DiemDanhManager::baoCaoBuoi(const std::string &maLHP,
                            std::size_t buoiIndex) const {
  const auto &buoi = timLopConst(maLHP).getDsBuoiDiemDanh().at(buoiIndex);
  std::vector<std::pair<std::string, Status>> result;
  for (const auto &chiTiet : buoi.getDanhSachChiTiet())
    result.emplace_back(chiTiet.getMaSV(), chiTiet.getTrangThai());
  return result;
}

std::vector<std::pair<std::string, int>>
DiemDanhManager::tongHopLop(const std::string &maLHP) const {
  const auto &lopHocPhan = timLopConst(maLHP);
  std::vector<std::pair<std::string, int>> result;

  std::unordered_set<std::string> seen;
  for (const auto &buoi : lopHocPhan.getDsBuoiDiemDanh())
    for (const auto &chiTiet : buoi.getDanhSachChiTiet())
      seen.insert(chiTiet.getMaSV());

  for (const auto &maSV : seen)
    result.emplace_back(maSV, soTietVang(maLHP, maSV));

  return result;
}

void DiemDanhManager::capNhatGhiChu(const std::string &maLHP,
                                    std::size_t buoiIndex,
                                    const std::string &maSV,
                                    const std::string &ghiChu) {
  timLop(maLHP).getBuoi(buoiIndex).capNhatGhiChu(maSV, ghiChu);
  _lhpManager.markDirty();
}

LopHocPhan &DiemDanhManager::timLop(const std::string &maLHP) {
  return _lhpManager.getLopRef(maLHP);
}

const LopHocPhan &DiemDanhManager::timLopConst(const std::string &maLHP) const {
  return _lhpManager.getLopRef(maLHP);
}

DiemDanhManager::TrangThaiNguong
DiemDanhManager::kiemTraNguong(const std::string &maLHP,
                               const std::string &maSV) const {
  const auto &lhp = timLopConst(maLHP);
  TrangThaiNguong t;
  t.tyLeVang = lhp.tyLeVang(maSV);            // đã có
  double nguong = lhp.getNguongCamThi();
  t.phanTramNguong = (nguong > 0.0) ? (t.tyLeVang / nguong) : 0.0;
  t.soTietConLai   = lhp.soTietVangToiDaChoPhep(maSV); // đã có
  t.biCamThi       = lhp.biCamThi(maSV);      // đã có
  t.daVuotNguong   = !t.biCamThi && (t.phanTramNguong > 0.8);
  t.sapVuotNguong  = !t.biCamThi && !t.daVuotNguong && (t.phanTramNguong > 0.5);
  return t;
}

std::vector<DiemDanhManager::LichSuBuoi>
DiemDanhManager::lichSuDiemDanhSV(const std::string &maLHP,
                                   const std::string &maSV) const {
  const auto &lhp = timLopConst(maLHP);
  std::vector<LichSuBuoi> result;
  const auto &dsBuoi = lhp.getDsBuoiDiemDanh();
  result.reserve(dsBuoi.size());
  for (std::size_t i = 0; i < dsBuoi.size(); ++i) {
    const auto &buoi = dsBuoi[i];
    LichSuBuoi ls;
    ls.buoiIndex = i;
    ls.ngay      = buoi.getNgayDiemDanhStr();
    ls.ca        = buoi.getCaDiemDanhStr();
    ls.soTiet    = buoi.getSoTiet();
    if (const auto *ct = buoi.findChiTiet(maSV)) {
      ls.trangThai = ct->getTrangThai();
      ls.coPhep    = ct->isCoPhep();
      ls.ghiChu    = ct->getGhiChu();
    } else {
      ls.trangThai = Status::DEFAULT;
      ls.coPhep    = false;
      ls.ghiChu    = "";
    }
    result.push_back(std::move(ls));
  }
  return result;
}

void DiemDanhManager::capNhatCoPhep(const std::string &maLHP,
                                    std::size_t buoiIndex,
                                    const std::string &maSV, bool coPhep) {
  timLop(maLHP).getBuoi(buoiIndex).capNhatCoPhep(maSV, coPhep);
  _lhpManager.markDirty();
}
