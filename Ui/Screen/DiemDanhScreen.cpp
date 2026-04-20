#include <BuoiDiemDanhForm.hpp>
#include <DiemDanhScreen.hpp>
#include <SinhVienHistoryScreen.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

// Form nhap/sua ghi chu cho 1 SV trong 1 buoi
static void formGhiChu(AppManager &app, const std::string &maLHP,
                       std::size_t buoiIndex, const std::string &maSV,
                       const std::string &tenSV) {
  auto screen = ScreenInteractive::Fullscreen();
  std::string ghiChu;

  // Pre-fill voi ghi chu hien tai (neu co)
  const auto &lhp = app.getLHPManager().getLopRef(maLHP);
  if (buoiIndex < lhp.getDsBuoiDiemDanh().size()) {
    if (const auto *ct = lhp.getDsBuoiDiemDanh()[buoiIndex].findChiTiet(maSV))
      ghiChu = ct->getGhiChu();
  }

  std::string thongBao;
  InputOption opt;
  opt.multiline = false;
  auto input = Input(&ghiChu, "Nhap ghi chu...", opt);

  auto btnLuu = Button("Luu [Enter]", [&] {
    try {
      app.getDDManager().capNhatGhiChu(maLHP, buoiIndex, maSV, ghiChu);
      screen.Exit();
    } catch (const std::exception &e) {
      thongBao = std::string("[ERR] ") + e.what();
    }
  });

  auto btnHuy = Button("Huy [Esc]", [&] { screen.Exit(); });
  auto layout = Container::Vertical(
      {input, Container::Horizontal({btnLuu, btnHuy})});

  auto renderer = Renderer(layout, [&] {
    return vbox({
        filler(),
        vbox({
            text(" GHI CHU - " + maSV +
                 (tenSV.empty() ? "" : " / " + tenSV)) | bold | center,
            separator(),
            hbox({text(" Ghi chu: ") | dim, input->Render() | flex}),
            separator(),
            hbox({btnLuu->Render(), text("  "), btnHuy->Render()}) | center,
            UiHelper::makeMessage(thongBao),
        }) | border | size(WIDTH, EQUAL, 50) | center,
        filler(),
        UiHelper::makeFooter("[Enter] Luu  [Esc] Huy"),
    });
  }) | CatchEvent([&](Event e) {
    if (e == Event::Escape) { screen.Exit(); return true; }
    if (e == Event::Return && !ghiChu.empty()) {
      btnLuu->OnEvent(Event::Return); return true;
    }
    return false;
  });

  screen.Loop(renderer);
}

static void screenDiemDanhBuoi(AppManager &app, const std::string &maLHP,
                               std::size_t buoiIndex, bool isAdmin) {
  bool thoat = false;
  int selected = 0;
  std::string thongBao;

  auto dsMaSV = app.getDKManager().getDsMaSVTheoLop(maLHP);
  struct SVInfo {
    std::string ten, lopSH;
  };
  std::unordered_map<std::string, SVInfo> svMap;

  for (const auto &sv : app.getSVManager().getAll())
    svMap[sv.getMaSV()] = {sv.getTenSV(), sv.getLopSHStr()};

  auto statusStr = [](Status s) -> std::string {
    switch (s) {
    case Status::CO_MAT:
      return "[●] Có mặt";
    case Status::VANG:
      return "[ ] Vắng   ";
    case Status::MUON:
      return "[~] Muộn   ";
    default:
      return "[-] Chưa ĐD";
    }
  };

  auto statusColor = [](Status s) -> Color {
    switch (s) {
    case Status::CO_MAT:
      return Color::Green;
    case Status::VANG:
      return Color::Red;
    case Status::MUON:
      return Color::Yellow;
    default:
      return Color::GrayDark;
    }
  };

  while (!thoat) {
    auto screen = ScreenInteractive::Fullscreen();
    int luaChon = -1;

    const auto &lhp = app.getLHPManager().getLopRef(maLHP);
    if (buoiIndex >= lhp.getDsBuoiDiemDanh().size()) {
      thoat = true;
      break;
    }
    const auto &buoi = lhp.getDsBuoiDiemDanh()[buoiIndex];
    bool khoa = buoi.isKhoaDiemDanh();

    // --- Maps trạng thái và có phép ---
    std::unordered_map<std::string, Status> trangThaiMap;
    std::unordered_map<std::string, bool>   coPhepMap;
    for (const auto &ct : buoi.getDanhSachChiTiet()) {
      trangThaiMap[ct.getMaSV()] = ct.getTrangThai();
      coPhepMap[ct.getMaSV()]    = ct.isCoPhep();
    }

    // --- Feature 2: cảnh báo ngưỡng vắng ---
    std::unordered_map<std::string, std::string> warnSuffix;
    auto updateWarn = [&](const std::string &maSV) {
      auto ng = app.getDDManager().kiemTraNguong(maLHP, maSV);
      if (ng.biCamThi)          warnSuffix[maSV] = " [CT!]";
      else if (ng.daVuotNguong) warnSuffix[maSV] = " [!!] ";
      else if (ng.sapVuotNguong)warnSuffix[maSV] = " [!]  ";
      else                      warnSuffix[maSV] = "";
    };
    for (const auto &maSV : dsMaSV) updateWarn(maSV);

    // --- buildEntry: tạo chuỗi hiển thị cho 1 SV ---
    auto buildEntry = [&](int i) -> std::string {
      const auto &maSV = dsMaSV[i];
      auto itSV = svMap.find(maSV);
      std::string ten = (itSV != svMap.end()) ? itSV->second.ten : "(?)";
      auto it = trangThaiMap.find(maSV);
      Status st = (it != trangThaiMap.end()) ? it->second : Status::DEFAULT;
      bool cp = coPhepMap.count(maSV) ? coPhepMap.at(maSV) : false;
      std::string sPart;
      if      (st == Status::CO_MAT) sPart = "[*] Co mat";
      else if (st == Status::VANG)   sPart = cp ? "[P] V.Phep " : "[ ] Vang   ";
      else if (st == Status::MUON)   sPart = "[~] Muon   ";
      else                           sPart = "[-] Chua DD";
      return sPart + "  " + maSV + "  " + ten + warnSuffix[maSV];
    };

    std::vector<std::string> entries;
    for (int i = 0; i < static_cast<int>(dsMaSV.size()); ++i)
      entries.push_back(buildEntry(i));
    if (entries.empty()) entries.push_back("(Chua co sinh vien dang ky)");

    if (!dsMaSV.empty())
      selected = std::min(selected, static_cast<int>(dsMaSV.size()) - 1);

    int soCoMat = 0, soVang = 0, soMuon = 0, soChuaDD = 0;
    auto recalcStats = [&]() {
      soCoMat = soVang = soMuon = soChuaDD = 0;
      for (const auto &sv : dsMaSV) {
        auto it = trangThaiMap.find(sv);
        if (it == trangThaiMap.end() || it->second == Status::DEFAULT) ++soChuaDD;
        else if (it->second == Status::CO_MAT) ++soCoMat;
        else if (it->second == Status::VANG)   ++soVang;
        else if (it->second == Status::MUON)   ++soMuon;
      }
    };
    recalcStats();

    // Feature 4: Quick Mark
    std::string quickInput;
    auto inputMaSV = Input(&quickInput, "Ma SV...", InputOption::Default());

    auto menuSV = Menu(&entries, &selected);

    // Helper cập nhật entry + warn sau khi đổi trạng thái
    auto refreshEntry = [&](const std::string &maSV, int idx) {
      updateWarn(maSV);
      entries[idx] = buildEntry(idx);
    };

    auto btnCoMat = Button("Co mat [1]", [&] {
      if (khoa) { thongBao = "[ERR] Buổi đã khóa!"; return; }
      if (dsMaSV.empty() || selected >= static_cast<int>(dsMaSV.size())) return;
      const std::string &maSV = dsMaSV[selected];
      try {
        if (trangThaiMap.count(maSV))
          app.getDDManager().capNhatTrangThai(maLHP, buoiIndex, maSV, Status::CO_MAT);
        else
          app.getDDManager().diemDanh(maLHP, buoiIndex, maSV, DateTime(), Status::CO_MAT, "");
        trangThaiMap[maSV] = Status::CO_MAT;
        coPhepMap[maSV] = false;
        refreshEntry(maSV, selected);
        recalcStats();
        thongBao = "[OK] Co mat: " + maSV;
      } catch (const std::exception &e) {
        thongBao = "[ERR] " + std::string(e.what());
      }
    });

    auto btnVang = Button("Vang   [2]", [&] {
      if (khoa) { thongBao = "[ERR] Buổi đã khóa!"; return; }
      if (dsMaSV.empty() || selected >= static_cast<int>(dsMaSV.size())) return;
      const std::string &maSV = dsMaSV[selected];
      try {
        if (trangThaiMap.count(maSV))
          app.getDDManager().capNhatTrangThai(maLHP, buoiIndex, maSV, Status::VANG);
        else
          app.getDDManager().diemDanh(maLHP, buoiIndex, maSV, DateTime(), Status::VANG, "");
        trangThaiMap[maSV] = Status::VANG;
        coPhepMap[maSV] = false; // mặc định không phép, dùng [P] để toggle
        refreshEntry(maSV, selected);
        recalcStats();
        thongBao = "[OK] Vang: " + maSV + "  ([P] = Vang co phep)";
      } catch (const std::exception &e) {
        thongBao = "[ERR] " + std::string(e.what());
      }
    });

    auto btnMuon = Button("Muon   [3]", [&] {
      if (khoa) { thongBao = "[ERR] Buổi đã khóa!"; return; }
      if (dsMaSV.empty() || selected >= static_cast<int>(dsMaSV.size())) return;
      const std::string &maSV = dsMaSV[selected];
      try {
        if (trangThaiMap.count(maSV))
          app.getDDManager().capNhatTrangThai(maLHP, buoiIndex, maSV, Status::MUON);
        else
          app.getDDManager().diemDanh(maLHP, buoiIndex, maSV, DateTime(), Status::MUON, "");
        trangThaiMap[maSV] = Status::MUON;
        coPhepMap[maSV] = false;
        refreshEntry(maSV, selected);
        recalcStats();
        thongBao = "[OK] Muon: " + maSV;
      } catch (const std::exception &e) {
        thongBao = "[ERR] " + std::string(e.what());
      }
    });

    auto btnTatCaCoMat = Button("Tat ca co mat [A]", [&] {
      if (khoa) { thongBao = "[ERR] Buổi đã khóa!"; return; }
      bool coLoi = false;
      for (int i = 0; i < static_cast<int>(dsMaSV.size()); ++i) {
        const std::string &maSV = dsMaSV[i];
        try {
          if (trangThaiMap.count(maSV))
            app.getDDManager().capNhatTrangThai(maLHP, buoiIndex, maSV, Status::CO_MAT);
          else
            app.getDDManager().diemDanh(maLHP, buoiIndex, maSV, DateTime(), Status::CO_MAT, "");
          trangThaiMap[maSV] = Status::CO_MAT;
          coPhepMap[maSV] = false;
          refreshEntry(maSV, i);
        } catch (const std::exception &e) {
          thongBao = "[ERR] " + std::string(e.what());
          coLoi = true; break;
        }
      }
      if (!coLoi) { recalcStats(); thongBao = "[OK] Da diem danh tat ca co mat"; }
    });

    // Feature 5: confirm khóa
    bool showConfirmKhoa = false;
    int cfCoMat = 0, cfVang = 0, cfMuon = 0, cfChuaDD = 0, cfAtRisk = 0;

    auto btnKhoa = Button("Khoa buoi [K]", [&] {
      if (khoa) { thongBao = "[ERR] Buổi đã khóa!"; return; }
      cfCoMat = soCoMat; cfVang = soVang; cfMuon = soMuon; cfChuaDD = soChuaDD;
      cfAtRisk = 0;
      for (const auto &maSV : dsMaSV) {
        auto ng = app.getDDManager().kiemTraNguong(maLHP, maSV);
        if (ng.sapVuotNguong || ng.daVuotNguong) ++cfAtRisk;
      }
      showConfirmKhoa = true;
    });

    auto btnQuayLai = Button("Quay lại [Q]", [&] {
      luaChon = 99;
      screen.Exit();
    });

    auto buildHeaderSub = [&]() -> std::string {
      return maLHP + "  |  Buổi " + std::to_string(buoiIndex + 1) + "  |  " +
             buoi.getNgayDiemDanhStr() + "  " + buoi.getCaDiemDanhStr() +
             (khoa ? "  [ĐÃ KHÓA]" : "");
    };
    std::string headerSub = buildHeaderSub();

    auto btnMoKhoa = Button("Mở khóa [M]", [&] {
      if (!khoa) {
        thongBao = "[ERR] Buổi chưa bị khóa!";
        return;
      }
      try {
        app.getDDManager().moKhoaBuoi(maLHP, buoiIndex);
        khoa = false;
        headerSub = buildHeaderSub();
        thongBao = "[OK] Đã mở khóa buổi";
      } catch (const std::exception &e) {
        thongBao = "[ERR] " + std::string(e.what());
      }
    });

    auto btnXoaBuoi = Button("Xóa buổi [X]", [&] {
      try {
        app.getDDManager().xoaBuoi(maLHP, buoiIndex);
        thongBao = "[OK] Đã xóa buổi";
        luaChon = 99;
        screen.Exit();
      } catch (const std::exception &e) {
        thongBao = "[ERR] " + std::string(e.what());
      }
    });

    Component layout;
    if (isAdmin) {
      layout = Container::Vertical(
          {menuSV, Container::Horizontal(
                       {btnMoKhoa, btnXoaBuoi, btnKhoa, btnQuayLai})});
    } else {
      // Feature 4: inputMaSV đặt trước menuSV trong layout
      layout = Container::Vertical(
          {inputMaSV, menuSV,
           Container::Horizontal({btnCoMat, btnVang, btnMuon}),
           Container::Horizontal({btnTatCaCoMat, btnKhoa, btnQuayLai})});
    }

    auto renderer =
        Renderer(
            layout,
            [&] {
              int total = static_cast<int>(dsMaSV.size());
              auto mainContent = vbox({
                  UiHelper::makeHeader("DIEM DANH", headerSub), separator(),
                  hbox({
                      text("  Co mat: ") | dim,
                      text(std::to_string(soCoMat)) | color(Color::Green) | bold,
                      text("  Vang: ") | dim,
                      text(std::to_string(soVang)) | color(Color::Red) | bold,
                      text("  Muon: ") | dim,
                      text(std::to_string(soMuon)) | color(Color::Yellow) | bold,
                      text("  Chua DD: ") | dim,
                      text(std::to_string(soChuaDD)) | color(Color::GrayDark) | bold,
                      text("  /" + std::to_string(total)) | dim,
                  }) | center,
                  separator(),
                  // Feature 4: quick mark input (chỉ GV)
                  !isAdmin ? hbox({text(" Tim nhanh: ") | dim,
                                   inputMaSV->Render() | size(WIDTH, EQUAL, 16),
                                   text(" [Enter]=Co mat") | dim, filler()})
                           : text(""),
                  vbox({menuSV->Render() | flex}) | border | flex,
                  separator(),
                  khoa ? text(" Buoi nay da bi khoa ") | color(Color::Red) | center
                       : [&]() -> Element {
                           if (isAdmin) {
                             return vbox({text(" CHE DO: XEM & QUAN LY ") | bold |
                                              color(Color::Yellow) | center,
                                          separator(),
                                          hbox({khoa ? btnMoKhoa->Render()
                                                     : text(" [Chua khoa] ") | dim,
                                                text(" "), btnXoaBuoi->Render(),
                                                text(" "),
                                                !khoa ? btnKhoa->Render()
                                                      : text(" [Da khoa] ") | dim,
                                                text(" "), btnQuayLai->Render()}) |
                                              center});
                           } else {
                             return vbox({hbox({btnCoMat->Render(), text(" "),
                                               btnVang->Render(), text(" "),
                                               btnMuon->Render()}) | center,
                                          hbox({btnTatCaCoMat->Render(), text(" "),
                                               btnKhoa->Render(), text(" "),
                                               btnQuayLai->Render()}) | center});
                           }
                         }(),
                  separator(), UiHelper::makeMessage(thongBao),
                  UiHelper::makeFooter(
                      isAdmin ? (khoa ? "[M] Mo khoa  [X] Xoa buoi  [Q] Quay lai"
                                      : "[K] Khoa  [X] Xoa buoi  [Q] Quay lai")
                              : (khoa ? "[Q] Quay lai"
                                      : "[1] CM  [2] Vang  [3] Muon "
                                        "[P] T.Phep  [A] Tat ca "
                                        "[K] Khoa  [H] Lich su  [Q] Quay")),
              });

              // Feature 5: overlay confirm khóa
              if (showConfirmKhoa) {
                auto dialog = vbox({
                    text(" XAC NHAN KHOA BUOI? ") | bold | center,
                    separator(),
                    hbox({text("  Co mat : ") | dim,
                          text(std::to_string(cfCoMat) + "/" +
                               std::to_string(total)) | bold | color(Color::Green)}),
                    hbox({text("  Vang   : ") | dim,
                          text(std::to_string(cfVang)) | bold | color(Color::Red)}),
                    hbox({text("  Muon   : ") | dim,
                          text(std::to_string(cfMuon)) | bold | color(Color::Yellow)}),
                    hbox({text("  Chua DD: ") | dim,
                          text(std::to_string(cfChuaDD)) | bold}),
                    separator(),
                    cfAtRisk > 0
                        ? (hbox({text("  [!] ") | color(Color::Red),
                                  text(std::to_string(cfAtRisk) +
                                       " SV sap vuot nguong cam thi") | dim}))
                        : text(""),
                    text("  Se tu vang: " + std::to_string(cfChuaDD) +
                         " SV chua diem danh") | dim,
                    separator(),
                    text(" [Y] Xac nhan    [N/Esc] Huy ") | center | bold,
                }) | border | size(WIDTH, EQUAL, 46) | center;
                return dbox({mainContent | dim,
                             vbox({filler(), dialog, filler()})});
              }
              return mainContent;
            }) |
        CatchEvent([&](Event e) {
          // Feature 5: chặn tất cả sự kiện khi đang hiển thị confirm
          if (showConfirmKhoa) {
            if (e == Event::Character('y') || e == Event::Character('Y')) {
              try {
                app.getDDManager().khoaBuoiVaAutoVang(maLHP, buoiIndex, dsMaSV);
                thongBao = "[OK] Da khoa buoi diem danh";
                showConfirmKhoa = false;
                luaChon = 0;
                screen.Exit();
              } catch (const std::exception &ex) {
                thongBao = "[ERR] " + std::string(ex.what());
                showConfirmKhoa = false;
              }
            } else {
              showConfirmKhoa = false; // N, Esc, hoặc bất kỳ phím nào khác
            }
            return true; // chặn tất cả
          }

          // Feature 4: quick mark khi có nội dung trong input
          if (e == Event::Return && !quickInput.empty() && !isAdmin && !khoa) {
            auto it = std::find(dsMaSV.begin(), dsMaSV.end(), quickInput);
            if (it != dsMaSV.end()) {
              int idx = static_cast<int>(it - dsMaSV.begin());
              selected = idx;
              const auto &maSV = *it;
              try {
                if (trangThaiMap.count(maSV))
                  app.getDDManager().capNhatTrangThai(maLHP, buoiIndex, maSV, Status::CO_MAT);
                else
                  app.getDDManager().diemDanh(maLHP, buoiIndex, maSV, DateTime(), Status::CO_MAT, "");
                trangThaiMap[maSV] = Status::CO_MAT;
                coPhepMap[maSV] = false;
                refreshEntry(maSV, idx);
                recalcStats();
                thongBao = "[OK] Co mat: " + maSV;
              } catch (const std::exception &ex) {
                thongBao = "[ERR] " + std::string(ex.what());
              }
              quickInput.clear();
            } else {
              thongBao = "[!] Khong tim thay ma: " + quickInput;
              quickInput.clear();
            }
            return true;
          }

          if (isAdmin) {
            if (e == Event::Character('m') || e == Event::Character('M')) {
              btnMoKhoa->OnEvent(Event::Return); return true;
            }
            if (e == Event::Character('x') || e == Event::Character('X')) {
              btnXoaBuoi->OnEvent(Event::Return); return true;
            }
            if (e == Event::Character('k') || e == Event::Character('K')) {
              btnKhoa->OnEvent(Event::Return); return true;
            }
          } else if (!khoa) {
            if (e == Event::Character('1')) { btnCoMat->OnEvent(Event::Return); return true; }
            if (e == Event::Character('2')) { btnVang->OnEvent(Event::Return); return true; }
            if (e == Event::Character('3')) { btnMuon->OnEvent(Event::Return); return true; }
            if (e == Event::Character('a') || e == Event::Character('A')) {
              btnTatCaCoMat->OnEvent(Event::Return); return true;
            }
            if (e == Event::Character('k') || e == Event::Character('K')) {
              btnKhoa->OnEvent(Event::Return); return true;
            }
            // Feature 1: toggle có phép cho SV đang vắng
            if (e == Event::Character('p') || e == Event::Character('P')) {
              if (!dsMaSV.empty() && selected < static_cast<int>(dsMaSV.size())) {
                const auto &maSV = dsMaSV[selected];
                auto it = trangThaiMap.find(maSV);
                if (it != trangThaiMap.end() && it->second == Status::VANG) {
                  bool newCP = !coPhepMap[maSV];
                  try {
                    app.getDDManager().capNhatCoPhep(maLHP, buoiIndex, maSV, newCP);
                    coPhepMap[maSV] = newCP;
                    entries[selected] = buildEntry(selected);
                    thongBao = newCP ? "[OK] Vang co phep: " + maSV
                                     : "[OK] Vang khong phep: " + maSV;
                  } catch (const std::exception &ex) {
                    thongBao = "[ERR] " + std::string(ex.what());
                  }
                } else {
                  thongBao = "[!] Chi toggle phep cho SV dang vang";
                }
              }
              return true;
            }
            // Feature 6: xem lich su
            if (e == Event::Character('h') || e == Event::Character('H')) {
              if (!dsMaSV.empty() && selected < static_cast<int>(dsMaSV.size())) {
                luaChon = 10;
                screen.Exit();
              }
              return true;
            }
            // Ghi chu: [G] mo form nhap ghi chu cho SV dang chon
            if (e == Event::Character('g') || e == Event::Character('G')) {
              if (!dsMaSV.empty() && selected < static_cast<int>(dsMaSV.size())) {
                const auto &maSV = dsMaSV[selected];
                // Chi cho phep ghi chu khi SV da duoc diem danh
                if (trangThaiMap.count(maSV)) {
                  luaChon = 12;
                  screen.Exit();
                } else {
                  thongBao = "[!] Can diem danh SV truoc khi ghi chu";
                }
              }
              return true;
            }
          }
          if (e == Event::Character('q') || e == Event::Character('Q') ||
              e == Event::Escape) {
            luaChon = 99; screen.Exit(); return true;
          }
          return false;
        });

    screen.Loop(renderer);
    if (luaChon == 99)
      thoat = true;
    else if (luaChon == 10 && !dsMaSV.empty() &&
             selected < static_cast<int>(dsMaSV.size())) {
      screenSinhVienHistory(app, maLHP, dsMaSV[selected]);
    } else if (luaChon == 12 && !dsMaSV.empty() &&
               selected < static_cast<int>(dsMaSV.size())) {
      const auto &maSV = dsMaSV[selected];
      std::string tenSV;
      auto it = svMap.find(maSV);
      if (it != svMap.end()) tenSV = it->second.ten;
      formGhiChu(app, maLHP, buoiIndex, maSV, tenSV);
      // loop tiep tuc (khong set thoat)
    }
  }
}

void screenDiemDanh(AppManager &app, const std::string &maGV) {
  bool thoat = false;
  int selLop = 0;
  int selBuoi = 0;

  auto gvOpt = app.getGVManager().timTheoMa(maGV);
  bool isAdmin = gvOpt.has_value() && gvOpt->isAdmin();

  while (!thoat) {
    auto screen = ScreenInteractive::Fullscreen();
    int luaChon = -1;

    const std::vector<LopHocPhan> &dsLHP =
        isAdmin ? app.getLHPManager().getAll()
                : app.getLHPManager().getLopTheoGV(maGV);

    if (!dsLHP.empty())
      selLop = std::min(selLop, static_cast<int>(dsLHP.size()) - 1);

    std::vector<std::string> lopEntries;
    for (const auto &lhp : dsLHP)
      lopEntries.push_back(lhp.getMaLHP() + "  " + lhp.getTenLHP());
    if (lopEntries.empty())
      lopEntries.push_back("(Chưa có lớp)");

    auto menuLop = Menu(&lopEntries, &selLop);

    // buoiEntries được cập nhật động trong renderer khi selLop thay đổi
    std::vector<std::string> buoiEntries;
    auto menuBuoi = Menu(&buoiEntries, &selBuoi);

    auto btnTaoBuoi = Button("Tạo buổi mới [N]", [&] {
      if (!dsLHP.empty() && selLop < static_cast<int>(dsLHP.size())) {
        formTaoBuoiDiemDanh(app, dsLHP[selLop].getMaLHP());
        luaChon = 0;
        screen.Exit();
      }
    });

    auto btnDiemDanh = Button("Điểm danh [Enter]", [&] {
      if (!dsLHP.empty() && selLop < static_cast<int>(dsLHP.size())) {
        const auto &buois = dsLHP[selLop].getDsBuoiDiemDanh();
        if (!buois.empty() && selBuoi < static_cast<int>(buois.size())) {
          luaChon = 1;
          screen.Exit();
        } else {
          // Chưa có buổi → hỏi tạo mới
          luaChon = 2;
          screen.Exit();
        }
      }
    });

    auto btnQuayLai = Button("Quay lại [Q]", [&] {
      luaChon = 99;
      screen.Exit();
    });

    auto layout = Container::Horizontal(
        {menuLop, Container::Vertical(
                      {menuBuoi, Container::Horizontal(
                                     {btnTaoBuoi, btnDiemDanh, btnQuayLai})})});

    int cachedSelLop = -1;
    int cachedSoSV = 0;
    int cachedSoBuoi = 0;

    auto renderer =
        Renderer(
            layout,
            [&] {
              if (selLop != cachedSelLop) {
                cachedSelLop = selLop;
                buoiEntries.clear();
                selBuoi = 0; // reset về buổi đầu khi đổi lớp
                if (!dsLHP.empty() && selLop < static_cast<int>(dsLHP.size())) {
                  const auto &lhp = dsLHP[selLop];
                  cachedSoSV =
                      static_cast<int>(app.getDKManager()
                                           .getDsMaSVTheoLop(lhp.getMaLHP())
                                           .size());
                  cachedSoBuoi =
                      static_cast<int>(lhp.getDsBuoiDiemDanh().size());
                  for (std::size_t i = 0; i < lhp.getDsBuoiDiemDanh().size();
                       ++i) {
                    const auto &b = lhp.getDsBuoiDiemDanh()[i];
                    buoiEntries.push_back(
                        "Buổi " + std::to_string(i + 1) + "  " +
                        b.getNgayDiemDanhStr() + "  " + b.getCaDiemDanhStr() +
                        "  " + std::to_string(b.getSoTiet()) + " tiết" +
                        (b.isKhoaDiemDanh() ? "  [KHÓA]" : ""));
                  }
                  if (buoiEntries.empty())
                    buoiEntries.push_back("(Chưa có buổi điểm danh)");
                }
              }

              return vbox(
                  {UiHelper::makeHeader("ĐIỂM DANH",
                                        isAdmin ? "Admin" : "GV: " + maGV),
                   separator(),
                   hbox({// Cột trái: danh sách lớp
                         vbox({text(" CHỌN LỚP ") | bold | center, separator(),
                               menuLop->Render() | flex}) |
                             border | size(WIDTH, EQUAL, 30),

                         // Cột giữa: danh sách buổi
                         vbox({hbox({text(" BUỔI ĐIỂM DANH (") | dim,
                                     text(std::to_string(cachedSoBuoi)) | bold,
                                     text(") ") | dim,
                                     text("  SV: " +
                                          std::to_string(cachedSoSV)) |
                                         dim}) |
                                   center,
                               separator(), menuBuoi->Render() | flex,
                               separator(),
                               hbox({btnTaoBuoi->Render(), text(" "),
                                     btnDiemDanh->Render(), text(" "),
                                     btnQuayLai->Render()}) |
                                   center}) |
                             border | flex}) |
                       flex,
                   separator(),
                   UiHelper::makeFooter("[↑↓] Chọn lớp/buổi  [N] Tạo buổi  "
                                        "[Enter] Điểm danh  [Q] Quay lại")});
            }) |
        CatchEvent([&](Event e) {
          if (e == Event::Character('n') || e == Event::Character('N')) {
            btnTaoBuoi->OnEvent(Event::Return);
            return true;
          }
          if (e == Event::Return) {
            btnDiemDanh->OnEvent(Event::Return);
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
    if (!dsLHP.empty() && selLop < static_cast<int>(dsLHP.size())) {
      std::string maLHPChon = dsLHP[selLop].getMaLHP();
      switch (luaChon) {
      case 0:
        break;
      case 1:
        screenDiemDanhBuoi(app, maLHPChon, selBuoi, isAdmin);
        break;
      case 2:
        if (formTaoBuoiDiemDanh(app, maLHPChon))
          selBuoi = 0;
        break;
      case 99:
        thoat = true;
        break;
      }
    } else {
      if (luaChon == 99)
        thoat = true;
    }
  }
}