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