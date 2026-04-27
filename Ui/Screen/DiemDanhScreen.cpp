#include <BuoiDiemDanhForm.hpp>
#include <DiemDanhScreen.hpp>
#include <SinhVienHistoryScreen.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

static std::string screenNhapGhiChu(const std::string &maSV,
                                     const std::string &tenSV,
                                     const std::string &ghiChuHienTai) {
  auto screen = ScreenInteractive::Fullscreen();
  std::string ghiChu = ghiChuHienTai;
  bool confirmed = false;

  InputOption opt;
  opt.multiline = false;
  auto input = Input(&ghiChu, "Nhập ghi chú...", opt);
  auto btnLuu = Button("Lưu", [&] { confirmed = true; screen.Exit(); });
  auto btnHuy = Button("Hủy [Esc]", [&] { screen.Exit(); });
  auto layout = Container::Vertical(
      {input, Container::Horizontal({btnLuu, btnHuy})});

  auto renderer = Renderer(layout, [&] {
    return vbox({
        filler(),
        vbox({
            text(" GHI CHÚ — " + maSV +
                 (tenSV.empty() ? "" : " — " + tenSV)) |
                bold | center,
            separator(),
            hbox({text(" Ghi chú: ") | dim,
                  input->Render() | flex}),
            separator(),
            hbox({btnLuu->Render(), text("  "), btnHuy->Render()}) | center,
            text(" [Enter] Lưu   [Esc] Hủy ") | dim | center,
        }) | border | size(WIDTH, EQUAL, 52) | center,
        filler(),
    });
  }) | CatchEvent([&](Event e) {
    if (e == Event::Return) { confirmed = true; screen.Exit(); return true; }
    if (e == Event::Escape) { screen.Exit(); return true; }
    return false;
  });

  screen.Loop(renderer);
  return confirmed ? ghiChu : ghiChuHienTai;
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

    std::unordered_map<std::string, Status> trangThaiMap;
    std::unordered_map<std::string, bool>   coPhepMap;
    for (const auto &ct : buoi.getDanhSachChiTiet()) {
      trangThaiMap[ct.getMaSV()] = ct.getTrangThai();
      coPhepMap[ct.getMaSV()]    = ct.isCoPhep();
    }

    std::unordered_map<std::string, std::string> warnSuffix;
    auto updateWarn = [&](const std::string &maSV) {
      auto ng = app.getDDManager().kiemTraNguong(maLHP, maSV);
      if (ng.biCamThi)          warnSuffix[maSV] = " [CT!]";
      else if (ng.daVuotNguong) warnSuffix[maSV] = " [!!] ";
      else if (ng.sapVuotNguong)warnSuffix[maSV] = " [!]  ";
      else                      warnSuffix[maSV] = "";
    };
    for (const auto &maSV : dsMaSV) updateWarn(maSV);

    auto buildEntry = [&](int i) -> std::string {
      const auto &maSV = dsMaSV[i];
      auto itSV = svMap.find(maSV);
      std::string ten = (itSV != svMap.end()) ? itSV->second.ten : "(?)";
      auto it = trangThaiMap.find(maSV);
      Status st = (it != trangThaiMap.end()) ? it->second : Status::DEFAULT;
      bool cp = coPhepMap.count(maSV) ? coPhepMap.at(maSV) : false;
      std::string sPart;
      if      (st == Status::CO_MAT) sPart = "[*] Có mặt";
      else if (st == Status::VANG)   sPart = cp ? "[P] V.Phép " : "[ ] Vắng   ";
      else if (st == Status::MUON)   sPart = "[~] Muộn   ";
      else                           sPart = "[-] Chưa ĐD";
      return sPart + "  " + maSV + "  " + ten + warnSuffix[maSV];
    };

    std::vector<std::string> entries;
    for (int i = 0; i < static_cast<int>(dsMaSV.size()); ++i)
      entries.push_back(buildEntry(i));
    if (entries.empty()) entries.push_back("(Chưa có sinh viên đăng ký)");

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

    std::string quickInput;
    auto inputMaSV = Input(&quickInput, "Mã SV...", InputOption::Default());

    auto menuSV = Menu(&entries, &selected);

    auto refreshEntry = [&](const std::string &maSV, int idx) {
      updateWarn(maSV);
      entries[idx] = buildEntry(idx);
    };

    auto btnCoMat = Button("Có mặt [1]", [&] {
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
        thongBao = "[OK] Có mặt: " + maSV;
      } catch (const std::exception &e) {
        thongBao = "[ERR] " + std::string(e.what());
      }
    });

    auto btnVang = Button("Vắng   [2]", [&] {
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
        thongBao = "[OK] Vắng: " + maSV + "  ([P] = Vắng có phép)";
      } catch (const std::exception &e) {
        thongBao = "[ERR] " + std::string(e.what());
      }
    });

    auto btnMuon = Button("Muộn   [3]", [&] {
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
        thongBao = "[OK] Muộn: " + maSV;
      } catch (const std::exception &e) {
        thongBao = "[ERR] " + std::string(e.what());
      }
    });

    auto btnTatCaCoMat = Button("Tất cả có mặt [A]", [&] {
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
      if (!coLoi) { recalcStats(); thongBao = "[OK] Đã điểm danh tất cả có mặt"; }
    });

    bool showConfirmKhoa = false;
    int cfCoMat = 0, cfVang = 0, cfMuon = 0, cfChuaDD = 0, cfAtRisk = 0;

    auto btnKhoa = Button("Khóa buổi [K]", [&] {
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
                  UiHelper::makeHeader("ĐIỂM DANH", headerSub), separator(),
                  hbox({
                      text("  Có mặt: ") | dim,
                      text(std::to_string(soCoMat)) | color(Color::Green) | bold,
                      text("  Vắng: ") | dim,
                      text(std::to_string(soVang)) | color(Color::Red) | bold,
                      text("  Muộn: ") | dim,
                      text(std::to_string(soMuon)) | color(Color::Yellow) | bold,
                      text("  Chưa ĐD: ") | dim,
                      text(std::to_string(soChuaDD)) | color(Color::GrayDark) | bold,
                      text("  /" + std::to_string(total)) | dim,
                  }) | center,
                  separator(),
                  !isAdmin ? hbox({text(" Tìm nhanh: ") | dim,
                                   inputMaSV->Render() | size(WIDTH, EQUAL, 16),
                                   text(" [Enter]=Có mặt") | dim, filler()})
                           : text(""),
                  vbox({menuSV->Render() | flex}) | border | flex,
                  separator(),
                  khoa ? text(" Buổi này đã bị khóa ") | color(Color::Red) | center
                       : [&]() -> Element {
                           if (isAdmin) {
                             return vbox({text(" CHẾ ĐỘ: XEM & QUẢN LÝ ") | bold |
                                              color(Color::Yellow) | center,
                                          separator(),
                                          hbox({khoa ? btnMoKhoa->Render()
                                                     : text(" [Chưa khóa] ") | dim,
                                                text(" "), btnXoaBuoi->Render(),
                                                text(" "),
                                                !khoa ? btnKhoa->Render()
                                                      : text(" [Đã khóa] ") | dim,
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
                       isAdmin ? (khoa ? "[M] Mở khóa  [X] Xóa buổi  [Q] Quay lại"
                                       : "[K] Khóa  [X] Xóa buổi  [Q] Quay lại")
                               : (khoa ? "[Q] Quay lại"
                                       : "[1] CM  [2] Vắng  [3] Muộn  [P] T.Phép"
                                         "  [G] Ghi chú  [A] Tất cả"
                                         "  [K] Khóa  [H] Lịch sư  [Q] Quay lại")),
              });

              if (showConfirmKhoa) {
                auto dialog = vbox({
                    text(" XÁC NHẬN KHÓA BUỔI? ") | bold | center,
                    separator(),
                    hbox({text("  Có mặt : ") | dim,
                          text(std::to_string(cfCoMat) + "/" +
                               std::to_string(total)) | bold | color(Color::Green)}),
                    hbox({text("  Vắng   : ") | dim,
                          text(std::to_string(cfVang)) | bold | color(Color::Red)}),
                    hbox({text("  Muộn   : ") | dim,
                          text(std::to_string(cfMuon)) | bold | color(Color::Yellow)}),
                    hbox({text("  Chưa ĐD: ") | dim,
                          text(std::to_string(cfChuaDD)) | bold}),
                    separator(),
                    cfAtRisk > 0
                        ? (hbox({text("  [!] ") | color(Color::Red),
                                  text(std::to_string(cfAtRisk) +
                                       " SV sắp vượt ngưỡng cấm thi") | dim}))
                        : text(""),
                    text("  Sẽ tự vắng: " + std::to_string(cfChuaDD) +
                         " SV chưa điểm danh") | dim,
                    separator(),
                    text(" [Y] Xác nhận    [N/Esc] Hủy ") | center | bold,
                }) | border | size(WIDTH, EQUAL, 46) | center;
                return dbox({mainContent | dim,
                             vbox({filler(), dialog, filler()})});
              }
              return mainContent;
            }) |
        CatchEvent([&](Event e) {
          if (showConfirmKhoa) {
            if (e == Event::Character('y') || e == Event::Character('Y')) {
              try {
                app.getDDManager().khoaBuoiVaAutoVang(maLHP, buoiIndex, dsMaSV);
                thongBao = "[OK] Đã khóa buổi điểm danh";
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
            return true;
          }

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
                thongBao = "[OK] Có mặt: " + maSV;
              } catch (const std::exception &ex) {
                thongBao = "[ERR] " + std::string(ex.what());
              }
              quickInput.clear();
            } else {
              thongBao = "[!] Không tìm thấy mã: " + quickInput;
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
                    thongBao = newCP ? "[OK] Vắng có phép: " + maSV
                                     : "[OK] Vắng không phép: " + maSV;
                  } catch (const std::exception &ex) {
                    thongBao = "[ERR] " + std::string(ex.what());
                  }
                } else {
                  thongBao = "[!] Chỉ toggle phép cho SV đang vắng";
                }
              }
              return true;
            }
            if (e == Event::Character('h') || e == Event::Character('H')) {
              if (!dsMaSV.empty() && selected < static_cast<int>(dsMaSV.size())) {
                luaChon = 10;
                screen.Exit();
              }
              return true;
            }
            if (e == Event::Character('g') || e == Event::Character('G')) {
              if (!dsMaSV.empty() && selected < static_cast<int>(dsMaSV.size())) {
                const auto &maSV = dsMaSV[selected];
                if (trangThaiMap.count(maSV)) {
                  luaChon = 20;
                  screen.Exit();
                } else {
                  thongBao = "[!] Hãy điểm danh SV trước khi thêm ghi chú";
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
    } else if (luaChon == 20 && !dsMaSV.empty() &&
               selected < static_cast<int>(dsMaSV.size())) {
      const auto &maSV = dsMaSV[selected];
      auto itSV = svMap.find(maSV);
      std::string tenSV = (itSV != svMap.end()) ? itSV->second.ten : "";
      std::string curGhiChu;
      {
        const auto &lhpRef = app.getLHPManager().getLopRef(maLHP);
        const auto &buoiRef = lhpRef.getDsBuoiDiemDanh()[buoiIndex];
        if (const auto *ct = buoiRef.findChiTiet(maSV))
          curGhiChu = ct->getGhiChu();
      }
      std::string newGhiChu = screenNhapGhiChu(maSV, tenSV, curGhiChu);
      if (newGhiChu != curGhiChu) {
        try {
          app.getDDManager().capNhatGhiChu(maLHP, buoiIndex, maSV, newGhiChu);
          thongBao = "[OK] Đã lưu ghi chú: " + maSV;
        } catch (const std::exception &ex) {
          thongBao = "[ERR] " + std::string(ex.what());
        }
      }
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
                selBuoi = 0;
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