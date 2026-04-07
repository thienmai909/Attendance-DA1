#include <LopHocPhanScreen.hpp>
#include <LopHocPhanForm.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

void screenLopHocPhan(AppManager &app, const std::string &maGV)
{
    bool thoat  = false;
    int selected = 0;

    while (!thoat) {
        auto screen = ScreenInteractive::Fullscreen();
        
        auto giangVien = app.getGVManager().timTheoMa(maGV);
        bool isAdmin = giangVien.has_value() && giangVien->isAdmin();

        const std::vector<LopHocPhan>& dsLHP = isAdmin
            ? app.getLHPManager().getAll()
            : app.getLHPManager().getLopTheoGV(maGV);
        
        if (!dsLHP.empty())
            selected = std::min(selected, static_cast<int>(dsLHP.size()) - 1);
        else
            selected = 0;
        
        int luaChon = -1;
        std::vector<std::string> entries;
        for (const auto& lhp : dsLHP)
            entries.push_back(lhp.getMaLHP() + "  " + lhp.getTenLHP());
        if (entries.empty())
            entries.push_back("(Chưa có lớp học phần)");

        std::vector<std::string> thaoTacEntries;
        std::vector<int>         actionCodes;

        if (isAdmin) {
            thaoTacEntries.push_back("[T] Thêm lớp");    actionCodes.push_back(0);
            thaoTacEntries.push_back("[S] Sửa lớp");     actionCodes.push_back(1);
            thaoTacEntries.push_back("[X] Xóa lớp");     actionCodes.push_back(2);
        }
        thaoTacEntries.push_back("[D] Quản lý SV");      actionCodes.push_back(3);
        thaoTacEntries.push_back("[Q] Quay lại");        actionCodes.push_back(99);

        int selectedAction = 0;
        auto menuLHP = Menu(&entries, &selected);
        auto menuAction = Menu(&thaoTacEntries, &selectedAction);
        auto layout = Container::Horizontal({ menuLHP, menuAction });

        auto renderer = Renderer(layout, [&] {
            Element chiTiet = filler();
            if (!dsLHP.empty() && selected < static_cast<int>(dsLHP.size())) {
                const auto& lhp = dsLHP[selected];

                static int cachedSelected = -1;
                static int cachedSoSV = 0;
                static std::string cachedTenGV;

                if (selected != cachedSelected) {
                    cachedSelected = selected;
                    cachedSoSV = static_cast<int>(
                        app.getDKManager().getDsMaSVTheoLop(lhp.getMaLHP()).size()
                    );
                    auto giangVienLop = app.getGVManager().timTheoMa(lhp.getMaGV());
                    cachedTenGV = giangVienLop.has_value()
                        ? giangVienLop->getHoTenGV() + " (" + lhp.getMaGV() + ")"
                        : lhp.getMaGV();
                }

                chiTiet = vbox({
                    text(" CHI TIẾT ") | bold | center,
                    separator(),
                    hbox({ text(" Mã lớp     : ") | dim, text(lhp.getMaLHP())  | bold }),
                    hbox({ text(" Tên lớp    : ") | dim, text(lhp.getTenLHP()) | bold }),
                    hbox({ text(" Tín chỉ    : ") | dim, text(std::to_string(lhp.getSoTC())) }),
                    hbox({ text(" Học kì     : ") | dim, text(lhp.getHocKiStr()) }),
                    hbox({ text(" Giảng viên : ") | dim, text(cachedTenGV) }),
                    hbox({ text(" Số SV      : ") | dim, text(std::to_string(cachedSoSV)) | bold }),
                    hbox({ text(" Tiến độ    : ") | dim, text(lhp.tienDoHocTapStr()) }),
                    hbox({ text(" Ngưỡng CT  : ") | dim, 
                        text(std::to_string(static_cast<int>(lhp.getNguongCamThi() * 100)) + "%") 
                    }),
                    separator(),
                    hbox({ text(" Số buổi    : ") | dim,
                        text(std::to_string(lhp.getDsBuoiDiemDanh().size()))
                    }),
                    filler()
                });
            }

            Element menuElement = menuAction->Render();

            return vbox({
                UiHelper::makeHeader("QUẢN LÝ HỌC PHẦN", 
                    isAdmin ? "Chế độ: Admin" : "Giảng viên: " + maGV),
                separator(),
                hbox({
                    vbox({
                        text(" THAO TÁC ") | bold | center,
                        separator(),
                        menuElement,
                        filler()
                    }) | border | size(WIDTH, EQUAL, 22),

                    vbox({
                        hbox({
                            text(" DANH SÁCH (") | dim,
                            text(std::to_string(dsLHP.size())) | bold,
                            text(") ") | dim
                        }) | center,
                        separator(),
                        menuLHP->Render() | flex
                    }) | border | flex,

                    chiTiet | border | size(WIDTH, EQUAL, 36)
                }) | flex,
                separator(),
                UiHelper::makeFooter(
                    isAdmin
                        ? "[↑↓] Chọn  [T]hêm  [S]ửa  [X]óa  [D] Quản lý SV  [Q] Quay lại"
                        : "[↑↓] Chọn  [D] Quản lý SV  [Q] Quay lại"
                )
            });
        }) | CatchEvent([&](Event e) {
            if (e == Event::Return && layout->ActiveChild() == menuAction)
                { luaChon = selectedAction; screen.Exit(); return true; }
            if (isAdmin) {
                if (e == Event::Character('t') || e == Event::Character('T'))
                    { luaChon = 0; screen.Exit(); return true; }
                if (e == Event::Character('s') || e == Event::Character('S'))
                    { luaChon = 1; screen.Exit(); return true; }
                if (e == Event::Character('x') || e == Event::Character('X'))
                    { luaChon = 2; screen.Exit(); return true; }
            }
            if (e == Event::Character('d') || e == Event::Character('D'))
                { luaChon = 3; screen.Exit(); return true; }
            if (e == Event::Character('q') || e == Event::Character('Q') || e == Event::Escape)
                { luaChon = 99; screen.Exit(); return true; }
            return false;
        });

        screen.Loop(renderer);

        std::string maLHPChon = (!dsLHP.empty() && selected < static_cast<int>(dsLHP.size()))
            ? dsLHP[selected].getMaLHP() : "";
        
        switch(luaChon) {
            case 0:
                formThemLop(app, maGV);
                break;
            case 1:
                if (!maLHPChon.empty()) formSuaLop(app, maLHPChon);
                break;
            case 2:
                if (!maLHPChon.empty()) formXoaLop(app, maLHPChon);
                break;
            case 3:
                if (!maLHPChon.empty()) screenQuanLySVTrongLop(app, maLHPChon);
                break;
            case 99:
                thoat = true;
                break;
        }
    }
}

void screenQuanLySVTrongLop(AppManager &app, const std::string &maLHP)
{
    bool thoat = false;
    int selected = 0;
    std::string thongBao;

    while (!thoat) {
        auto screen = ScreenInteractive::Fullscreen();

        auto danhSachMaSV = app.getDKManager().getDsMaSVTheoLop(maLHP);
        int luaChon = -1;

        if (!danhSachMaSV.empty())
            selected = std::min(selected, static_cast<int>(danhSachMaSV.size()) - 1);
        else
            selected = 0;

        std::vector<std::string> sinhVienEntries;
        for (const auto& maSV : danhSachMaSV) {
            auto sinhVienOpt = app.getSVManager().timTheoMa(maSV);
            std::string ten = sinhVienOpt.has_value() ? sinhVienOpt->getTenSV() : "?";
            sinhVienEntries.push_back(maSV + " " + ten);
        }
        if (sinhVienEntries.empty())
            sinhVienEntries.push_back("(Chưa có danh sách sinh viên)");

        auto menuSV = Menu(&sinhVienEntries, &selected);

        std::string inputMaSVStr;
        InputOption inputOpt;
        inputOpt.multiline = false;
        auto inputMaSV = Input(&inputMaSVStr, "Nhập mã SV...", inputOpt);

        auto btnThem = Button("Thêm", [&] {
            if (inputMaSVStr.empty()) {
                thongBao = "[ERR] Vui lòng nhập mã SV!";
                return; 
            }
            if (!app.getSVManager().timTheoMa(inputMaSVStr).has_value()) {
                thongBao = "[ERR] Không tìm thấy SV: " + inputMaSVStr;
                return;
            }

            try {
                app.getDKManager().dangKy(inputMaSVStr, maLHP);
                LOG_INFO("QuanLySVLop", "Thêm SV " + inputMaSVStr + " vào " + maLHP);
                thongBao = "[OK] Đã thêm SV: " + inputMaSVStr;
                inputMaSVStr.clear();
                luaChon = 0;
                screen.Exit();
            } catch (const std::exception& e) {
                thongBao = "[ERR] " + std::string(e.what());
                LOG_ERROR("QuanLySVLop", e.what());
            }
        });

        auto btnHuyDK = Button("Hủy ĐK", [&] {
            if (danhSachMaSV.empty() || selected >= static_cast<int>(danhSachMaSV.size())) {
                thongBao = "[ERR] Chưa chọn sinh viên!";
                return;
            }
            try {
                std::string maSVHuy = danhSachMaSV[selected];
                app.getDKManager().huyDangKy(maSVHuy, maLHP);
                LOG_INFO("QuanLySVLop", "Hủy ĐK SV " + maSVHuy + " khỏi " + maLHP);
                thongBao = "[OK] Đã hủy đăng ký: " + maSVHuy;
                selected = std::max(0, selected - 1);
                luaChon = 0;
                screen.Exit();
            } catch (const std::exception& e) {
                thongBao = "[ERR] " + std::string(e.what());
                LOG_ERROR("QuanLySVLop", e.what());
            }
        });

        auto btnQuayLai = Button("Quay lại", [&] {
            luaChon = 99;
            screen.Exit();
        });

        auto layout = Container::Vertical({
            inputMaSV,
            Container::Horizontal({ btnThem, btnHuyDK, btnQuayLai }),
            menuSV
        });

        auto renderer = Renderer(layout, [&] {
            static int    cachedSelected = -1;
            static Element cachedChiTiet = filler();

            if (selected != cachedSelected) {
                cachedSelected = selected;
                if (!danhSachMaSV.empty() && selected < static_cast<int>(danhSachMaSV.size())) {
                    auto sinhVienOpt = app.getSVManager().timTheoMa(danhSachMaSV[selected]);
                    if (sinhVienOpt.has_value()) {
                        cachedChiTiet = vbox({
                            text(" CHI TIẾT SINH VIÊN ") | bold | center,
                            separator(),
                            hbox({ text(" Mã SV   : ") | dim, text(sinhVienOpt->getMaSV())     | bold }),
                            hbox({ text(" Họ tên  : ") | dim, text(sinhVienOpt->getTenSV())    | bold }),
                            hbox({ text(" Lớp SH  : ") | dim, text(sinhVienOpt->getLopSHStr()) }),
                            hbox({ text(" Liên hê : ") | dim, text(sinhVienOpt->getLienHeStr()) }),
                            filler()
                        });
                    }
                } else {
                    cachedChiTiet = filler();
                }
            }

            auto lhpOpt = app.getLHPManager().timTheoMa(maLHP);
            std::string tenLHP = lhpOpt.has_value() ? lhpOpt->getTenLHP() : maLHP;

            return vbox({
                UiHelper::makeHeader(
                    "QUẢN LÝ SINH VIÊN TRONG LỚP",
                    maLHP + " - " + tenLHP
                ),
                separator(),
                vbox({
                    text(" THÊM SINH VIÊN ") | bold | center,
                    separator(),
                    hbox({
                        text(" Mã SV : ") | size(WIDTH, EQUAL, 10),
                        inputMaSV->Render() | size(WIDTH, EQUAL, 20),
                        text(" "),
                        btnThem->Render()
                    }),
                }) | border,
                hbox({
                    vbox({
                        hbox({
                            text(" DANH SÁCH (") | dim,
                            text(std::to_string(danhSachMaSV.size())) | bold,
                            text(") ") | dim
                        }) | border,
                        separator(),
                        menuSV->Render() | flex
                    }) | border | flex,
                    cachedChiTiet | border | size(WIDTH, EQUAL, 30)
                }) | flex,
                separator(),
                hbox({
                    btnHuyDK->Render(),
                    text(" "),
                    btnQuayLai->Render()
                }) | center,
                separator(),
                UiHelper::makeMessage(thongBao),
                UiHelper::makeFooter(
                    "[Tab] Chuyển ô  [Enter] Thêm/Hủy  [Q] Quay lại"
                )
            });
        }) | CatchEvent([&](Event e) {
            if (e == Event::Character('q') || e == Event::Character('Q')
                || e == Event::Escape)
                { thoat = true; screen.Exit(); return true; }
            return false;
        });

        screen.Loop(renderer);

        if (luaChon == 99) thoat = true;
    }
}
