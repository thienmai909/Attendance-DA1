#include <LopHocPhanForm.hpp>
#include <UiHelper.hpp>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

static const std::vector<HocKi> hocKiMap = {
    HocKi::DEFAULT, HocKi::I, HocKi::II, HocKi::He
};

static int hocKiToIndex(HocKi hocKi) {
    for (int i = 0; i < static_cast<int>(hocKiMap.size()); ++i)
        if (hocKiMap[i] == hocKi) return i;
    return 0;
}

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

bool formThemLop(AppManager &app, const std::string &maGV)
{
    auto screen = ScreenInteractive::Fullscreen();
    bool ketQua = false;
    std::string thongBao;

    std::string maLHP, tenLHP, soTCStr, tongSoTietStr, nguongStr;
    int selectedHocKi = 0;

    // --- Phòng học (tùy chọn) ---
    std::string maPhong, sucChuaStr;
    int selectedLoaiPhong = 0; // 0 = Ly thuyet, 1 = Thuc hanh
    auto loaiPhongEntries = std::vector<std::string>{"Lý thuyết", "Thực hành"};

    auto giangVienOpt = app.getGVManager().timTheoMa(maGV);
    bool isAdmin = giangVienOpt.has_value() && giangVienOpt->isAdmin();

    std::string maGVChon = maGV;
    std::string inputMaGVStr = maGV;
    std::string maGVThongBao;

    auto hocKiEntries = std::vector<std::string> {
        "Chưa xác định", "Học kì I", "Học kì II", "Học kì hè"
    };

    InputOption opt = makeInputOpt();

    auto inputMa       = Input(&maLHP, "IN1001", opt);
    auto inputTen      = Input(&tenLHP, "Lap trinh can ban", opt);
    auto inputSoTC     = Input(&soTCStr, "3", opt);
    auto inputTongTiet = Input(&tongSoTietStr, "45", opt);
    auto inputNguong   = Input(&nguongStr, "0.3", opt);
    auto inputMaGV     = Input(&inputMaGVStr, "00234...", opt);
    auto menuHocKi     = Radiobox(&hocKiEntries, &selectedHocKi);
    auto inputMaPhong  = Input(&maPhong, "B2-01 (bỏ trống nếu chưa có)", opt);
    auto inputSucChua  = Input(&sucChuaStr, "50", opt);
    auto menuLoaiPhong = Radiobox(&loaiPhongEntries, &selectedLoaiPhong);

    inputMaGV |= CatchEvent([&](Event e) {
        if (e == Event::Return || e == Event::Tab) {
            if (inputMaGVStr.empty()) {
                maGVChon = maGV;
                maGVThongBao = "";
                return false;
            }
            auto giangVien = app.getGVManager().timTheoMa(inputMaGVStr);
            if (giangVien.has_value()) {
                maGVChon = inputMaGVStr;
                maGVThongBao = "[OK] " + giangVien->getHoTenGV();
            } else {
                maGVChon = maGV;
                maGVThongBao = "[ERR] Không tìm thấy GV: " + inputMaGVStr;
            }
        }
        return false;
    });

    auto btnThem = Button("Thêm", [&] {
        try {
            if (maLHP.empty() || tenLHP.empty())
                throw std::invalid_argument("Mã lớp và Tên lớp không được để trống!");
            if (soTCStr.empty() || tongSoTietStr.empty() || nguongStr.empty())
                throw std::invalid_argument("Vui lòng điền đầy đủ thông tin!");

            int soTC = std::stoi(soTCStr);
            int tongSoTiet = std::stoi(tongSoTietStr);
            double nguong = std::stod(nguongStr);

            if (soTC <= 0 || tongSoTiet <= 0)
                throw std::invalid_argument("Số tín chỉ và tổng số tiết phải > 0!");
            if (nguong < 0.0 || nguong > 1.0)
                throw std::invalid_argument("Ngưỡng cấm thi phải từ 0.0 đến 1.0");

            LopHocPhan lhp(maLHP, tenLHP, soTC, tongSoTiet, nguong, hocKiMap[selectedHocKi]);
            if (!maGVChon.empty())
                lhp.setMaGV(maGVChon);

            if (!maPhong.empty()) {
                int sc = sucChuaStr.empty() ? 0 : std::stoi(sucChuaStr);
                RoomType loai = (selectedLoaiPhong == 1)
                    ? RoomType::PhongThucHanh : RoomType::PhongLyThuyet;
                lhp.setPhongHoc(maPhong, sc, loai);
            }

            app.getLHPManager().them(lhp);
            LOG_INFO("LopHocPhanForm", "Thêm lớp: " + maLHP);
            ketQua = true;
            screen.Exit();
        } catch (const std::invalid_argument& e) {
            thongBao = "[ERR] " + std::string(e.what());
            LOG_ERROR("LopHocPhanForm", e.what());
        }
    }, btnStyle());

    auto btnHuy = Button("Hủy", [&]{ screen.Exit(); }, btnStyle());

    Component layout;
    if (isAdmin) {
        layout = Container::Vertical({
            inputMa, inputTen, inputSoTC,
            inputTongTiet, inputNguong,
            inputMaGV,
            menuHocKi,
            inputMaPhong, inputSucChua, menuLoaiPhong,
            Container::Horizontal({ btnThem, btnHuy })
        });
    } else {
        layout = Container::Vertical({
            inputMa, inputTen, inputSoTC,
            inputTongTiet, inputNguong,
            menuHocKi,
            inputMaPhong, inputSucChua, menuLoaiPhong,
            Container::Horizontal({ btnThem, btnHuy })
        });
    }

    auto renderer = Renderer(layout, [&]{
        Element dongGiangVien;
        if (isAdmin) {
            dongGiangVien = vbox({
                hbox({
                    text(" Giảng viên      : ") | size(WIDTH, EQUAL, 18),
                    inputMaGV->Render() | size(WIDTH, EQUAL, 16),
                    text(" "),
                    text(maGVThongBao) | color(
                        maGVThongBao.starts_with("[OK]")
                            ? Color::Green : Color::Red
                    )
                }),
                hbox({
                    text("                 ") | size(WIDTH, EQUAL, 18),
                    text(" [Tab/Enter] để kiểm tra mã GV ") | dim
                })
            });
        } else {
            std::string tenGV = giangVienOpt.has_value() ? giangVienOpt->getHoTenGV() : maGV;
            dongGiangVien = hbox({
                text(" Giang vien      : ") | size(WIDTH, EQUAL, 18),
                text(tenGV + " (" + maGV + ")") | bold
            });
        }

        return vbox({
            filler(),
            vbox({
                UiHelper::makeHeader("THÊM LỚP HỌC PHẦN"),
                separator(),
                hbox({ text(" Mã lớp          : ") | size(WIDTH, EQUAL, 18), inputMa->Render() | flex }),
                hbox({ text(" Tên lớp         : ") | size(WIDTH, EQUAL, 18), inputTen->Render() | flex }),
                hbox({ text(" Số tín chỉ      : ") | size(WIDTH, EQUAL, 18), inputSoTC->Render() | flex }),
                hbox({ text(" Tổng số tiết    : ") | size(WIDTH, EQUAL, 18), inputTongTiet->Render() | flex }),
                hbox({ text(" Ngưỡng cấm thi  : ") | size(WIDTH, EQUAL, 18), inputNguong->Render() | flex }),
                dongGiangVien,
                separator(),
                hbox({ text(" Học kì          : ") | size(WIDTH, EQUAL, 18), menuHocKi->Render() }),
                separator(),
                text(" Phòng học (tùy chọn): ") | dim,
                hbox({ text("   Mã phòng      : ") | size(WIDTH, EQUAL, 18), inputMaPhong->Render() | flex }),
                hbox({ text("   Sức chứa      : ") | size(WIDTH, EQUAL, 18), inputSucChua->Render() | flex }),
                hbox({ text("   Loại phòng    : ") | size(WIDTH, EQUAL, 18), menuLoaiPhong->Render() }),
                separator(),
                hbox({ btnThem->Render(), text(" "), btnHuy->Render() }) | center,
                separator(),
                UiHelper::makeMessage(thongBao)
            }) | border | size(WIDTH, EQUAL, 62) | center,
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

bool formSuaLop(AppManager &app, const std::string &maLHP)
{
    auto lhpOpt = app.getLHPManager().timTheoMa(maLHP);
    if (!lhpOpt.has_value()) return false;

    auto screen = ScreenInteractive::Fullscreen();
    bool ketQua = false;
    std::string thongBao;
    std::string tenLHP = lhpOpt->getTenLHP();
    std::string soTCStr = std::to_string(lhpOpt->getSoTC());
    std::string tongSoTietStr = std::to_string(lhpOpt->getTongSoTiet());
    std::string nguongStr = std::to_string(lhpOpt->getNguongCamThi());
    int selectedHocKi = hocKiToIndex(lhpOpt->getHocKi());

    std::string maPhong, sucChuaStr;
    int selectedLoaiPhong = 0;
    if (lhpOpt->getPhongHoc().has_value()) {
        const auto &ph = lhpOpt->getPhongHoc().value();
        maPhong    = ph.getTenPhong();
        sucChuaStr = std::to_string(ph.getSucChua());
        selectedLoaiPhong = (ph.getLoaiPhong() == RoomType::PhongThucHanh) ? 1 : 0;
    }
    auto loaiPhongEntries = std::vector<std::string>{"Lý thuyết", "Thực hành"};

    auto hocKiEntries = std::vector<std::string> {
        "Chưa xác định", "Học kì I", "Học kì II", "Học kì hè"
    };

    InputOption opt = makeInputOpt();

    auto inputTen      = Input(&tenLHP, "", opt);
    auto inputSoTC     = Input(&soTCStr, "", opt);
    auto inputTongTiet = Input(&tongSoTietStr, "", opt);
    auto inputNguong   = Input(&nguongStr, "", opt);
    auto menuHocKi     = Radiobox(&hocKiEntries, &selectedHocKi);
    auto inputMaPhong  = Input(&maPhong, "B2-01 (bỏ trống để xóa phòng)", opt);
    auto inputSucChua  = Input(&sucChuaStr, "50", opt);
    auto menuLoaiPhong = Radiobox(&loaiPhongEntries, &selectedLoaiPhong);

    auto btnLuu = Button("Lưu", [&] {
        try {
            if (tenLHP.empty())
                throw std::invalid_argument("Tên lớp không được để trống!");

            int soTC = std::stoi(soTCStr);
            int tongSoTiet = std::stoi(tongSoTietStr);
            double nguong = std::stod(nguongStr);

            if (soTC <= 0 || tongSoTiet <= 0)
                throw std::invalid_argument("Số tín chỉ và tổng số tiết phải > 0!");
            if (nguong < 0.0 || nguong > 1.0)
                throw std::invalid_argument("Ngưỡng cấm thi phải từ 0.0 đến 1.0");

            LopHocPhan lhp(maLHP, tenLHP, soTC, tongSoTiet, nguong, hocKiMap[selectedHocKi]);
            lhp.setMaGV(lhpOpt->getMaGV());

            if (!maPhong.empty()) {
                int sc = sucChuaStr.empty() ? 0 : std::stoi(sucChuaStr);
                RoomType loai = (selectedLoaiPhong == 1)
                    ? RoomType::PhongThucHanh : RoomType::PhongLyThuyet;
                lhp.setPhongHoc(maPhong, sc, loai);
            }

            app.getLHPManager().capNhat(lhp);
            LOG_INFO("LopHocPhanForm", "Sửa lớp: " + maLHP);
            ketQua = true;
            screen.Exit();
        } catch (const std::exception& e) {
            thongBao = "[ERR] " + std::string(e.what());
            LOG_ERROR("LopHocPhanForm", e.what());
        }
    }, btnStyle());

    auto btnHuy = Button("Hủy", [&] { screen.Exit(); }, btnStyle());

    auto layout = Container::Vertical({
        inputTen, inputSoTC,
        inputTongTiet, inputNguong,
        menuHocKi,
        inputMaPhong, inputSucChua, menuLoaiPhong,
        Container::Horizontal({ btnLuu, btnHuy })
    });

    auto renderer = Renderer(layout, [&] {
        std::string phongHienTai = lhpOpt->getTenPhongHoc();
        return vbox({
            filler(),
            vbox({
                UiHelper::makeHeader("SỬA LỚP HỌC PHẦN", "Mã lớp: " + maLHP),
                separator(),
                hbox({ text(" Tên lớp         : ") | size(WIDTH, EQUAL, 18), inputTen->Render()      | flex }),
                hbox({ text(" Số tín chỉ      : ") | size(WIDTH, EQUAL, 18), inputSoTC->Render()     | flex }),
                hbox({ text(" Tổng số tiết    : ") | size(WIDTH, EQUAL, 18), inputTongTiet->Render() | flex }),
                hbox({ text(" Ngưỡng cấm thi  : ") | size(WIDTH, EQUAL, 18), inputNguong->Render()   | flex }),
                separator(),
                hbox({ text(" Học kì          : ") | size(WIDTH, EQUAL, 18), menuHocKi->Render()     | flex }),
                separator(),
                hbox({
                    text(" Phòng hiện tại  : ") | size(WIDTH, EQUAL, 18) | dim,
                    text(phongHienTai.empty() ? "(chưa có)" : phongHienTai) |
                        (phongHienTai.empty() ? dim : bold)
                }),
                text(" Phòng học mới (bỏ trống = xóa phòng): ") | dim,
                hbox({ text("   Mã phòng      : ") | size(WIDTH, EQUAL, 18), inputMaPhong->Render()  | flex }),
                hbox({ text("   Sức chứa      : ") | size(WIDTH, EQUAL, 18), inputSucChua->Render()  | flex }),
                hbox({ text("   Loại phòng    : ") | size(WIDTH, EQUAL, 18), menuLoaiPhong->Render() }),
                separator(),
                hbox({ btnLuu->Render(), text(" "), btnHuy->Render() }) | center,
                separator(),
                UiHelper::makeMessage(thongBao)
            }) | border | size(WIDTH, EQUAL, 62) | center,
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

bool formXoaLop(AppManager &app, const std::string &maLHP)
{
    auto lhpOpt = app.getLHPManager().timTheoMa(maLHP);
    if (!lhpOpt.has_value()) return false;

    auto screen = ScreenInteractive::Fullscreen();
    bool ketQua = false;
    
    int soSV = static_cast<int>(
        app.getDKManager().getDsMaSVTheoLop(maLHP).size()
    );

    auto btnXoa = Button("Xóa", [&] {
        try {
            app.getLHPManager().xoa(maLHP);
            LOG_INFO("LopHocPhanForm", "Xóa lớp: " + maLHP);
            ketQua = true;
            screen.Exit();
        } catch (const std::exception& e) {
            LOG_ERROR("LopHocPhanForm", e.what());
            screen.Exit();
        }
    }, btnStyle());

    auto btnHuy = Button("Hủy", [&] { screen.Exit(); }, btnStyle());
    auto layout = Container::Horizontal({ btnXoa, btnHuy });

    auto renderer = Renderer(layout, [&] {
        std::vector<Element> canhBao;
        if (soSV > 0)
            canhBao.push_back(
                text(" Cảnh báo: Còn " + std::to_string(soSV) + " SV đang đăng ký lớp này! ")
                    | color(Color::Red) | bold | center
            );
        
        return vbox({
            filler(),
            vbox({
                UiHelper::makeHeader("XÓA LỚP HỌC PHẦN") | color(Color::Red),
                separator(),
                text(" Bạn có chắc muốn xóa lớp học phần sau? ") | center,
                separator(),
                hbox({ text(" Mã lớp  : ") | dim, text(maLHP) | bold}) | center,
                hbox({ text(" Tên lớp : ") | dim, text(lhpOpt->getTenLHP()) | bold}) | center,
                hbox({ text(" Số SV   : ") | dim, text(std::to_string(soSV)) | bold}) | center,
                separator(),
                vbox(canhBao),
                text(" Hành động này không thể hoàn tác! ") | color(Color::Magenta) | center,
                separator(),
                hbox({ btnXoa->Render(), text(" "), btnHuy->Render() }) | center
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

bool formSetPhongHoc(AppManager &app, const std::string &maLHP)
{
    auto lhpOpt = app.getLHPManager().timTheoMa(maLHP);
    if (!lhpOpt.has_value()) return false;

    auto screen = ScreenInteractive::Fullscreen();
    bool ketQua = false;
    std::string thongBao;

    const auto& dsPhong = app.getPHManager().getAll();

    std::vector<std::string> entries;
    entries.push_back("  (Xóa phòng - không gắn phòng)");
    for (const auto& p : dsPhong)
        entries.push_back("  " + p.getTenPhong() + "  [" +
                          p.getLoaiPhongStr() + "]  " +
                          std::to_string(p.getSucChua()) + " chỗ");

    int selected = 0;
    auto phHienTai = lhpOpt->getPhongHoc();
    if (phHienTai.has_value()) {
        for (int i = 1; i < static_cast<int>(entries.size()); ++i) {
            if (dsPhong[i - 1].getTenPhong() == phHienTai->getTenPhong()) {
                selected = i; break;
            }
        }
    }

    auto menu = Menu(&entries, &selected);

    auto btnChon = Button("Xác nhận [Enter]", [&] {
        try {
            LopHocPhan lhp = lhpOpt.value();
            if (selected == 0) {
                LopHocPhan lhpMoi(lhp.getMaLHP(), lhp.getTenLHP(),
                                  lhp.getSoTC(), lhp.getTongSoTiet(),
                                  lhp.getNguongCamThi(), lhp.getHocKi());
                lhpMoi.setMaGV(lhp.getMaGV());
                for (const auto& b : lhp.getDsBuoiDiemDanh())
                    ;
                app.getLHPManager().capNhat(lhpMoi);
                thongBao = "[OK] Đã xóa phòng khỏi lớp";
            } else {
                const auto& ph = dsPhong[selected - 1];
                lhp.setPhongHoc(ph);
                app.getLHPManager().capNhat(lhp);
                thongBao = "[OK] Đã đặt phòng: " + ph.getTenPhong();
            }
            ketQua = true;
            screen.Exit();
        } catch (const std::exception& e) {
            thongBao = std::string("[ERR] ") + e.what();
        }
    }, btnStyle());

    auto btnHuy = Button("Hủy [Esc]", [&] { screen.Exit(); }, btnStyle());
    auto layout = Container::Vertical({
        menu,
        Container::Horizontal({ btnChon, btnHuy })
    });

    auto renderer = Renderer(layout, [&] {
        std::string phHT = lhpOpt->getTenPhongHoc();
        return vbox({
            filler(),
            vbox({
                UiHelper::makeHeader("ĐẶT PHÒNG HỌC", maLHP + " - " + lhpOpt->getTenLHP()),
                separator(),
                hbox({ text(" Phòng hiện tại : ") | dim,
                       text(phHT.empty() ? "(chưa có)" : phHT) |
                           (phHT.empty() ? dim : bold | color(Color::Blue))
                }),
                separator(),
                text(" Chọn phòng mới: ") | dim,
                menu->Render() | border | flex,
                separator(),
                hbox({ btnChon->Render(), text("  "), btnHuy->Render() }) | center,
                UiHelper::makeMessage(thongBao),
            }) | border | size(WIDTH, EQUAL, 60) | center,
            filler(),
            UiHelper::makeFooter("[Enter] Xác nhận  [Esc] Hủy  [↑↓] Chọn")
        });
    }) | CatchEvent([&](Event e) {
        if (e == Event::Escape) { screen.Exit(); return true; }
        if (e == Event::Return) { btnChon->OnEvent(Event::Return); return true; }
        return false;
    });

    screen.Loop(renderer);
    return ketQua;
}
