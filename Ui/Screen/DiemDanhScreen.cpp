#include <DiemDanhScreen.hpp>
#include <BuoiDiemDanhForm.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

static void screenDiemDanhBuoi(
    AppManager& app,
    const std::string maLHP,
    std::size_t buoiIndex,
    bool isAdmin
) {
    bool thoat = false;
    int selected = 0;
    std::string thongBao;

    auto dsMaSV = app.getDKManager().getDsMaSVTheoLop(maLHP);
    struct SVInfo { std::string ten, lopSH; };
    std::unordered_map<std::string, SVInfo> svMap;

    for (const auto& sv : app.getSVManager().getAll())
        svMap[sv.getMaSV()] = { 
            sv.getTenSV(), 
            sv.getLopSHStr()
        };
    
    auto statusStr = [](Status s) -> std::string {
        switch (s) {
            case Status::CO_MAT: return "[●] Có mặt";
            case Status::VANG:   return "[ ] Vắng   ";
            case Status::MUON:   return "[~] Muộn   ";
            default:             return "[-] Chưa ĐD";
        }
    };

    auto statusColor = [](Status s) -> Color {
        switch (s) {
            case Status::CO_MAT: return Color::Green;
            case Status::VANG:   return Color::Red;
            case Status::MUON:   return Color::Yellow;
            default:             return Color::GrayDark;
        }
    };

    while (!thoat) {
        auto screen  = ScreenInteractive::Fullscreen();
        int  luaChon = -1;

        const auto& lhp  = app.getLHPManager().getLopRef(maLHP);
        if (buoiIndex >= lhp.getDsBuoiDiemDanh().size()) {
            thoat = true;
            break;
        }
        const auto& buoi = lhp.getDsBuoiDiemDanh()[buoiIndex];
        bool        khoa = buoi.isKhoaDiemDanh();

        std::unordered_map<std::string, Status> trangThaiMap;
        for (const auto& ct : buoi.getDanhSachChiTiet())
            trangThaiMap[ct.getMaSV()] = ct.getTrangThai();
        
        std::vector<std::string> entries;
        for (const auto& maSV : dsMaSV) {
            auto itSV = svMap.find(maSV);
            auto itTT = trangThaiMap.find(maSV);

            std::string ten  = (itSV != svMap.end()) ? itSV->second.ten : "(?)";
            Status      tt   = (itTT != trangThaiMap.end())
                ? itTT->second : Status::DEFAULT;

            entries.push_back(
                statusStr(tt) + "  " + maSV + "  " + ten
            );
        }

        if (entries.empty())
            entries.push_back("(Chưa có sinh viên đăng ký)");

        if (!dsMaSV.empty())
            selected = std::min(selected, static_cast<int>(dsMaSV.size()) - 1);

        auto menuSV = Menu(&entries, &selected);

        auto btnCoMat = Button("Có mặt [1]", [&] {
            if (khoa) { thongBao = "[ERR] Buổi đã khóa!"; return; }
            if (dsMaSV.empty() || selected >= static_cast<int>(dsMaSV.size())) return;
            try {
                app.getDDManager().capNhatTrangThai(
                    maLHP, buoiIndex, dsMaSV[selected], Status::CO_MAT);
            } catch (...) {
                try {
                    app.getDDManager().diemDanh(
                        maLHP, buoiIndex, dsMaSV[selected],
                        DateTime(), Status::CO_MAT, "");
                } catch (const std::exception& e) {
                    thongBao = "[ERR] " + std::string(e.what());
                    return;
                }
            }
            thongBao = "[OK] Có mặt: " + dsMaSV[selected];
            luaChon  = 0;
            screen.Exit();
        });

        auto btnVang = Button("Vắng   [2]", [&] {
            if (khoa) { thongBao = "[ERR] Buổi đã khóa!"; return; }
            if (dsMaSV.empty() || selected >= static_cast<int>(dsMaSV.size())) return;
            try {
                app.getDDManager().capNhatTrangThai(
                    maLHP, buoiIndex, dsMaSV[selected], Status::VANG);
            } catch (...) {
                try {
                    app.getDDManager().diemDanh(
                        maLHP, buoiIndex, dsMaSV[selected],
                        DateTime(), Status::VANG, "");
                } catch (const std::exception& e) {
                    thongBao = "[ERR] " + std::string(e.what()); return;
                }
            }
            thongBao = "[OK] Vắng: " + dsMaSV[selected];
            luaChon  = 0;
            screen.Exit();
        });

        auto btnMuon = Button("Muộn   [3]", [&] {
            if (khoa) { thongBao = "[ERR] Buổi đã khóa!"; return; }
            if (dsMaSV.empty() || selected >= static_cast<int>(dsMaSV.size())) return;
            try {
                app.getDDManager().capNhatTrangThai(
                    maLHP, buoiIndex, dsMaSV[selected], Status::MUON);
            } catch (...) {
                try {
                    app.getDDManager().diemDanh(
                        maLHP, buoiIndex, dsMaSV[selected],
                        DateTime(), Status::MUON, "");
                } catch (const std::exception& e) {
                    thongBao = "[ERR] " + std::string(e.what()); return;
                }
            }
            thongBao = "[OK] Muộn: " + dsMaSV[selected];
            luaChon  = 0;
            screen.Exit();
        });

        auto btnTatCaCoMat = Button("Tất cả có mặt [A]", [&] {
            if (khoa) { thongBao = "[ERR] Buổi đã khóa!"; return; }
            bool coLoi = false;
            for (int i = 0; i < static_cast<int>(dsMaSV.size()); ++i) {
                const std::string& maSV = dsMaSV[i];
                try {
                    if (trangThaiMap.count(maSV))
                        app.getDDManager().capNhatTrangThai(
                            maLHP, buoiIndex, maSV, Status::CO_MAT);
                    else
                        app.getDDManager().diemDanh(
                            maLHP, buoiIndex, maSV, DateTime(), Status::CO_MAT, "");

                    trangThaiMap[maSV] = Status::CO_MAT;
                    auto itSV = svMap.find(maSV);
                    std::string ten = (itSV != svMap.end()) ? itSV->second.ten : "(?)";
                    entries[i] = statusStr(Status::CO_MAT) + "  " + maSV + "  " + ten;
                } catch (const std::exception& e) {
                    thongBao = "[ERR] " + std::string(e.what());
                    coLoi = true;
                    break;
                }
            }
            if (!coLoi)
                thongBao = "[OK] Đã điểm danh tất cả có mặt";
        });

        auto btnKhoa = Button("Khóa buổi [K]", [&] {
            if (khoa) { thongBao = "[ERR] Buổi đã khóa!"; return; }
            try {
                app.getDDManager().khoaBuoiVaAutoVang(
                    maLHP, buoiIndex, dsMaSV);
                thongBao = "[OK] Đã khóa buổi điểm danh";
                luaChon  = 0;
                screen.Exit();
            } catch (const std::exception& e) {
                thongBao = "[ERR] " + std::string(e.what());
            }
        });

        auto btnQuayLai = Button("Quay lại [Q]", [&] {
            luaChon = 99;
            screen.Exit();
        });

        auto layout = Container::Vertical({
            menuSV,
            Container::Horizontal({ btnCoMat, btnVang, btnMuon }),
            Container::Horizontal({ btnTatCaCoMat, btnKhoa, btnQuayLai })
        });

        auto renderer = Renderer(layout, [&] {
            // Thống kê nhanh
            int soCoMat = 0, soVang = 0, soMuon = 0, soChuaDD = 0;
            for (const auto& maSV : dsMaSV) {
                auto it = trangThaiMap.find(maSV);
                if (it == trangThaiMap.end() || it->second == Status::DEFAULT)     
                    ++soChuaDD;
                else if (it->second == Status::CO_MAT) 
                    ++soCoMat;
                else if (it->second == Status::VANG)   
                    ++soVang;
                else if (it->second == Status::MUON)   
                    ++soMuon;
            }

            return vbox({
                UiHelper::makeHeader(
                    "ĐIỂM DANH",
                    maLHP + "  |  Buổi " + std::to_string(buoiIndex + 1)
                    + "  |  " + buoi.getNgayDiemDanhStr()
                    + "  " + buoi.getCaDiemDanhStr()
                    + (khoa ? "  [ĐÃ KHÓA]" : "")
                ),
                separator(),
                // Thống kê nhanh
                hbox({
                    text("  Có mặt: ") | dim,
                    text(std::to_string(soCoMat)) | color(Color::Green) | bold,
                    text("  Vắng: ") | dim,
                    text(std::to_string(soVang))  | color(Color::Red)   | bold,
                    text("  Muộn: ") | dim,
                    text(std::to_string(soMuon))  | color(Color::Yellow)| bold,
                    text("  Chưa ĐD: ") | dim,
                    text(std::to_string(soChuaDD))| color(Color::GrayDark)| bold,
                }) | center,
                separator(),
                // Danh sách SV
                vbox({
                    menuSV->Render() | flex
                }) | border | flex,
                separator(),
                // Nút điểm danh
                khoa
                    ? text(" Buổi này đã bị khóa - không thể điểm danh thêm ") 
                        | color(Color::Red) | center
                    : vbox({
                        hbox({
                            btnCoMat->Render(), text(" "),
                            btnVang->Render(),  text(" "),
                            btnMuon->Render()
                        }) | center,
                        hbox({
                            btnTatCaCoMat->Render(), text(" "),
                            btnKhoa->Render(),       text(" "),
                            btnQuayLai->Render()
                        }) | center
                    }),
                separator(),
                UiHelper::makeMessage(thongBao),
                UiHelper::makeFooter(
                    khoa
                        ? "[Q] Quay lại"
                        : "[1] Có mặt  [2] Vắng  [3] Muộn  [A] Tất cả  [K] Khóa  [Q] Quay lai"
                )
            });
        }) | CatchEvent([&](Event e) {
            if (!khoa) {
                if (e == Event::Character('1'))
                    { btnCoMat->OnEvent(Event::Return); return true; }
                if (e == Event::Character('2'))
                    { btnVang->OnEvent(Event::Return);  return true; }
                if (e == Event::Character('3'))
                    { btnMuon->OnEvent(Event::Return);  return true; }
                if (e == Event::Character('a') || e == Event::Character('A'))
                    { btnTatCaCoMat->OnEvent(Event::Return); return true; }
                if (e == Event::Character('k') || e == Event::Character('K'))
                    { btnKhoa->OnEvent(Event::Return);  return true; }
            }
            if (e == Event::Character('q') || e == Event::Character('Q')
             || e == Event::Escape)
                { luaChon = 99; screen.Exit(); return true; }
            return false;
        });

        screen.Loop(renderer);
        if (luaChon == 99) thoat = true;
    }
}

void screenDiemDanh(AppManager &app, const std::string &maGV)
{
    bool thoat = false;
    int  selLop  = 0;
    int  selBuoi = 0;

    auto gvOpt   = app.getGVManager().timTheoMa(maGV);
    bool isAdmin = gvOpt.has_value() && gvOpt->isAdmin();

    while(!thoat) {
        auto screen  = ScreenInteractive::Fullscreen();
        int  luaChon = -1;

        const std::vector<LopHocPhan>& dsLHP = isAdmin
            ? app.getLHPManager().getAll()
            : app.getLHPManager().getLopTheoGV(maGV);
        
        if (!dsLHP.empty())
            selLop = std::min(selLop, static_cast<int>(dsLHP.size()) - 1);
        
        std::vector<std::string> lopEntries;
        for (const auto& lhp : dsLHP)
            lopEntries.push_back(lhp.getMaLHP() + "  " + lhp.getTenLHP());
        if (lopEntries.empty())
            lopEntries.push_back("(Chưa có lớp)");
        
        auto menuLop = Menu(&lopEntries, &selLop);

        // buoiEntries được cập nhật động trong renderer khi selLop thay đổi
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
                const auto& buois = dsLHP[selLop].getDsBuoiDiemDanh();
                if (!buois.empty() && selBuoi < static_cast<int>(buois.size())) {
                    luaChon = 1;
                    screen.Exit();
                } else {
                    // Chưa có buổi → hỏi tạo mới
                    luaChon = 2;
                    screen.Exit();
                }
            }
        });

        auto btnQuayLai = Button("Quay lại [Q]", [&] {
            luaChon = 99;
            screen.Exit();
        });

        auto layout = Container::Horizontal({
            menuLop,
            Container::Vertical({
                menuBuoi,
                Container::Horizontal({ btnTaoBuoi, btnDiemDanh, btnQuayLai })
            })
        });

        int cachedSelLop = -1;
        int cachedSoSV   = 0;
        int cachedSoBuoi = 0;

        auto renderer = Renderer(layout, [&] {
            if (selLop != cachedSelLop) {
                cachedSelLop = selLop;
                buoiEntries.clear();
                selBuoi = 0; // reset về buổi đầu khi đổi lớp
                if (!dsLHP.empty() && selLop < static_cast<int>(dsLHP.size())) {
                    const auto& lhp = dsLHP[selLop];
                    cachedSoSV   = static_cast<int>(
                        app.getDKManager().getDsMaSVTheoLop(lhp.getMaLHP()).size());
                    cachedSoBuoi = static_cast<int>(lhp.getDsBuoiDiemDanh().size());
                    for (std::size_t i = 0; i < lhp.getDsBuoiDiemDanh().size(); ++i) {
                        const auto& b = lhp.getDsBuoiDiemDanh()[i];
                        buoiEntries.push_back(
                            "Buổi " + std::to_string(i + 1)
                            + "  " + b.getNgayDiemDanhStr()
                            + "  " + b.getCaDiemDanhStr()
                            + "  " + std::to_string(b.getSoTiet()) + " tiết"
                            + (b.isKhoaDiemDanh() ? "  [KHÓA]" : "")
                        );
                    }
                    if (buoiEntries.empty())
                        buoiEntries.push_back("(Chưa có buổi điểm danh)");
                }
            }

            return vbox({
                UiHelper::makeHeader("ĐIỂM DANH",
                    isAdmin ? "Admin" : "GV: " + maGV),
                separator(),
                hbox({
                    // Cột trái: danh sách lớp
                    vbox({
                        text(" CHỌN LỚP ") | bold | center,
                        separator(),
                        menuLop->Render() | flex
                    }) | border | size(WIDTH, EQUAL, 30),

                    // Cột giữa: danh sách buổi
                    vbox({
                        hbox({
                            text(" BUỔI ĐIỂM DANH (") | dim,
                            text(std::to_string(cachedSoBuoi)) | bold,
                            text(") ") | dim,
                            text("  SV: " + std::to_string(cachedSoSV)) | dim
                        }) | center,
                        separator(),
                        menuBuoi->Render() | flex,
                        separator(),
                        hbox({
                            btnTaoBuoi->Render(),
                            text(" "),
                            btnDiemDanh->Render(),
                            text(" "),
                            btnQuayLai->Render()
                        }) | center
                    }) | border | flex
                }) | flex,
                separator(),
                UiHelper::makeFooter(
                    "[↑↓] Chọn lớp/buổi  [N] Tạo buổi  [Enter] Điểm danh  [Q] Quay lại"
                )
            });
        }) | CatchEvent([&](Event e) {
            if (e == Event::Character('n') || e == Event::Character('N'))
                { btnTaoBuoi->OnEvent(Event::Return); return true; }
            if (e == Event::Return)
                { btnDiemDanh->OnEvent(Event::Return); return true; }
            if (e == Event::Character('q') || e == Event::Character('Q')
             || e == Event::Escape)
                { luaChon = 99; screen.Exit(); return true; }
            return false;
        });

        screen.Loop(renderer);
        if (!dsLHP.empty() && selLop < static_cast<int>(dsLHP.size())) {
            std::string maLHPChon = dsLHP[selLop].getMaLHP();
            switch (luaChon) {
                case 0: break;  // đã tạo buổi, loop lại refresh
                case 1:
                    screenDiemDanhBuoi(app, maLHPChon, selBuoi, isAdmin);
                    break;
                case 2:
                    // Chưa có buổi → mở form tạo luôn
                    if (formTaoBuoiDiemDanh(app, maLHPChon))
                        selBuoi = 0;
                    break;
                case 99: thoat = true; break;
            }
        } else {
            if (luaChon == 99) thoat = true;
        }
    }
}