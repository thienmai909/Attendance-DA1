#include <SinhVienHistoryScreen.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

void screenSinhVienHistory(AppManager &app, const std::string &maLHP,
                           const std::string &maSV) {
  auto screen = ScreenInteractive::Fullscreen();

  // Lấy tên SV
  std::string tenSV;
  auto svOpt = app.getSVManager().timTheoMa(maSV);
  if (svOpt.has_value()) tenSV = svOpt->getTenSV();

  // Lấy tên lớp HP
  std::string tenLHP;
  auto lhpOpt = app.getLHPManager().timTheoMa(maLHP);
  if (lhpOpt.has_value()) tenLHP = lhpOpt->getTenLHP();

  // Lấy lịch sử điểm danh
  auto lichSu = app.getDDManager().lichSuDiemDanhSV(maLHP, maSV);

  // Lấy thông tin ngưỡng
  auto ng = app.getDDManager().kiemTraNguong(maLHP, maSV);

  // Build danh sách hiển thị (dùng Menu để scroll được)
  std::vector<std::string> rows;
  for (const auto &ls : lichSu) {
    std::string stStr;
    if (ls.trangThai == Status::CO_MAT)
      stStr = "[●] Có mặt  ";
    else if (ls.trangThai == Status::VANG)
      stStr = ls.coPhep ? "[P] V.Phép  " : "[ ] Vắng    ";
    else if (ls.trangThai == Status::MUON)
      stStr = "[~] Muộn    ";
    else
      stStr = "[-] Chưa ĐD ";

    std::string note = ls.ghiChu.empty() ? "" : "  -- " + ls.ghiChu;
    rows.push_back("  Buổi " + std::to_string(ls.buoiIndex + 1) + "  " +
                   ls.ngay + "  " + ls.ca + "  " +
                   std::to_string(ls.soTiet) + "t  " + stStr + note);
  }
  if (rows.empty()) rows.push_back("  (Chưa có buổi nào)");

  int selected = 0;
  auto menu = Menu(&rows, &selected);
  auto btnQuayLai = Button("Quay lại [Q]", [&] { screen.Exit(); });
  auto layout = Container::Vertical({menu, btnQuayLai});

  // Chuỗi màu cảnh báo
  std::string warnStr;
  Color warnColor = Color::Default;
  if (ng.biCamThi) {
    warnStr = "  [!!] CẤM THI";
    warnColor = Color::Red;
  } else if (ng.daVuotNguong) {
    warnStr = "  [!!] Nguy hiểm - sắp cấm thi";
    warnColor = Color::Red;
  } else if (ng.sapVuotNguong) {
    warnStr = "  [!] Cần theo dõi";
    warnColor = Color::Yellow;
  }

  auto renderer = Renderer(layout, [&] {
    return vbox({
        UiHelper::makeHeader("LỊCH SỬ ĐIỂM DANH",
                             maSV + (tenSV.empty() ? "" : "  " + tenSV) +
                                 "  |  " + maLHP),
        separator(),
        hbox({
            text("  Tỉ lệ vắng: ") | dim,
            text(std::format("{:.1f}%", ng.tyLeVang * 100.0)) | bold,
            text("   Còn được vắng: ") | dim,
            text(std::to_string(ng.soTietConLai) + " tiết") | bold,
            warnStr.empty() ? text("") : text(warnStr) | color(warnColor) | bold,
        }),
        separator(),
        menu->Render() | border | flex,
        separator(),
        hbox({btnQuayLai->Render()}) | center,
        UiHelper::makeFooter("[↑↓] Cuộn  [Q] Quay lại"),
    });
  }) | CatchEvent([&](Event e) {
    if (e == Event::Character('q') || e == Event::Character('Q') ||
        e == Event::Escape) {
      screen.Exit();
      return true;
    }
    return false;
  });

  screen.Loop(renderer);
}
