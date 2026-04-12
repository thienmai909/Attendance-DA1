#include <BuoiDiemDanhForm.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

static const std::vector<CaHoc> caHocMap = {
    CaHoc::DEFAULT, CaHoc::SANG, CaHoc::CHIEU, CaHoc::TOI
};

static ButtonOption btnStyle() {
    ButtonOption opt;
    opt.transform = [](const EntryState& s) {
        auto e = text(s.label) | center | size(WIDTH, EQUAL, 14);
        if (s.active) e |= bold;
        e = e | border;
        if (s.focused) e |= inverted;
        return e;
    };
    return opt;
}

bool formTaoBuoiDiemDanh(AppManager &app, const std::string &maLHP)
{
    auto screen = ScreenInteractive::Fullscreen();
    bool ketQua = false;
    std::string thongBao;

    DateTime now = DateTime();
    std::string ngayStr = now.toDayString();
    std::string soTietStr = "3";
    int selectedCa = 1;

    auto caEntries = std::vector<std::string> {
        "Chưa xác định", "Sáng", "Chiều", "Tối"
    };

    InputOption opt;
    opt.multiline = false;

    auto inputNgay = Input(&ngayStr, "DD/MM/YYYY", opt);
    auto inputTiet = Input(&soTietStr, "3", opt);
    auto menuCa = Radiobox(&caEntries, &selectedCa);

    auto btnTao = Button("Tạo Buổi", [&] {
        try {
            if (ngayStr.empty())
                throw std::invalid_argument("Vui lòng nhập ngày điểm danh");
            if (soTietStr.empty())
                throw std::invalid_argument("Vui lòng nhập số tiết!");

            int d = 0, m = 0, y = 0;
            if (std::sscanf(ngayStr.c_str(), "%d/%d/%d", &d, &m, &y) != 3)
                throw std::invalid_argument("Ngày sai định dạng (DD/MM/YYYY)!");
            
            int soTiet = std::stoi(soTietStr);
            if (soTiet <= 0)
                throw std::invalid_argument("Số tiết phải > 0!");

            DateTime ngay(d, m, y);
            app.getLHPManager().themBuoi(maLHP, ngay, caHocMap[selectedCa], soTiet);
            LOG_INFO("BuoiForm", "Tạo buổi cho lớp " + maLHP);
            ketQua = true;
            screen.Exit();
        } catch (const std::exception& e) {
            thongBao = "[ERR] " + std::string(e.what());
            LOG_ERROR("BuoiForm", e.what());
        }
    }, btnStyle());

    auto btnHuy = Button("Hủy", [&] { screen.Exit(); }, btnStyle());

    auto layout = Container::Vertical({
        inputNgay, inputTiet,
        menuCa,
        Container::Horizontal({ btnTao, btnHuy })
    });

    auto renderer = Renderer(layout, [&] {
        auto lhpOpt = app.getLHPManager().timTheoMa(maLHP);
        std::string tenLHP = lhpOpt.has_value() ? lhpOpt->getTenLHP() : maLHP;

        return vbox({
            filler(),
            vbox({
                UiHelper::makeHeader("TẠO BUỔI ĐIỂM DANH", maLHP + " - " + tenLHP),
                separator(),
                hbox({ text(" Ngày     : ") | size(WIDTH, EQUAL, 12), inputNgay->Render() | flex }),
                hbox({ text(" Số tiết  : ") | size(WIDTH, EQUAL, 12), inputTiet->Render() | flex }),
                separator(),
                hbox({ text(" Ca học   : ") | size(WIDTH, EQUAL, 12), menuCa->Render() }),
                separator(),
                hbox({ btnTao->Render(), text("  "), btnHuy->Render() }) | center,
                separator(),
                UiHelper::makeMessage(thongBao)
            }) | border | size(WIDTH, EQUAL, 50) | center,
            filler(),
            UiHelper::makeFooter("[Tab] Chuyển ô  [Enter] Tạo  [Esc] Huỷ")
        });
    }) | CatchEvent([&](Event e) {
        if (e == Event::Escape) { screen.Exit(); return true; }
        return false;
    });

    screen.Loop(renderer);
    return ketQua;
}