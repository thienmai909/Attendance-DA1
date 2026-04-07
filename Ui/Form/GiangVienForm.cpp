#include <GiangVienForm.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

static const std::vector<Degree> degreeMap = {
    Degree::NONE, Degree::CUNHAN, Degree::THACSI,
    Degree::TIENSI, Degree::PGSTS, Degree::GS
};

static int degreeToIndex(Degree d) {
    for (int i = 0; i < static_cast<int>(degreeMap.size()); ++i)
        if (degreeMap[i] == d) return i;
    return 0;
}

static ButtonOption btnStyle() {
    ButtonOption opt;
    opt.transform = [](const EntryState& s) {
        auto e = text(s.label) | center | size(WIDTH, EQUAL, 14);
        if (s.active)  e |= bold;
        e = e | border;
        if (s.focused) e |= inverted;
        return e;
    };
    return opt;
}

bool formThemGiangVien(AppManager &app)
{
    auto screen = ScreenInteractive::Fullscreen();
    bool ketQua = false;
    std::string thongBao;

    std::string maGV, hoTen, taiKhoan, matKhau, email, sdt;
    int selectedDegree = 0;

    auto degreeEntries = std::vector<std::string>{
        "Không có", "Cử nhân", "Thạc sĩ",
        "Tiến sĩ", "PGS.TS", "GS.TS"
    };

    InputOption opt;
    opt.multiline = false;

    InputOption passOpt;
    passOpt.multiline = false;
    passOpt.password  = true;

    auto inputMa      = Input(&maGV,      " 00234...",         opt);
    auto inputTen     = Input(&hoTen,     " Nguyen Van A",     opt);
    auto inputTK      = Input(&taiKhoan,  " Tên tài khoản...", opt);
    auto inputMK      = Input(&matKhau,   " Mật khẩu...",      passOpt);
    auto inputEmail   = Input(&email,     " gv@edu.vn",        opt);
    auto inputSdt     = Input(&sdt,       " 09xxxxxxxx",       opt);
    auto menuDegree   = Radiobox(&degreeEntries, &selectedDegree);

    auto btnThem = Button("Thêm", [&] {
        try {
            if (maGV.empty() || hoTen.empty())
                throw std::invalid_argument("Mã GV va Họ tên không được để trống!");
            if (taiKhoan.empty() || matKhau.empty())
                throw std::invalid_argument("Tài khoản va mật khẩu không được để trống!");

            // GV tạo từ admin luôn là không phải admin
            GiangVien giangVien(maGV, hoTen, false);
            giangVien.setHocVi(degreeMap[selectedDegree]);
            giangVien.setTaiKhoan(taiKhoan, matKhau);
            if (!email.empty() || !sdt.empty())
                giangVien.setLienHe(email, sdt);

            app.getGVManager().them(giangVien);
            LOG_INFO("GiangVienForm", "Thêm GV: " + maGV);
            ketQua = true;
            screen.Exit();
        } catch (const std::exception& e) {
            thongBao = "[ERR] " + std::string(e.what());
            LOG_ERROR("GiangVienForm", e.what());
        }
    }, btnStyle());

    auto btnHuy = Button("Hủy", [&] { screen.Exit(); }, btnStyle());

    auto layout = Container::Vertical({
        inputMa, inputTen, inputTK, inputMK,
        inputEmail, inputSdt,
        menuDegree,
        Container::Horizontal({ btnThem, btnHuy })
    });

    auto renderer = Renderer(layout, [&] {
        return vbox({
            filler(),
            vbox({
                UiHelper::makeHeader("THÊM GIẢNG VIÊN"),
                separator(),
                hbox({ text(" Mã GV      : ") | size(WIDTH, EQUAL, 14), inputMa->Render()    | flex }),
                hbox({ text(" Họ tên     : ") | size(WIDTH, EQUAL, 14), inputTen->Render()   | flex }),
                hbox({ text(" Tài khoản  : ") | size(WIDTH, EQUAL, 14), inputTK->Render()    | flex }),
                hbox({ text(" Mật khẩu   : ") | size(WIDTH, EQUAL, 14), inputMK->Render()    | flex }),
                hbox({ text(" Email      : ") | size(WIDTH, EQUAL, 14), inputEmail->Render() | flex }),
                hbox({ text(" SĐT        : ") | size(WIDTH, EQUAL, 14), inputSdt->Render()   | flex }),
                separator(),
                hbox({ text(" Học vị     : ") | size(WIDTH, EQUAL, 14), menuDegree->Render() }),
                separator(),
                text(" Lưu ý: Tài khoản này không có quyền Admin ") | dim | center,
                separator(),
                hbox({ btnThem->Render(), text("  "), btnHuy->Render() }) | center,
                separator(),
                UiHelper::makeMessage(thongBao)
            }) | border | size(WIDTH, EQUAL, 60) | center,
            filler(),
            UiHelper::makeFooter("[Tab] Chuyển ô  [Enter] Xác nhận  [Esc] Hủy")
        });
    }) | CatchEvent([&](Event e) {
        if (e == Event::Escape) { screen.Exit(); return true; }
        return false;
    });

    screen.Loop(renderer);
    return ketQua;
}

bool formSuaGiangVien(AppManager &app, const std::string &maGV)
{
    auto giangVienOpt = app.getGVManager().timTheoMa(maGV);
    if (!giangVienOpt.has_value()) return false;

    auto screen = ScreenInteractive::Fullscreen();
    bool ketQua = false;
    std::string thongBao;

    std::string hoTen    = giangVienOpt->getHoTenGV();
    std::string taiKhoan = giangVienOpt->getTenTaiKhoan();
    std::string matKhauMoi;
    std::string email    = giangVienOpt->getLienHe().has_value() ? giangVienOpt->getLienHe()->getEmail() : "";
    std::string sdt      = giangVienOpt->getLienHe().has_value() ? giangVienOpt->getLienHe()->getPhoneNumber()   : "";
    int selectedDegree = degreeToIndex(giangVienOpt->getHocVi());

    auto degreeEntries = std::vector<std::string>{
        "Không có", "Cử nhân", "Thạc sĩ",
        "Tiến sĩ", "PGS.TS", "GS.TS"
    };

    InputOption opt;
    opt.multiline = false;

    InputOption passOpt;
    passOpt.multiline = false;
    passOpt.password  = true;

    auto inputTen   = Input(&hoTen,      "",                opt);
    auto inputTK    = Input(&taiKhoan,   "",                opt);
    auto inputMK    = Input(&matKhauMoi, "De trong = giu nguyen", passOpt);
    auto inputEmail = Input(&email,      "",                opt);
    auto inputSdt   = Input(&sdt,        "",                opt);
    auto menuDegree = Radiobox(&degreeEntries, &selectedDegree);

    auto btnLuu = Button("Lưu", [&] {
        try {
            if (hoTen.empty())
                throw std::invalid_argument("Họ tên không được để trống!");

            GiangVien giangVien(maGV, hoTen, giangVienOpt->isAdmin());
            giangVien.setHocVi(degreeMap[selectedDegree]);

            if (!matKhauMoi.empty())
                giangVien.setTaiKhoan(taiKhoan, matKhauMoi);
            else
                giangVien.khoiPhucTaiKhoan(taiKhoan, giangVienOpt->getMatKhauHash());

            if (!email.empty() || !sdt.empty())
                giangVien.setLienHe(email, sdt);

            app.getGVManager().capNhat(giangVien);
            LOG_INFO("GiangVienForm", "Sửa GV: " + maGV);
            ketQua = true;
            screen.Exit();

        } catch (const std::exception& e) {
            thongBao = "[ERR] " + std::string(e.what());
            LOG_ERROR("GiangVienForm", e.what());
        }
    }, btnStyle());

    auto btnHuy = Button("Hủy", [&] { screen.Exit(); }, btnStyle());

    auto layout = Container::Vertical({
        inputTen, inputTK, inputMK,
        inputEmail, inputSdt,
        menuDegree,
        Container::Horizontal({ btnLuu, btnHuy })
    });

    auto renderer = Renderer(layout, [&] {
        return vbox({
            filler(),
            vbox({
                UiHelper::makeHeader("SỬA GIẢNG VIÊN", "Mã GV: " + maGV),
                separator(),
                hbox({ text(" Họ tên     : ") | size(WIDTH, EQUAL, 14), inputTen->Render()   | flex }),
                hbox({ text(" Tài khoản  : ") | size(WIDTH, EQUAL, 14), inputTK->Render()    | flex }),
                hbox({ text(" Mật khẩu   : ") | size(WIDTH, EQUAL, 14), inputMK->Render()    | flex }),
                hbox({ text(" Email      : ") | size(WIDTH, EQUAL, 14), inputEmail->Render() | flex }),
                hbox({ text(" SĐT        : ") | size(WIDTH, EQUAL, 14), inputSdt->Render()   | flex }),
                separator(),
                hbox({ text(" Học vị     : ") | size(WIDTH, EQUAL, 14), menuDegree->Render() }),
                separator(),
                hbox({ btnLuu->Render(), text("  "), btnHuy->Render() }) | center,
                separator(),
                UiHelper::makeMessage(thongBao)
            }) | border | size(WIDTH, EQUAL, 60) | center,
            filler(),
            UiHelper::makeFooter("[Tab] Chuyển ô  [Enter] Lưu  [Esc] Hủy")
        });
    }) | CatchEvent([&](Event e) {
        if (e == Event::Escape) { screen.Exit(); return true; }
        return false;
    });

    screen.Loop(renderer);
    return ketQua;
}

bool formXoaGiangVien(AppManager &app, const std::string &maGV)
{
    auto giangVienOpt = app.getGVManager().timTheoMa(maGV);
    if (!giangVienOpt.has_value()) return false;

    auto screen = ScreenInteractive::Fullscreen();
    bool ketQua = false;

    int soLop = static_cast<int>(
        app.getLHPManager().getLopTheoGV(maGV).size()
    );

    auto btnXoa = Button("Xóa", [&] {
        try {
            app.getGVManager().xoa(maGV);
            LOG_INFO("GiangVienForm", "Xóa GV: " + maGV);
            ketQua = true;
            screen.Exit();
        } catch (const std::exception& e) {
            LOG_ERROR("GiangVienForm", e.what());
            screen.Exit();
        }
    }, btnStyle());

    auto btnHuy = Button("Hủy", [&] { screen.Exit(); }, btnStyle());
    auto layout = Container::Horizontal({ btnXoa, btnHuy });

    auto renderer = Renderer(layout, [&] {
        std::vector<Element> canhBao;
        if (soLop > 0)
            canhBao.push_back(
                text(" Cảnh báo: GV đang phụ trách " + std::to_string(soLop) + " lớp! ")
                | color(Color::Red) | bold | center
            );

        return vbox({
            filler(),
            vbox({
                UiHelper::makeHeader("XÓA GIẢNG VIÊN") | color(Color::Red),
                separator(),
                text(" Bạn có chắc muốn xóa giảng viên sau? ") | center,
                separator(),
                hbox({ text(" Mã GV  : ") | dim, text(maGV)                        | bold }) | center,
                hbox({ text(" Họ tên : ") | dim, text(giangVienOpt->getHoTenGV())  | bold }) | center,
                hbox({ text(" Số lớp : ") | dim, text(std::to_string(soLop))       | bold }) | center,
                separator(),
                vbox(canhBao),
                text(" Hành động này không thể hoàn tác! ")
                    | color(Color::Yellow) | center,
                separator(),
                hbox({ btnXoa->Render(), text("  "), btnHuy->Render() }) | center
            }) | border | size(WIDTH, EQUAL, 54) | center,
            filler(),
            UiHelper::makeFooter("[Enter] Xác nhận  [Esc] Hủy")
        });
    }) | CatchEvent([&](Event e) {
        if (e == Event::Escape) { screen.Exit(); return true; }
        return false;
    });

    screen.Loop(renderer);
    return ketQua;
}
