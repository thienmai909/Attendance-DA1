#include <BaoCaoScreen.hpp>
#include <UiHelper.hpp>
#include <algorithm>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

// ----------------------------------------------------------------
// Helper: thanh tab
// ----------------------------------------------------------------
static Element renderTabBar(int activeTab, bool isAdmin) {
  std::vector<std::string> tabs;
  std::vector<int> tabIdx;
  if (isAdmin) {
    tabs.push_back("[1] Tổng quan");
    tabIdx.push_back(0);
  }
  tabs.push_back("[2] Chi tiết lớp");
  tabIdx.push_back(1);
  tabs.push_back("[3] Danh sách SV");
  tabIdx.push_back(2);
  tabs.push_back("[4] Thống kê buổi");
  tabIdx.push_back(3);
  tabs.push_back("[5] Xuất file");
  tabIdx.push_back(4);

  Elements els;
  for (int i = 0; i < (int)tabs.size(); ++i) {
    auto e = text(" " + tabs[i] + " ");
    if (tabIdx[i] == activeTab)
      e = e | bold | inverted;
    else
      e = e | dim;
    els.push_back(e);
    if (i + 1 < (int)tabs.size())
      els.push_back(text("|"));
  }
  return hbox(std::move(els)) | border;
}

// ----------------------------------------------------------------
// Helper: trang thai SV theo nguong
// ----------------------------------------------------------------
static std::string trangThaiSV(double tyLeVang, bool biCamThi) {
  if (biCamThi)
    return "[CT] Cấm thi";
  if (tyLeVang > 0.18)
    return "[!!] Nguy";
  if (tyLeVang > 0.10)
    return " [!] Cần chú ý";
  return "      OK";
}

// ----------------------------------------------------------------
// screenBaoCao — main
// ----------------------------------------------------------------
void screenBaoCao(AppManager &app, const std::string &maGV) {
  auto gvOpt = app.getGVManager().timTheoMa(maGV);
  bool isAdmin = gvOpt.has_value() && gvOpt->isAdmin();
  std::string tenGV = gvOpt.has_value() ? gvOpt->getHoTenGV() : maGV;

  // Lay danh sach LHP theo quyền
  auto getLopList = [&]() -> std::vector<LopHocPhan> {
    return isAdmin ? app.getLHPManager().getAll()
                   : app.getLHPManager().getLopTheoGV(maGV);
  };

  // Tab hien tai: admin bat dau tab 0 (tong quan), GV bat dau tab 1
  int activeTab = isAdmin ? 0 : 1;

  // Chon lop chung (cho tab 2,3,4,5)
  int selLop = 0;
  int contentScroll = 0; // Y offset for tab content

  bool thoat = false;

  while (!thoat) {
    auto screen = ScreenInteractive::Fullscreen();
    int luaChon = -1;
    std::string thongBao;

    // Sort state (giu nguyen khi doi lop)
    SortSV sortSV = SortSV::VANG_DESC;
    SortLop sortLop = SortLop::VANG_DESC;
    SortBuoi sortBuoi = SortBuoi::NGAY_ASC;

    auto dsLop = getLopList();

    // ----------------------------------------------------------
    // Build entries LHP cho menu ben trai
    // ----------------------------------------------------------
    std::vector<std::string> lopEntries;
    for (const auto &lhp : dsLop)
      lopEntries.push_back(lhp.getMaLHP() + " " + lhp.getTenLHP());
    if (lopEntries.empty())
      lopEntries.push_back("(Chua co lop)");

    if (!dsLop.empty())
      selLop = std::min(selLop, (int)dsLop.size() - 1);

    // ----------------------------------------------------------
    // Radiobox chon loai bao cao (Tab 5)
    // ----------------------------------------------------------
    int selLoaiBaoCao = 0; // 0=toan dien, 1=SV, 2=camthi, 3=buoi, 4=tongquan
    std::vector<std::string> loaiBCEntries = {
        "Báo cáo toàn diện (4 sheet)", "Danh sách SV + thống kê",
        "Danh sách cấm thi", "Điem danh từng buổi", "Tổng quan tất cả lớp"};
    if (!isAdmin)
      loaiBCEntries.pop_back();

    // ----------------------------------------------------------
    // Components
    // ----------------------------------------------------------
    auto menuLop = Menu(&lopEntries, &selLop);
    auto radioLoai = Radiobox(&loaiBCEntries, &selLoaiBaoCao);

    auto btnXuat = Button("Xuất XLSX [X]", [&] {
      if (dsLop.empty()) {
        thongBao = "[ERR] Không có lớp đề xuất!";
        return;
      }
      std::string maLHPChon = dsLop[selLop].getMaLHP();
      try {
        std::string filePath;
        switch (selLoaiBaoCao) {
        case 0:
          filePath = app.getBCManager().xuatBaoCaoToanDien(maLHPChon);
          break;
        case 1:
          app.getBCManager().xuatBaoCaoSinhVien(maLHPChon);
          filePath = "output/" + maLHPChon + "_sinhvien_*.xlsx";
          break;
        case 2:
          app.getBCManager().xuatDanhSachCamThi(maLHPChon);
          filePath = "output/" + maLHPChon + "_camthi_*.xlsx";
          break;
        case 3:
          app.getBCManager().xuatBaoCaoTatCaBuoi(maLHPChon);
          filePath = "output/" + maLHPChon + "_tatca_buoi_*.xlsx";
          break;
        case 4:
          app.getBCManager().xuatBaoCaoTongQuan();
          filePath = "output/tongquan_*.xlsx";
          break;
        }
        // Lay ten file thuc te tu xuatBaoCaoToanDien
        if (selLoaiBaoCao == 0)
          thongBao = "[OK] Đã xuất: " + filePath;
        else
          thongBao = "[OK] Đã xuất vào thư mực output/";
        luaChon = 5; // refresh
        screen.Exit();
      } catch (const std::exception &e) {
        thongBao = std::string("[ERR] ") + e.what();
      }
    });

    auto layout = Container::Vertical({menuLop, radioLoai, btnXuat});

    auto renderer =
        Renderer(
            layout,
            [&] {
              // ---------------------- content theo tab ----------------------
              Element content = filler();

              // --- Tab 0: Tong quan (admin) ---
              if (activeTab == 0 && isAdmin) {
                // Sort label
                std::string sortLopLabel =
                    (sortLop == SortLop::VANG_DESC)  ? "[V] \u25bc %V\u1eafng"
                    : (sortLop == SortLop::SO_CT_DESC) ? "[C] \u25bc SV CT"
                    : (sortLop == SortLop::MA_LHP_AZ)  ? "[L] \u25b2 M\u00e3 LHP"
                    : (sortLop == SortLop::TEN_AZ)     ? "[A] \u25b2 T\u00ean A\u2192Z"
                                                       : "[Z] \u25bc T\u00ean Z\u2192A";

                auto dsLopTK = app.getTKManager().thongKeTatCaLop(sortLop);
                auto lopMax = app.getTKManager().lopVangCaoNhat();

                Elements rows;
                rows.push_back(
                    hbox({
                        text(" M\u00e3 LHP   ") | bold | size(WIDTH, EQUAL, 12),
                        text(" T\u00ean LHP                     ") | bold |
                            size(WIDTH, EQUAL, 30),
                        text(" SV  ") | bold | size(WIDTH, EQUAL, 6),
                        text(" Bu\u1ed5i") | bold | size(WIDTH, EQUAL, 6),
                        text(sortLop == SortLop::VANG_DESC
                                 ? " %V\u1eafng \u25bc"
                                 : " %V\u1eafng  ") |
                            bold | size(WIDTH, EQUAL, 9) |
                            (sortLop == SortLop::VANG_DESC ? inverted : nothing),
                        text(sortLop == SortLop::SO_CT_DESC ? " SV CT\u25bc" : " SV CT") |
                            bold | size(WIDTH, EQUAL, 7) |
                            (sortLop == SortLop::SO_CT_DESC ? inverted : nothing),
                        text(sortLop == SortLop::TEN_AZ ? " T\u00ean\u25b2"
                             : sortLop == SortLop::TEN_ZA  ? " T\u00ean\u25bc" : " T\u00ean  ") |
                            bold | size(WIDTH, EQUAL, 8) |
                            ((sortLop == SortLop::TEN_AZ || sortLop == SortLop::TEN_ZA) ? inverted : nothing),
                    }) |
                    color(Color::Default));
                rows.push_back(separator());
                for (const auto &lk : dsLopTK) {
                  int pct = (int)(lk.tyLeVangTrungBinh * 100);
                  bool warn = lopMax.has_value() && lk.maLHP == lopMax->maLHP;
                  auto row = hbox({
                      text(" " + lk.maLHP + " ") | size(WIDTH, EQUAL, 12),
                      text(" " + lk.tenLHP + " ") | size(WIDTH, EQUAL, 30),
                      text(" " + std::to_string(lk.soSinhVien) + " ") |
                          size(WIDTH, EQUAL, 6),
                      text(" " + std::to_string(lk.soBuoiDaHoc) + " ") |
                          size(WIDTH, EQUAL, 6),
                      text(" " + std::to_string(pct) + "%  ") |
                          size(WIDTH, EQUAL, 9),
                      text(" " + std::to_string(lk.soSVBiCamThi) + "    ") |
                          size(WIDTH, EQUAL, 7),
                  });
                  if (warn)
                    row = row | color(Color::Red);
                  else if (pct > 30)
                    row = row | color(Color::Magenta);
                  rows.push_back(row);
                }

                std::string maxInfo =
                    lopMax.has_value()
                        ? lopMax->maLHP + " - " + lopMax->tenLHP + " (" +
                              std::to_string(
                                  (int)(lopMax->tyLeVangTrungBinh * 100)) +
                              "%)"
                        : "(Ch\u01b0a c\u00f3 d\u1eef li\u1ec7u)";

                content = vbox(
                    {hbox({text(" L\u1edbp v\u1eafng cao nh\u1ea5t: ") | dim,
                           text(maxInfo) | bold | color(Color::Red), filler(),
                           text(" [S] S\u1eafp x\u1ebfp: ") | dim,
                           text(sortLopLabel) | bold | color(Color::Blue)}),
                     separator(), vbox(std::move(rows)) | frame | flex});
              }

              else if (activeTab == 1) {
                if (!dsLop.empty() && selLop < (int)dsLop.size()) {
                  const std::string &maLHP = dsLop[selLop].getMaLHP();
                  const auto &lhp = app.getTKManager().getLHPRef(maLHP);

                  auto dsSVAll = app.getTKManager().thongKeToanLop(maLHP);

                  auto byCa = app.getTKManager().tyLeVangTheoCa(maLHP);

                  int soSVTotal = (int)dsSVAll.size();
                  int soSVBiCamThi = 0;
                  double tongTyLe = 0.0;
                  int svNguy = 0, svChuY = 0;
                  for (const auto &sv : dsSVAll) {
                    tongTyLe += sv.tyLeVang;
                    if (sv.biCamThi)
                      ++soSVBiCamThi;
                    else if (sv.tyLeVang > 0.18)
                      ++svNguy;
                    else if (sv.tyLeVang > 0.10)
                      ++svChuY;
                  }
                  double tyLeVangTB =
                      soSVTotal > 0 ? tongTyLe / soSVTotal : 0.0;

                  int top5n = std::min(5, soSVTotal);

                  std::string tenGVLop = "-";
                  if (!lhp.getMaGV().empty()) {
                    auto gvL = app.getGVManager().timTheoMa(lhp.getMaGV());
                    if (gvL.has_value())
                      tenGVLop = gvL->getHoTenGV();
                  }
                  std::string tenPhong = lhp.getTenPhongHoc();
                  if (tenPhong.empty())
                    tenPhong = "(Chưa có)";

                  int pctTienDo = lhp.getTongSoTiet() > 0
                                      ? (int)(100.0 * lhp.getSoTietDaHoc() /
                                              lhp.getTongSoTiet())
                                      : 0;
                  int maxVang =
                      (int)(lhp.getTongSoTiet() * lhp.getNguongCamThi());
                  int nguongPct = (int)(lhp.getNguongCamThi() * 100);

                  Elements topRows;
                  for (int i = 0; i < top5n; ++i) {
                    const auto &sv = dsSVAll[i];
                    int con = maxVang - sv.soTietVang;
                    std::string conStr =
                        con > 0 ? "(còn " + std::to_string(con) + "t)"
                                : "(ĐÃ VƯỢT)";
                    topRows.push_back(hbox(
                        {text("  " + std::to_string(i + 1) + ". ") | dim,
                         text(sv.maSV + " - " + sv.tenSV) |
                             size(WIDTH, EQUAL, 28),
                         text("  " + std::to_string(sv.soTietVang) + "t / " +
                              std::to_string((int)(sv.tyLeVang * 100)) +
                              "%  ") |
                             bold,
                         text(conStr) | dim}));
                  }
                  if (dsSVAll.empty())
                    topRows.push_back(text("  (Chưa có dữ liệu điểm danh)") |
                                      dim);

                  Elements caRows;
                  for (const auto &[ca, ty] : byCa)
                    caRows.push_back(hbox(
                        {text("  Ca " + std::to_string((int)ca) + ": ") | dim,
                         text(std::to_string((int)(ty * 100)) + "%") | bold}));

                  // Manual scroll: build flat element list
                  Elements allE;
                  allE.push_back(text(" THÔNG TIN LỚP HỌC PHẦN ") | bold | inverted | center);
                  allE.push_back(hbox({text(" Giảng viên : ") | dim, text(tenGVLop) | bold}));
                  allE.push_back(hbox({text(" Phòng học  : ") | dim, text(tenPhong) | bold}));
                  allE.push_back(hbox({text(" Học kỳ     : ") | dim, text(lhp.getHocKiStr())}));
                  allE.push_back(hbox({text(" Ngưỡng CT  : ") | dim,
                    text(std::to_string(nguongPct) + "% (tối đa " + std::to_string(maxVang) + " tiết)") | bold}));
                  allE.push_back(hbox({text(" Tiến độ    : ") | dim,
                    text(std::to_string(lhp.getSoTietDaHoc()) + "/" + std::to_string(lhp.getTongSoTiet()) +
                         " tiết (" + std::to_string(pctTienDo) + "%)") | bold}));
                  allE.push_back(separator());
                  allE.push_back(text(" THỐNG KÊ ") | bold | inverted | center);
                  allE.push_back(hbox({text(" Số SV      : ") | dim, text(std::to_string(soSVTotal)) | bold}));
                  allE.push_back(hbox({text(" % Vắng TB  : ") | dim,
                    text(std::to_string((int)(tyLeVangTB * 100)) + "%") | bold}));
                  allE.push_back(hbox({text(" SV cấm thi : ") | dim,
                    text(std::to_string(soSVBiCamThi)) | bold | color(Color::Red)}));
                  allE.push_back(hbox({text(" SV nguy    : ") | dim,
                    text(std::to_string(svNguy)) | bold | color(Color::RedLight)}));
                  allE.push_back(hbox({text(" SV cần chú : ") | dim,
                    text(std::to_string(svChuY)) | bold | color(Color::Magenta)}));
                  allE.push_back(separator());
                  allE.push_back(text(" TOP 5 VẮNG NHIỀU: ") | bold);
                  for (auto& r : topRows) allE.push_back(r);
                  allE.push_back(separator());
                  allE.push_back(text(" VẮNG THEO CA: ") | bold);
                  for (auto& r : caRows) allE.push_back(r);
                  int cs1 = std::max(0, std::min(contentScroll, (int)allE.size()-1));
                  content = vbox({
                    hbox({filler(), text(" [J]▼ [K]▲  " + std::to_string(cs1+1) +
                                        "/" + std::to_string(allE.size()) + " ") | dim}),
                    vbox(std::vector<Element>(allE.begin()+cs1, allE.end())) | flex
                  }) | flex;
                }
              }

              // --- Tab 2: Danh sach SV ---
              else if (activeTab == 2) {
                if (!dsLop.empty() && selLop < (int)dsLop.size()) {
                  const std::string &maLHP = dsLop[selLop].getMaLHP();
                  // Truyen sort vao ham
                  auto dsSV = app.getTKManager().thongKeToanLop(maLHP, sortSV);
                  const auto &lhp2 = app.getTKManager().getLHPRef(maLHP);
                  int maxVang2 =
                      (int)(lhp2.getTongSoTiet() * lhp2.getNguongCamThi());

                  // Sort label cho indicator
                  std::string sortSVLabel =
                      (sortSV == SortSV::VANG_DESC)  ? "\u25bc V\u1eafng%"
                      : (sortSV == SortSV::VANG_ASC) ? "\u25b2 V\u1eafng%"
                      : (sortSV == SortSV::TEN_AZ)   ? "\u25b2 T\u00ean A\u2192Z"
                      : (sortSV == SortSV::TEN_ZA)   ? "\u25bc T\u00ean Z\u2192A"
                                                     : "\u25b2 M\u00e3 SV";

                  int nCT = 0, nNguy = 0, nChuY = 0;
                  for (const auto &sv : dsSV) {
                    if (sv.biCamThi)
                      ++nCT;
                    else if (sv.tyLeVang > 0.18)
                      ++nNguy;
                    else if (sv.tyLeVang > 0.10)
                      ++nChuY;
                  }

                  Elements svRows;
                  // Dong tom tat + sort indicator
                  int nOk = (int)dsSV.size() - nCT - nNguy - nChuY;
                  svRows.push_back(hbox({
                      text(" T\u1ed5ng: " + std::to_string(dsSV.size()) +
                           " SV  ") |
                          dim,
                      text("[CT] " + std::to_string(nCT) + "  ") |
                          color(Color::Red),
                      text("[!!] " + std::to_string(nNguy) + "  ") |
                          color(Color::RedLight),
                      text("[!] " + std::to_string(nChuY) + "  ") |
                          color(Color::Magenta),
                      text("[OK] " + std::to_string(nOk)) | color(Color::Green),
                      text("  | T\u1ed1i \u0111a v\u1eafng: " +
                           std::to_string(maxVang2) + " ti\u1ebft") |
                          dim,
                      filler(),
                      text(" [S] S\u1eafp: ") | dim,
                      text(sortSVLabel) | bold | color(Color::Blue),
                  }));
                  svRows.push_back(separator());
                  svRows.push_back(
                      hbox({
                          text(" # ") | size(WIDTH, EQUAL, 4),
                          text(" M\u00e3 SV    ") | bold |
                              size(WIDTH, EQUAL, 11) |
                              (sortSV == SortSV::MSSV ? inverted : nothing),
                          text(" H\u1ecd T\u00ean                    ") | bold |
                              size(WIDTH, EQUAL, 28) |
                              ((sortSV == SortSV::TEN_AZ || sortSV == SortSV::TEN_ZA) ? inverted : nothing),
                          text(" V\u1eafng") | bold | size(WIDTH, EQUAL, 6) |
                              (sortSV == SortSV::VANG_DESC ||
                                       sortSV == SortSV::VANG_ASC
                                   ? inverted
                                   : nothing),
                          text(" Mu\u1ed9n") | bold | size(WIDTH, EQUAL, 6),
                          text(" % V\u1eafng") | bold | size(WIDTH, EQUAL, 8) |
                              (sortSV == SortSV::VANG_DESC ||
                                       sortSV == SortSV::VANG_ASC
                                   ? inverted
                                   : nothing),
                          text(" C\u00f2n v\u1eafng") | bold |
                              size(WIDTH, EQUAL, 10),
                          text(" Tr\u1ea1ng th\u00e1i") | bold,
                      }) |
                      color(Color::Default));
                  svRows.push_back(separator());
                  for (int i = 0; i < (int)dsSV.size(); ++i) {
                    const auto &sv = dsSV[i];
                    int pct = (int)(sv.tyLeVang * 100);
                    int con = maxVang2 - sv.soTietVang;
                    std::string conStr =
                        con > 0 ? std::to_string(con) + "t" : "VƯỢT";
                    std::string tt = trangThaiSV(sv.tyLeVang, sv.biCamThi);
                    Color col = sv.biCamThi          ? Color::Red
                                : sv.tyLeVang > 0.18 ? Color::RedLight
                                : sv.tyLeVang > 0.10 ? Color::Yellow
                                                     : Color::Black;
                    auto row =
                        hbox({
                            text(" " + std::to_string(i + 1) + " ") |
                                size(WIDTH, EQUAL, 4),
                            text(" " + sv.maSV + " ") | size(WIDTH, EQUAL, 11),
                            text(" " + sv.tenSV + " ") | size(WIDTH, EQUAL, 28),
                            text(" " + std::to_string(sv.soTietVang) + "t ") |
                                size(WIDTH, EQUAL, 6),
                            text(" " + std::to_string(sv.soTietMuon) + "t ") |
                                size(WIDTH, EQUAL, 6),
                            text(" " + std::to_string(pct) + "% ") |
                                size(WIDTH, EQUAL, 8),
                            text(" " + conStr + "      ") |
                                size(WIDTH, EQUAL, 10) |
                                (con <= 0 ? color(Color::Red)
                                          : color(Color::Green)),
                            text(" " + tt) | size(WIDTH, EQUAL, 16),
                        }) |
                        color(col);
                    svRows.push_back(row);
                  }
                  if (dsSV.empty())
                    svRows.push_back(text("  (Chưa có dữ liệu điểm danh)") |
                                     dim);

                  int cs2 = std::max(0, std::min(contentScroll, (int)svRows.size()-1));
                  content = vbox({
                    hbox({filler(), text(" [J]▼ [K]▲  " + std::to_string(cs2+1) +
                                        "/" + std::to_string(svRows.size()) + " ") | dim}),
                    vbox(std::vector<Element>(svRows.begin()+cs2, svRows.end())) | flex
                  }) | flex;
                }
              }

              // --- Tab 3: Thong ke buoi ---
              else if (activeTab == 3) {
                if (!dsLop.empty() && selLop < (int)dsLop.size()) {
                  const std::string &maLHP = dsLop[selLop].getMaLHP();
                  auto dsBuoi =
                      app.getTKManager().thongKeTatCaBuoi(maLHP, sortBuoi);
                  auto maxB = app.getTKManager().buoiVangCaoNhat(maLHP);
                  std::size_t maxIdx =
                      maxB.has_value() ? maxB->buoiIndex : SIZE_MAX;

                  // Sort label
                  std::string sortBuoiLabel =
                      (sortBuoi == SortBuoi::NGAY_ASC)    ? "\u25b2 Ng\u00e0y"
                      : (sortBuoi == SortBuoi::COMAT_ASC) ? "\u25b2 %CM"
                                                          : "\u25bc V\u1eafng";

                  Elements bRows;
                  // Sort indicator row
                  bRows.push_back(hbox({
                      filler(),
                      text(" [S] S\u1eafp x\u1ebfp: ") | dim,
                      text(sortBuoiLabel) | bold | color(Color::Blue),
                  }));
                  bRows.push_back(
                      hbox({
                          text(" Bu\u1ed5i") | bold | size(WIDTH, EQUAL, 6) |
                              (sortBuoi == SortBuoi::NGAY_ASC ? inverted
                                                              : nothing),
                          text(" Ng\u00e0y          ") | bold |
                              size(WIDTH, EQUAL, 15),
                          text(" Ca  ") | bold | size(WIDTH, EQUAL, 6),
                          text(" Ti\u1ebft") | bold | size(WIDTH, EQUAL, 6),
                          text("  CM ") | bold | size(WIDTH, EQUAL, 6),
                          text(" V\u1eafng") | bold | size(WIDTH, EQUAL, 6) |
                              (sortBuoi == SortBuoi::VANG_DESC ? inverted
                                                               : nothing),
                          text(" Mu\u1ed9n") | bold | size(WIDTH, EQUAL, 6),
                          text(" %CM    ") | bold | size(WIDTH, EQUAL, 9) |
                              (sortBuoi == SortBuoi::COMAT_ASC ? inverted
                                                               : nothing),
                      }) |
                      color(Color::Default));
                  bRows.push_back(separator());
                  for (const auto &b : dsBuoi) {
                    bool isMax = (b.buoiIndex == maxIdx);
                    int pct = (int)(b.tyLeCoMat * 100);
                    auto row = hbox({
                        text(" " + std::to_string(b.buoiIndex + 1) + "    ") |
                            size(WIDTH, EQUAL, 6),
                        text(" " + b.ngay + " ") | size(WIDTH, EQUAL, 15),
                        text(" " + b.ca + "  ") | size(WIDTH, EQUAL, 6),
                        text(" " + std::to_string(b.soTiet) + "  ") |
                            size(WIDTH, EQUAL, 6),
                        text("  " + std::to_string(b.soCoMat) + " ") |
                            size(WIDTH, EQUAL, 6),
                        text(" " + std::to_string(b.soVang) + "  ") |
                            size(WIDTH, EQUAL, 6),
                        text(" " + std::to_string(b.soMuon) + "  ") |
                            size(WIDTH, EQUAL, 6),
                        text(" " + std::to_string(pct) + "%") |
                            size(WIDTH, EQUAL, 9),
                    });
                    if (isMax)
                      row = row | color(Color::Red);
                    bRows.push_back(row);
                  }
                  if (dsBuoi.empty())
                    bRows.push_back(text("  (Chưa có buổi điểm danh)") | dim);

                  int cs3 = std::max(0, std::min(contentScroll, (int)bRows.size()-1));
                  content = vbox({
                    hbox({filler(), text(" [J]▼ [K]▲  " + std::to_string(cs3+1) +
                                        "/" + std::to_string(bRows.size()) + " ") | dim}),
                    vbox(std::vector<Element>(bRows.begin()+cs3, bRows.end())) | flex
                  }) | flex;
                }
              }

              // --- Tab 4: Xuat file ---
              else if (activeTab == 4) {
                std::string maLHPChon =
                    (!dsLop.empty() && selLop < (int)dsLop.size())
                        ? dsLop[selLop].getMaLHP()
                        : "";
                std::string tenLHPChon =
                    (!dsLop.empty() && selLop < (int)dsLop.size())
                        ? dsLop[selLop].getTenLHP()
                        : "";

                content = vbox(
                    {text(" XUẤT BÁO CÁO XLSX ") | bold | center, separator(),
                     hbox({text(" Lớp học phần : ") | dim,
                           text(maLHPChon + " - " + tenLHPChon) | bold}),
                     separator(), text(" Loại báo cáo:") | dim,
                     radioLoai->Render() | border, separator(),
                     btnXuat->Render() | center, separator(),
                     UiHelper::makeMessage(thongBao), filler()});
              }

              // ---------------------- frame chung ----------------------
              Element lopPanel = vbox({text(" DANH SÁCH LỚP ") | bold | center,
                                       separator(), menuLop->Render() | flex}) |
                                 border | size(WIDTH, EQUAL, 28);

              return vbox({UiHelper::makeHeader("BÁO CÁO & THỐNG KÊ",
                                                isAdmin ? "Admin: " + tenGV
                                                        : "GV: " + tenGV),
                           renderTabBar(activeTab, isAdmin),
                           hbox({lopPanel, content | border | flex}) | flex,
                           separator(), UiHelper::makeMessage(thongBao),
                           UiHelper::makeFooter(
                               isAdmin ? "[1] Tổng quan  [2] Chi tiết  [3] SV  "
                                         "[4] Buổi  [5] Xuất  [Q] Quay lại"
                                       : "[2] Chi tiết  [3] SV  [4] Buổi  [5] "
                                         "Xuất  [Q] Quay lại")});
            }) |
        CatchEvent([&](Event e) {
          // FIX #7: Chi cho admin chuyen sang tab 0
          if (e == Event::Character('1') && isAdmin) {
            activeTab = 0; contentScroll = 0;
            return true;
          }
          if (e == Event::Character('1') && !isAdmin) {
            activeTab = 1; contentScroll = 0;
            return true;
          } // GV nhan [1] -> chi tiet lop
          if (e == Event::Character('2')) {
            activeTab = 1; contentScroll = 0;
            return true;
          }
          if (e == Event::Character('3')) {
            activeTab = 2; contentScroll = 0;
            return true;
          }
          if (e == Event::Character('4')) {
            activeTab = 3; contentScroll = 0;
            return true;
          }
          if (e == Event::Character('5')) {
            activeTab = 4; contentScroll = 0;
            return true;
          }
          // Xuat nhanh
          if ((e == Event::Character('x') || e == Event::Character('X')) &&
              activeTab == 4) {
            btnXuat->OnEvent(Event::Return);
            return true;
          }
          if (e == Event::Character('q') || e == Event::Character('Q') ||
              e == Event::Escape) {
            luaChon = 99;
            screen.Exit();
            return true;
          }
          // [S] Sort cycle theo tab hien tai
          if (e == Event::Character('s') || e == Event::Character('S')) {
            if (activeTab == 0 && isAdmin) {
              sortLop =
                  static_cast<SortLop>((static_cast<int>(sortLop) + 1) % 5);
              return true;
            }
            if (activeTab == 2) {
              sortSV = static_cast<SortSV>((static_cast<int>(sortSV) + 1) % 5);
              return true;
            }
            if (activeTab == 3) {
              sortBuoi =
                  static_cast<SortBuoi>((static_cast<int>(sortBuoi) + 1) % 3);
              return true;
            }
          }
          // Content scroll with J/K
          if ((e == Event::Character('j') || e == Event::Character('J')) &&
              activeTab >= 1 && activeTab <= 3) {
            contentScroll++;
            return true;
          }
          if ((e == Event::Character('k') || e == Event::Character('K')) &&
              activeTab >= 1 && activeTab <= 3) {
            contentScroll = std::max(0, contentScroll - 1);
            return true;
          }
          return false;
        });

    screen.Loop(renderer);
    if (luaChon == 99)
      thoat = true;
    // luaChon == 5 → loop lai (refresh sau khi xuat)
  }
}
