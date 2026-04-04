#include <SinhVienForm.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include <vector>

using namespace ftxui;

static ButtonOption btnStyle() {
    ButtonOption opt;
    opt.transform = [] (const EntryState& s) {
        auto e = text(s.label) | center | size(WIDTH, EQUAL, 14);
        if (s.active) e |= bold;
        e = e | bold;
        if (s.focused) e |= inverted;
        return e;
    };
    return opt;
}

bool formThemSinhVien(AppManager &app)
{
    auto screen = ScreenInteractive::Fullscreen();
    bool ketQua = false;
    std::string thongBao;

    std::string maSV, tenSV, ngaySinh, email, sdt;
    int selectedLop = 0;

    auto lopEntries = std::vector<std::string> {
        "Chưa xếp lớp",
        "ĐHSTIN23A",
        "ĐHSTIN23B",
        "ĐHSTIN23C"
    };

    InputOption opt;
    opt.multiline = false;

    auto inputMa = Input(&maSV, "VD: 00234xxxxx", opt);
    auto inputTen = Input(&tenSV, "VD: Nguyễn Văn A", opt);
    auto inputNgay = Input(&ngaySinh, "VD: 01/01/2005", opt);
    auto inputEmail = Input(&email, "VD: mãSV@student.dthu.edu.vn", opt);
    auto inputSdt = Input(&sdt, "VD: 03xxxxxxxx", opt);
    auto menuLop = Radiobox(&lopEntries, &selectedLop);

    auto btnThem = Button("Thêm", [&] {
        try {
            if (maSV.empty() || tenSV.empty())
                throw std::invalid_argument("Mã SV và Họ tên không được để trống");
            
            SinhVien sinhVien(maSV, tenSV);
            sinhVien.setLopSH(static_cast<LopSinhHoat>(selectedLop));

            if (!ngaySinh.empty()) {
                int d = 0, m = 0, y = 0;
                if (std::sscanf(ngaySinh.c_str(), "%d/%d/%d", &d, &m, &y) == 3)
                    sinhVien.setNgaySinh(d, m, y); 
                else
                    throw std::invalid_argument("Ngày sinh sai định dạng (DD/MM/YYYY)!");
            }
            if (!email.empty() || !sdt.empty())
                sinhVien.setLienHe(email, sdt);
            
            app.getSVManager().them(sinhVien);
            LOG_INFO("SinhVienForm", "Thêm SV: " + maSV);
            ketQua = true;
            screen.Exit();
        } catch (const std::exception& e) {
            thongBao = "[ERR] " + std::string(e.what());
            LOG_ERROR("SinhVienForm", e.what());
        }
    }, btnStyle());

    auto btnHuy = Button("Hủy", [&] { screen.Exit(); }, btnStyle());
    auto layout = Container::Vertical({
        inputMa, inputTen, inputNgay,
        inputEmail, inputSdt,
        menuLop,
        Container::Horizontal({btnThem, btnHuy})
    });

    auto renderer = Renderer(layout, [&] {
        return vbox({
            filler(),
            vbox({
                UiHelper::makeHeader("THÊM SINH VIÊN"),
                separator(),
                hbox({ text(" Mã SV     : ") | size(WIDTH, EQUAL, 14), inputMa->Render() | flex}),
                hbox({ text(" Họ  tên   : ") | size(WIDTH, EQUAL, 14), inputTen->Render() | flex}),
                hbox({ text(" Ngày sinh : ") | size(WIDTH, EQUAL, 14), inputNgay->Render() | flex}),
                hbox({ text(" Email     : ") | size(WIDTH, EQUAL, 14), inputEmail->Render() | flex}),
                hbox({ text(" SĐT       : ") | size(WIDTH, EQUAL, 14), inputSdt->Render() | flex}),
                separator(),
                hbox({ text(" Lớp SH    : ") | size(WIDTH, EQUAL, 14), menuLop->Render()}),
                separator(),
                hbox({
                    btnThem->Render(),
                    text(" "),
                    btnHuy->Render()
                }) | center,
                separator(),
                UiHelper::makeMessage(thongBao)
            }) | border | size(WIDTH, EQUAL, 60) | center,
            filler(),
            UiHelper::makeFooter("[Tab] Chuyển ô  [Enter] Xác nhận  [Esc] Hủy")
        });
    }) | CatchEvent([&](Event e) {
        if (e == Event::Escape) {
            screen.Exit(); 
            return true;
        }
        return false; 
    });

    screen.Loop(renderer);
    return ketQua;
}

bool formSuaSinhVien(AppManager &app, const std::string &maSV)
{
    auto sinhVienOpt = app.getSVManager().timTheoMa(maSV);
    if (!sinhVienOpt.has_value()) return false;

    auto screen = ScreenInteractive::Fullscreen();
    bool ketQua = false;
    std::string thongBao;

    std::string tenSV = sinhVienOpt->getTenSV();
    std::string ngaySinh = sinhVienOpt->getNgaySinhStr();
    std::string email = sinhVienOpt->getLienHe().has_value() ? sinhVienOpt->getLienHe()->getEmail() : "";
    std::string sdt = sinhVienOpt->getLienHe().has_value() ? sinhVienOpt->getLienHe()->getPhoneNumber() : "";
    int selectedLop = static_cast<int>(sinhVienOpt->getLopSH());

    auto lopEntries = std::vector<std::string>{
        "Chưa xếp lớp", "ĐHSTIN23A", "ĐHSTIN23B", "ĐHSTIN23C"
    };

    InputOption inputOpt;
    inputOpt.multiline = false;

    auto inputTen = Input(&tenSV, "", inputOpt);
    auto inputNgay = Input(&ngaySinh, "DD/MM/YYYY", inputOpt);
    auto inputEmail = Input(&email, "", inputOpt);
    auto inputSdt = Input(&sdt, "", inputOpt);
    auto menuLop = Radiobox(&lopEntries, &selectedLop);

    auto btnLuu = Button("Lưu", [&] {
        try {
            if (tenSV.empty())
                throw std::invalid_argument("Họ tên không được để trống!");
            
            SinhVien sinhVien(maSV, tenSV);
            sinhVien.setLopSH(static_cast<LopSinhHoat>(selectedLop));

            if (!ngaySinh.empty()) {
                int d = 0, m = 0, y = 0;
                if (std::sscanf(ngaySinh.c_str(), "%d/%d/%d", &d, &m, &y) == 3)
                    sinhVien.setNgaySinh(d, m, y);
                else
                    throw std::invalid_argument("Ngày sinh sai định dạng!");
            }

            if (!email.empty() || !sdt.empty())
                sinhVien.setLienHe(email, sdt);
            
            app.getSVManager().capNhat(sinhVien);
            LOG_INFO("SinhVienForm", "Sửa SV: " + maSV);
            ketQua = true;
            screen.Exit();
        } catch (const std::exception& e) {
            thongBao = "[ERR] " + std::string(e.what());
            LOG_ERROR("SinhVienForm", e.what());
        }
    }, btnStyle());

    auto btnHuy = Button("Hủy", [&] { screen.Exit(); }, btnStyle());

    auto layout = Container::Vertical({
        inputTen, inputNgay, inputEmail, inputSdt,
        menuLop,
        Container::Horizontal({ btnLuu, btnHuy })
    });

    auto renderer = Renderer(layout, [&] {
        return vbox({
            filler(),
            vbox({
                UiHelper::makeHeader("SỬA THÔNG TIN SINH VIÊN", "Mã sinh viên: " + maSV),
                separator(),
                hbox({ text(" Họ tên    : ") | size(WIDTH, EQUAL, 14), inputTen->Render()   | flex}),
                hbox({ text(" Ngày sinh : ") | size(WIDTH, EQUAL, 14), inputNgay->Render()  | flex}),
                hbox({ text(" Email     : ") | size(WIDTH, EQUAL, 14), inputEmail->Render() | flex}),
                hbox({ text(" SĐT       : ") | size(WIDTH, EQUAL, 14), inputSdt->Render()   | flex}),
                separator(),
                hbox({ text(" Lớp SH    : ") | size(WIDTH, EQUAL, 14), menuLop->Render()}),
                separator(),
                hbox({ btnLuu->Render(), text(" "), btnHuy->Render() }) | center,
                separator(),
                UiHelper::makeMessage(thongBao)
            }) | border | size(WIDTH, EQUAL, 60) | center,
            filler(),
            UiHelper::makeFooter("[Tab] chuyển ô  [Enter] Lưu  [Esc] Hủy")
        });
    }) | CatchEvent([&] (Event e) {
        if (e == Event::Escape) { screen.Exit(); return true; }
        return false;
    });

    screen.Loop(renderer);
    return ketQua;
}

bool formXoaSinhVien(AppManager &app, const std::string &maSV)
{
    auto sinhVienOpt = app.getSVManager().timTheoMa(maSV);
    if (!sinhVienOpt.has_value()) return false;

    auto screen = ScreenInteractive::Fullscreen();
    bool ketQua = false;

    auto btnXoa = Button("Xóa", [&] {
        try {
            app.getSVManager().xoa(maSV);
            LOG_INFO("SinhVienForm", "Xóa SV: " + maSV);
            ketQua = true;
            screen.Exit();
        } catch (const std::exception& e) {
            LOG_ERROR("SinhVienForm", e.what());
            screen.Exit();
        }
    }, btnStyle());

    auto btnHuy = Button("Hủy", [&] { screen.Exit(); }, btnStyle());

    auto layout = Container::Horizontal({ btnXoa, btnHuy });

    auto renderer = Renderer(layout, [&] {
        return vbox({
            filler(),
            vbox({
                UiHelper::makeHeader("XÓA SINH VIÊN") | color(Color::Red),
                separator(),
                text(" Bạn có chắc muốn xóa sinh viên sau?") | center,
                separator(),
                hbox({ text(" Mã SV  : ") | dim, text(maSV)                    | bold}) | center,
                hbox({ text(" Họ tên : ") | dim, text(sinhVienOpt->getTenSV()) | bold}) | center,
                separator(),
                text(" Hành động này không thể hoàn tác! ") | color(Color::Yellow) | center,
                separator(),
                hbox({ btnXoa->Render(), text(" "), btnHuy->Render() }) | center
            }) | border | size(WIDTH, EQUAL, 50) | center,
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
