#include <LopHocPhanForm.hpp>
#include <LopHocPhanScreen.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

void screenLopHocPhan(AppManager &app, const std::string &maGV) {
  bool thoat = false;
  int selected = 0;

  while (!thoat) {
    auto screen = ScreenInteractive::Fullscreen();

    auto giangVien = app.getGVManager().timTheoMa(maGV);
    bool isAdmin = giangVien.has_value() && giangVien->isAdmin();

    const std::vector<LopHocPhan> &dsLHP =
        isAdmin ? app.getLHPManager().getAll()
                : app.getLHPManager().getLopTheoGV(maGV);

    if (!dsLHP.empty())
      selected = std::min(selected, static_cast<int>(dsLHP.size()) - 1);
    else
      selected = 0;

    int luaChon = -1;
    std::vector<std::string> entries;
    for (const auto &lhp : dsLHP)
      entries.push_back(lhp.getMaLHP() + "  " + lhp.getTenLHP());
    if (entries.empty())
      entries.push_back("(Chưa có lớp học phần)");

    std::vector<std::string> thaoTacEntries;
    std::vector<int> actionCodes;

    thaoTacEntries.push_back("[D] Quản lý SV");
    actionCodes.push_back(3);
    if (isAdmin) {
      thaoTacEntries.push_back("[T] Thêm lớp");
      actionCodes.push_back(0);
      thaoTacEntries.push_back("[S] Sửa lớp");
      actionCodes.push_back(1);
      thaoTacEntries.push_back("[X] Xóa lớp");
      actionCodes.push_back(2);
      thaoTacEntries.push_back("[R] Đặt phòng");
      actionCodes.push_back(4);
    }
    thaoTacEntries.push_back("[Q] Quay lại");
    actionCodes.push_back(99);

    int selectedAction = 0;
    auto menuLHP = Menu(&entries, &selected);
    auto menuAction = Menu(&thaoTacEntries, &selectedAction);
    auto layout = Container::Horizontal({menuLHP, menuAction});

    int cachedSelected = -1;
    int cachedSoSV = 0;
    std::string cachedTenGV;

    auto renderer =
        Renderer(
            layout,
            [&] {
              Element chiTiet = filler();
              if (!dsLHP.empty() && selected < static_cast<int>(dsLHP.size())) {
                const auto &lhp = dsLHP[selected];

                if (selected != cachedSelected) {
                  cachedSelected = selected;
                  cachedSoSV =
                      static_cast<int>(app.getDKManager()
                                           .getDsMaSVTheoLop(lhp.getMaLHP())
                                           .size());
                  auto giangVienLop =
                      app.getGVManager().timTheoMa(lhp.getMaGV());
                  cachedTenGV = giangVienLop.has_value()
                                    ? giangVienLop->getHoTenGV() + " (" +
                                          lhp.getMaGV() + ")"
                                    : lhp.getMaGV();
                }

                chiTiet = vbox(
                    {text(" CHI TIẾT ") | bold | center, separator(),
                     hbox({text(" Mã lớp     : ") | dim,
                           text(lhp.getMaLHP()) | bold}),
                     hbox({text(" Tên lớp    : ") | dim,
                           text(lhp.getTenLHP()) | bold}),
                     hbox({text(" Tín chỉ    : ") | dim,
                           text(std::to_string(lhp.getSoTC()))}),
                     hbox({text(" Học kì     : ") | dim,
                           text(lhp.getHocKiStr())}),
                     hbox({text(" Giảng viên : ") | dim, text(cachedTenGV)}),
                     hbox({text(" Số SV      : ") | dim,
                           text(std::to_string(cachedSoSV)) | bold}),
                     hbox({text(" Tiến độ    : ") | dim,
                           text(lhp.tienDoHocTapStr())}),
                     hbox({text(" Ngưỡng CT  : ") | dim,
                           text(std::to_string(static_cast<int>(
                                    lhp.getNguongCamThi() * 100)) +
                                "%")}),
                     separator(),
                     hbox({text(" Số buổi    : ") | dim,
                           text(std::to_string(
                               lhp.getDsBuoiDiemDanh().size()))}),
                     separator(),
                     // Phong hoc
                     [&]() -> Element {
                       auto ph = lhp.getPhongHoc();
                       if (ph.has_value()) {
                         return vbox({
                             hbox({text(" Phòng      : ") | dim,
                                   text(ph->getTenPhong()) | bold |
                                       color(Color::Blue)}),
                             hbox({text(" Loại       : ") | dim,
                                   text(ph->getLoaiPhongStr())}),
                             hbox({text(" Sức chứa   : ") | dim,
                                   text(std::to_string(ph->getSucChua()) +
                                        " chỗ")}),
                         });
                       } else {
                         return hbox({text(" Phòng      : ") | dim,
                                      text("(Chưa có phòng)") | dim});
                       }
                     }(),
                     filler()});
              }

              Element menuElement = menuAction->Render();

              return vbox(
                  {UiHelper::makeHeader("QUẢN LÝ HỌC PHẦN",
                                        isAdmin ? "Chế độ: Admin"
                                                : "Giảng viên: " + maGV),
                   separator(),
                   hbox({vbox({text(" THAO TÁC ") | bold | center, separator(),
                               menuElement, filler()}) |
                             border | size(WIDTH, EQUAL, 22),

                         vbox({hbox({text(" DANH SÁCH (") | dim,
                                     text(std::to_string(dsLHP.size())) | bold,
                                     text(") ") | dim}) |
                                   center,
                               separator(), menuLHP->Render() | flex}) |
                             border | flex,

                         chiTiet | border | size(WIDTH, EQUAL, 60)}) |
                       flex,
                   separator(),
                   UiHelper::makeFooter(
                       isAdmin ? "[T]hêm  [S]ửa  [X]óa  [R] Đặt phòng  [D] "
                                 "Quản lý SV  [Q] Quay lại"
                               : "[D] Quản lý SV  [Q] Quay lại")});
            }) |
        CatchEvent([&](Event e) {
          if (e == Event::Return && layout->ActiveChild() == menuAction) {
            luaChon = selectedAction;
            screen.Exit();
            return true;
          }
          if (isAdmin) {
            if (e == Event::Character('t') || e == Event::Character('T')) {
              luaChon = 0;
              screen.Exit();
              return true;
            }
            if (e == Event::Character('s') || e == Event::Character('S')) {
              luaChon = 1;
              screen.Exit();
              return true;
            }
            if (e == Event::Character('x') || e == Event::Character('X')) {
              luaChon = 2;
              screen.Exit();
              return true;
            }
            if (e == Event::Character('r') || e == Event::Character('R')) {
              luaChon = 4;
              screen.Exit();
              return true;
            }
          }
          if (e == Event::Character('d') || e == Event::Character('D')) {
            luaChon = 3;
            screen.Exit();
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

    std::string maLHPChon =
        (!dsLHP.empty() && selected < static_cast<int>(dsLHP.size()))
            ? dsLHP[selected].getMaLHP()
            : "";

    switch (luaChon) {
    case 0:
      formThemLop(app, maGV);
      break;
    case 1:
      if (!maLHPChon.empty())
        formSuaLop(app, maLHPChon);
      break;
    case 2:
      if (!maLHPChon.empty())
        formXoaLop(app, maLHPChon);
      break;
    case 3:
      if (!maLHPChon.empty())
        screenQuanLySVTrongLop(app, maLHPChon, isAdmin);
      break;
    case 4:
      if (!maLHPChon.empty())
        formSetPhongHoc(app, maLHPChon);
      break;
    case 99:
      thoat = true;
      break;
    }
  }
}

void screenQuanLySVTrongLop(AppManager &app, const std::string &maLHP,
                            bool isAdmin) {
  bool thoat = false;
  int selected = 0;
  std::string thongBao;

  auto lhpOpt = app.getLHPManager().timTheoMa(maLHP);
  std::string tenLHP = lhpOpt.has_value() ? lhpOpt->getTenLHP() : maLHP;

  struct SVInfo {
    std::string ten, lopSH, email, sdt;
  };
  auto buildSVMap = [&]() {
    std::unordered_map<std::string, SVInfo> map;
    for (const auto &sinhVien : app.getSVManager().getAll()) {
      auto lh = sinhVien.getLienHe();
      map[sinhVien.getMaSV()] = {
          sinhVien.getTenSV(),
          sinhVien.getLopSHStr(),
          lh.has_value() ? lh->getEmail() : "(none)",
          lh.has_value() ? lh->getPhoneNumber() : ""
      };
    }
    return map;
  };

  auto sinhVienMap = buildSVMap();

  auto buildEntries = [&](const std::vector<std::string> &danhSachMaSV) {
    std::vector<std::string> entries;
    entries.reserve(danhSachMaSV.size());
    for (const auto &maSV : danhSachMaSV) {
      auto it = sinhVienMap.find(maSV);
      entries.push_back(maSV + "  " +
                        ((it != sinhVienMap.end()) ? it->second.ten : "(?)"));
    }
    if (entries.empty())
      entries.push_back("(Chưa có sinh viên)");
    return entries;
  };

  auto danhSachMaSV = app.getDKManager().getDsMaSVTheoLop(maLHP);
  auto entries = buildEntries(danhSachMaSV);

  auto refresh = [&]() {
    danhSachMaSV = app.getDKManager().getDsMaSVTheoLop(maLHP);
    entries = buildEntries(danhSachMaSV);
    if (!danhSachMaSV.empty())
      selected = std::min(selected, static_cast<int>(danhSachMaSV.size()) - 1);
    else
      selected = 0;
  };

  while (!thoat) {
    auto screen = ScreenInteractive::Fullscreen();
    int luaChon = -1;

    int cachedSelected = -1;
    Element cachedChiTiet = filler();

    if (!danhSachMaSV.empty())
      selected = std::min(selected, static_cast<int>(danhSachMaSV.size()) - 1);

    std::string inputMaSVStr;
    InputOption inputOpt;
    inputOpt.multiline = false;
    inputOpt.transform = [](InputState state) -> Element { return state.element | color(Color::Black); };

    auto menuSV = Menu(&entries, &selected);
    auto inputMaSV = Input(&inputMaSVStr, "Nhập mã SV...", inputOpt);

    auto btnThem = Button("Thêm", [&] {
      if (!isAdmin) {
        thongBao = "[ERR] Không có quyền thêm sinh viên!";
        return;
      }
      if (inputMaSVStr.empty()) {
        thongBao = "[ERR] Vui lòng nhập mã SV!";
        return;
      }
      if (sinhVienMap.find(inputMaSVStr) == sinhVienMap.end()) {
        thongBao = "[ERR] Không tìm thấy SV: " + inputMaSVStr;
        return;
      }

      try {
        app.getDKManager().dangKy(inputMaSVStr, maLHP);
        LOG_INFO("QuanLySVLop", "Thêm SV " + inputMaSVStr + " vào " + maLHP);
        thongBao = "[OK] Đã thêm SV: " + inputMaSVStr;
        inputMaSVStr.clear();
        refresh();
        luaChon = 0;
        screen.Exit();
      } catch (const std::exception &e) {
        thongBao = "[ERR] " + std::string(e.what());
        LOG_ERROR("QuanLySVLop", e.what());
      }
    });

    auto btnHuyDK = Button("Hủy Đăng Ký", [&] {
      if (!isAdmin) {
        thongBao = "[ERR] Không có quyền thêm sinh viên!";
        return;
      }
      if (danhSachMaSV.empty() ||
          selected >= static_cast<int>(danhSachMaSV.size())) {
        thongBao = "[ERR] Chưa chọn sinh viên!";
        return;
      }
      try {
        std::string maSVHuy = danhSachMaSV[selected];
        app.getDKManager().huyDangKy(maSVHuy, maLHP);
        LOG_INFO("QuanLySVLop", "Hủy ĐK SV " + maSVHuy + " khỏi " + maLHP);
        thongBao = "[OK] Đã hủy đăng ký: " + maSVHuy;
        selected = std::max(0, selected - 1);
        refresh();
        luaChon = 0;
        screen.Exit();
      } catch (const std::exception &e) {
        thongBao = "[ERR] " + std::string(e.what());
        LOG_ERROR("QuanLySVLop", e.what());
      }
    });

    auto btnQuayLai = Button("Quay lại", [&] {
      luaChon = 99;
      screen.Exit();
    });

    Component layout;
    if (isAdmin) {
      layout = Container::Vertical(
          {inputMaSV, Container::Horizontal({btnThem, btnHuyDK, btnQuayLai}),
           menuSV});
    } else {
      layout =
          Container::Vertical({Container::Horizontal({btnQuayLai}), menuSV});
    }

    auto renderer =
        Renderer(
            layout,
            [&] {
              if (selected != cachedSelected) {
                cachedSelected = selected;
                cachedChiTiet = filler();

                if (!danhSachMaSV.empty() &&
                    selected < static_cast<int>(danhSachMaSV.size())) {
                  const std::string &maSV = danhSachMaSV[selected];
                  auto it = sinhVienMap.find(maSV);
                  if (it != sinhVienMap.end()) {
                    auto sinhVienOpt = app.getSVManager().timTheoMa(maSV);
                    if (sinhVienOpt.has_value()) {
                      const auto &info = it->second;
                      cachedChiTiet = vbox(
                          {text(" CHI TIẾT SINH VIÊN ") | bold | center,
                           separator(),
                           hbox({text(" Mã SV   : ") | dim,
                                 text(danhSachMaSV[selected]) | bold}),
                           hbox({text(" Họ tên  : ") | dim,
                                 text(info.ten) | bold}),
                           hbox({text(" Lớp SH  : ") | dim, text(info.lopSH)}),
                           separator(),
                           hbox({text(" Email   : ") | dim,
                                 text(info.email) | color(Color::Blue)}),
                           hbox({text(" SĐT     : ") | dim, text(info.sdt)}),
                           filler()});
                    }
                  }
                }
              }

              Element vungThaoTac;
              if (isAdmin) {
                vungThaoTac =
                    vbox({text(" THÊM SINH VIÊN ") | bold | center, separator(),
                          hbox({text(" Mã SV : ") | size(WIDTH, EQUAL, 10),
                                inputMaSV->Render() | size(WIDTH, EQUAL, 20),
                                text("  "), btnThem->Render()})}) |
                    border;
              } else {
                vungThaoTac =
                    vbox({text(" CHỈ XEM - Không có quyền chỉnh sửa ") | dim |
                          center}) |
                    border;
              }

              return vbox(
                  {UiHelper::makeHeader("QUẢN LÝ SINH VIÊN TRONG LỚP",
                                        maLHP + " - " + tenLHP),
                   separator(), vungThaoTac,
                   hbox({vbox({hbox({text(" DANH SÁCH (") | dim,
                                     text(std::to_string(danhSachMaSV.size())) |
                                         bold,
                                     text(") ") | dim}) |
                                   border,
                               separator(), menuSV->Render() | vscroll_indicator | yframe | flex}) |
                             border | flex,
                         cachedChiTiet | border | size(WIDTH, EQUAL, 42)}) |
                       flex,
                   separator(),
                   isAdmin ? hbox({btnHuyDK->Render(), text("  "),
                                   btnQuayLai->Render()}) |
                                 center
                           : hbox({btnQuayLai->Render()}) | center,
                   separator(), UiHelper::makeMessage(thongBao),
                   UiHelper::makeFooter(
                       "[Tab] Chuyển ô  [Enter] Thêm/Hủy  [Q] Quay lại")});
            }) |
        CatchEvent([&](Event e) {
          if (e == Event::Character('q') || e == Event::Character('Q') ||
              e == Event::Escape) {
            thoat = true;
            screen.Exit();
            return true;
          }
          return false;
        });

    screen.Loop(renderer);

    if (luaChon == 99)
      thoat = true;
  }
}
