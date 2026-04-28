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
  if (tyLeVang > 0.8)
    return "[!!] Nguy";
  if (tyLeVang > 0.5)
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

  bool thoat = false;

  while (!thoat) {
    auto screen = ScreenInteractive::Fullscreen();
    int luaChon = -1;
    std::string thongBao;

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
                auto dsLopTK = app.getTKManager().thongKeTatCaLop();
                auto lopMax = app.getTKManager().lopVangCaoNhat();

                Elements rows;
                rows.push_back(
                    hbox({
                        text(" Mã LHP   ") | bold | size(WIDTH, EQUAL, 12),
                        text(" Tên LHP                     ") | bold |
                            size(WIDTH, EQUAL, 30),
                        text(" SV  ") | bold | size(WIDTH, EQUAL, 6),
                        text(" Buổi") | bold | size(WIDTH, EQUAL, 6),
                        text(" % Vắng  ") | bold | size(WIDTH, EQUAL, 9),
                        text(" SV CT") | bold | size(WIDTH, EQUAL, 7),
                    }) |
                    inverted);
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
                    row = row | color(Color::Yellow);
                  rows.push_back(row);
                }

                std::string maxInfo =
                    lopMax.has_value()
                        ? lopMax->maLHP + " - " + lopMax->tenLHP + " (" +
                              std::to_string(
                                  (int)(lopMax->tyLeVangTrungBinh * 100)) +
                              "%)"
                        : "(Chưa có dữ liệu)";

                content =
                    vbox({hbox({text(" Lớp vắng cao nhất: ") | dim,
                                text(maxInfo) | bold | color(Color::Red)}),
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
                    else if (sv.tyLeVang > 0.8)
                      ++svNguy;
                    else if (sv.tyLeVang > 0.5)
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

                  content = vbox(
                      {text(" THÔNG TIN LỚP HỌC PHẦN ") | bold | inverted |
                           center,
                       hbox({text(" Giảng viên : ") | dim,
                             text(tenGVLop) | bold}),
                       hbox({text(" Phòng học  : ") | dim,
                             text(tenPhong) | bold}),
                       hbox({text(" Học kỳ     : ") | dim,
                             text(lhp.getHocKiStr())}),
                       hbox({text(" Ngưỡng CT  : ") | dim,
                             text(std::to_string(nguongPct) + "% (tối đa " +
                                  std::to_string(maxVang) + " tiết)") |
                                 bold}),
                       hbox(
                           {text(" Tiến độ    : ") | dim,
                            text(std::to_string(lhp.getSoTietDaHoc()) + "/" +
                                 std::to_string(lhp.getTongSoTiet()) +
                                 " tiết (" + std::to_string(pctTienDo) + "%)") |
                                bold}),
                       separator(),
                       text(" THỐNG KÊ ") | bold | inverted | center,
                       hbox({text(" Số SV      : ") | dim,
                             text(std::to_string(soSVTotal)) | bold}),
                       hbox({text(" % Vắng TB  : ") | dim,
                             text(std::to_string((int)(tyLeVangTB * 100)) +
                                  "%") |
                                 bold}),
                       hbox({text(" SV cấm thi : ") | dim,
                             text(std::to_string(soSVBiCamThi)) | bold |
                                 color(Color::Red)}),
                       hbox({text(" SV nguy    : ") | dim,
                             text(std::to_string(svNguy)) | bold |
                                 color(Color::RedLight)}),
                       hbox({text(" SV cần chú : ") | dim,
                             text(std::to_string(svChuY)) | bold |
                                 color(Color::Yellow)}),
                       separator(),
                       text(" TOP 5 VẮNG NHIỀU (tiết vắng / % / còn được "
                            "vắng): ") |
                           bold,
                       vbox(std::move(topRows)),
                       separator(),
                       text(" VẮNG THEO CA: ") | bold,
                       vbox(std::move(caRows)),
                       filler()});
                }
              }

              // --- Tab 2: Danh sach SV ---
              else if (activeTab == 2) {
                if (!dsLop.empty() && selLop < (int)dsLop.size()) {
                  const std::string &maLHP = dsLop[selLop].getMaLHP();
                  auto dsSV = app.getTKManager().thongKeToanLop(maLHP);
                  const auto &lhp2 = app.getTKManager().getLHPRef(maLHP);
                  int maxVang2 =
                      (int)(lhp2.getTongSoTiet() * lhp2.getNguongCamThi());

                  int nCT = 0, nNguy = 0, nChuY = 0;
                  for (const auto &sv : dsSV) {
                    if (sv.biCamThi)
                      ++nCT;
                    else if (sv.tyLeVang > 0.8)
                      ++nNguy;
                    else if (sv.tyLeVang > 0.5)
                      ++nChuY;
                  }

                  Elements svRows;
                  // Dong tom tat
                  int nOk = (int)dsSV.size() - nCT - nNguy - nChuY;
                  svRows.push_back(hbox({
                      text(" Tổng: " + std::to_string(dsSV.size()) + " SV  ") |
                          dim,
                      text("[CT] " + std::to_string(nCT) + "  ") |
                          color(Color::Red),
                      text("[!!] " + std::to_string(nNguy) + "  ") |
                          color(Color::RedLight),
                      text("[!] " + std::to_string(nChuY) + "  ") |
                          color(Color::Yellow),
                      text("[OK] " + std::to_string(nOk)) | color(Color::Green),
                      text("  | Tối đa vắng: " + std::to_string(maxVang2) +
                           " tiết") |
                          dim,
                  }));
                  svRows.push_back(separator());
                  svRows.push_back(
                      hbox({
                          text(" # ") | size(WIDTH, EQUAL, 4),
                          text(" Mã SV    ") | bold | size(WIDTH, EQUAL, 11),
                          text(" Họ Tên                    ") | bold |
                              size(WIDTH, EQUAL, 28),
                          text(" Vắng") | bold | size(WIDTH, EQUAL, 6),
                          text(" Muộn") | bold | size(WIDTH, EQUAL, 6),
                          text(" % Vắng") | bold | size(WIDTH, EQUAL, 8),
                          text(" Còn vắng") | bold | size(WIDTH, EQUAL, 10),
                          text(" Trạng thái") | bold,
                      }) |
                      inverted);
                  svRows.push_back(separator());
                  for (int i = 0; i < (int)dsSV.size(); ++i) {
                    const auto &sv = dsSV[i];
                    int pct = (int)(sv.tyLeVang * 100);
                    int con = maxVang2 - sv.soTietVang;
                    std::string conStr =
                        con > 0 ? std::to_string(con) + "t" : "VƯỢT";
                    std::string tt = trangThaiSV(sv.tyLeVang, sv.biCamThi);
                    Color col = sv.biCamThi         ? Color::Red
                                : sv.tyLeVang > 0.8 ? Color::RedLight
                                : sv.tyLeVang > 0.5 ? Color::Yellow
                                                    : Color::White;
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

                  content = vbox(std::move(svRows)) | frame | flex;
                }
              }

              // --- Tab 3: Thong ke buoi ---
              else if (activeTab == 3) {
                if (!dsLop.empty() && selLop < (int)dsLop.size()) {
                  const std::string &maLHP = dsLop[selLop].getMaLHP();
                  auto dsBuoi = app.getTKManager().thongKeTatCaBuoi(maLHP);
                  auto maxB = app.getTKManager().buoiVangCaoNhat(maLHP);
                  std::size_t maxIdx =
                      maxB.has_value() ? maxB->buoiIndex : SIZE_MAX;

                  Elements bRows;
                  bRows.push_back(
                      hbox({
                          text(" Buổi") | bold | size(WIDTH, EQUAL, 6),
                          text(" Ngày          ") | bold |
                              size(WIDTH, EQUAL, 15),
                          text(" Ca  ") | bold | size(WIDTH, EQUAL, 6),
                          text(" Tiết") | bold | size(WIDTH, EQUAL, 6),
                          text("  CM ") | bold | size(WIDTH, EQUAL, 6),
                          text(" Vắng") | bold | size(WIDTH, EQUAL, 6),
                          text(" Muộn") | bold | size(WIDTH, EQUAL, 6),
                          text(" %CM    ") | bold | size(WIDTH, EQUAL, 9),
                      }) |
                      inverted);
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

                  content = vbox(std::move(bRows)) | frame | flex;
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
            activeTab = 0;
            return true;
          }
          if (e == Event::Character('1') && !isAdmin) {
            activeTab = 1;
            return true;
          } // GV nhan [1] -> chi tiet lop
          if (e == Event::Character('2')) {
            activeTab = 1;
            return true;
          }
          if (e == Event::Character('3')) {
            activeTab = 2;
            return true;
          }
          if (e == Event::Character('4')) {
            activeTab = 3;
            return true;
          }
          if (e == Event::Character('5')) {
            activeTab = 4;
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
          return false;
        });

    screen.Loop(renderer);
    if (luaChon == 99)
      thoat = true;
    // luaChon == 5 → loop lai (refresh sau khi xuat)
  }
}
