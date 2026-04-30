#include <GiangVien.hpp>
#include <MainMenuScreen.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

int screenMainMenu(AppManager &app, const std::string &maGV) {
  auto screen = ScreenInteractive::Fullscreen();
  int ketQua = -1;

  auto giangVienOpt = app.getGVManager().timTheoMa(maGV);
  bool isAdmin = giangVienOpt.has_value() && giangVienOpt->isAdmin();
  std::string tenGV =
      giangVienOpt.has_value() ? giangVienOpt->getHoTenGV() : maGV;

  std::vector<std::string> entries;
  std::vector<int> indices;

  // if (!isAdmin)
  entries.push_back("  [D]  Điểm danh");
  indices.push_back(0);
  entries.push_back("  [L]  Quản lý lớp HP");
  indices.push_back(1);

  if (isAdmin) {
    entries.push_back("  [S]  Quản lý sinh viên");
    indices.push_back(2);
    entries.push_back("  [G]  Quản lý giảng viên");
    indices.push_back(3);
    entries.push_back("  [P]  Quản lý phòng học");
    indices.push_back(5);
  }

  entries.push_back("  [B]  Báo cáo & thống kê");
  indices.push_back(4);
  entries.push_back("  [Q]  Thoát");
  indices.push_back(99);

  int selected = 0;
  auto menu = Menu(&entries, &selected, MenuOption::Vertical());

  auto renderer =
      Renderer(
          menu,
          [&] {
            // ── Thong ke he thong (admin dung) ──
            int soLopAll =
                static_cast<int>(app.getLHPManager().getAll().size());
            int soSVAll = static_cast<int>(app.getSVManager().getAll().size());
            const auto &dsGVAll = app.getGVManager().getAll();
            int soGVAll = static_cast<int>(dsGVAll.size());
            int soGVThuong = 0;
            for (const auto &gv : dsGVAll)
              if (!gv.isAdmin())
                soGVThuong++;

            Element thongTin;

            if (isAdmin) {
              // ── Che do Admin: thong ke toan he thong ──
              thongTin = vbox(
                  {text(" THÔNG TIN HỆ THỐNG ") | bold | center, separator(),
                   hbox({text(" Xin chào   : ") | dim, text(tenGV) | bold}),
                   hbox({text(" Quyền      : ") | dim,
                         text("Quản trị viên") | color(Color::Magenta) | bold}),
                   separator(), text(" Tổng quan :") | bold,
                   hbox({text("   Lớp học phần : ") | dim,
                         text(std::to_string(soLopAll)) | bold |
                             color(Color::Blue)}),
                   hbox({text("   Sinh viên    : ") | dim,
                         text(std::to_string(soSVAll)) | bold |
                             color(Color::Blue)}),
                   hbox({text("   Giảng viên   : ") | dim,
                         text(std::to_string(soGVThuong)) | bold |
                             color(Color::Blue),
                         text("  (+ 1 Admin)") | dim}),
                   filler()});
            } else {
              // ── Che do Giang vien: thong tin ca nhan + lop phu trach ──
              auto lh = giangVienOpt.has_value() ? giangVienOpt->getLienHe()
                                                 : std::optional<Contact>{};
              std::string hocVi =
                  giangVienOpt.has_value() ? giangVienOpt->getHocViStr() : "";

              auto dsLopGV = app.getLHPManager().getLopTheoGV(maGV);

              // Danh sach lop
              std::vector<Element> lopElems;
              for (const auto &lhp : dsLopGV) {
                int svCount = static_cast<int>(
                    app.getDKManager().getDsMaSVTheoLop(lhp.getMaLHP()).size());
                int buoiDone = 0;
                for (const auto &b : lhp.getDsBuoiDiemDanh())
                  if (b.isKhoaDiemDanh())
                    buoiDone++;
                int tongBuoi = static_cast<int>(lhp.getDsBuoiDiemDanh().size());

                lopElems.push_back(hbox({
                    text("  \xe2\x80\xa2 ") | dim, // bullet •
                    text(lhp.getMaLHP()) | bold,
                    text(" ") | dim,
                    text(lhp.getTenLHP()),
                    filler(),
                    text(std::to_string(svCount) + " SV") | color(Color::Blue),
                    text("  ") | dim,
                    text(std::to_string(buoiDone) + "/" +
                         std::to_string(tongBuoi) + " buổi") |
                        dim,
                }));
              }
              if (lopElems.empty())
                lopElems.push_back(text("  (Chưa có lớp nào)") | dim);

              thongTin = vbox(
                  {text(" THÔNG TIN GIẢNG VIÊN ") | bold | center, separator(),
                   hbox({text(" Họ tên  : ") | dim, text(tenGV) | bold}),
                   hbox({text(" Mã GV   : ") | dim, text(maGV)}),
                   hbox({text(" Học vị  : ") | dim, text(hocVi)}), separator(),
                   hbox({text(" Email   : ") | dim,
                         text(lh.has_value() ? lh->getEmail() : "(none)") |
                             color(Color::Blue)}),
                   hbox({text(" SĐT     : ") | dim,
                         text(lh.has_value() ? lh->getPhoneNumber()
                                             : "(none)")}),
                   separator(),
                   hbox({text(" Lớp phụ trách (") | bold,
                         text(std::to_string(dsLopGV.size())) | bold |
                             color(Color::Blue),
                         text(") :") | bold}),
                   vbox(lopElems) | vscroll_indicator | yframe | flex,
                   filler()});
            }

            return vbox(
                {UiHelper::makeHeader("HỆ THỐNG ĐIỂM DANH"), separator(),
                 hbox({vbox({text(" MENU ") | bold | center, separator(),
                             menu->Render() | flex}) |
                           border | size(WIDTH, EQUAL, 36),

                       thongTin | border | flex}) |
                     flex,
                 separator(),
                 UiHelper::makeFooter(isAdmin ? "[D/L/S/G/P/B/Q] Phím tắt  "
                                                "[↑↓] Di chuyển  [Enter] Chọn"
                                              : "[D/L/B/Q] Phím tắt  [↑↓] Di "
                                                "chuyển  [Enter] Chọn")});
          }) |
      CatchEvent([&](Event e) {
        if (e == Event::Return) {
          ketQua = indices[selected];
          screen.Exit();
          return true;
        }

        // if (!isAdmin)
        if (e == Event::Character('d') || e == Event::Character('D')) {
          ketQua = 0;
          screen.Exit();
          return true;
        }

        if (e == Event::Character('l') || e == Event::Character('L')) {
          ketQua = 1;
          screen.Exit();
          return true;
        }

        if (isAdmin) {
          if (e == Event::Character('s') || e == Event::Character('S')) {
            ketQua = 2;
            screen.Exit();
            return true;
          }
          if (e == Event::Character('g') || e == Event::Character('G')) {
            ketQua = 3;
            screen.Exit();
            return true;
          }
          if (e == Event::Character('p') || e == Event::Character('P')) {
            ketQua = 5;
            screen.Exit();
            return true;
          }
        }

        if (e == Event::Character('b') || e == Event::Character('B')) {
          ketQua = 4;
          screen.Exit();
          return true;
        }
        if (e == Event::Character('q') || e == Event::Character('Q') ||
            e == Event::Escape) {
          ketQua = 99;
          screen.Exit();
          return true;
        }

        return false;
      });

  screen.Loop(renderer);
  return ketQua;
}