#include <BaoCaoScreen.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <algorithm>

using namespace ftxui;

// ----------------------------------------------------------------
// Helper: thanh tab
// ----------------------------------------------------------------
static Element renderTabBar(int activeTab, bool isAdmin) {
    std::vector<std::string> tabs;
    std::vector<int>         tabIdx;
    if (isAdmin) { tabs.push_back("[1] Tổng quan");  tabIdx.push_back(0); }
    tabs.push_back("[2] Chi tiết lớp");  tabIdx.push_back(1);
    tabs.push_back("[3] Danh sách SV");  tabIdx.push_back(2);
    tabs.push_back("[4] Thống kê buổi"); tabIdx.push_back(3);
    tabs.push_back("[5] Xuất file");     tabIdx.push_back(4);

    Elements els;
    for (int i = 0; i < (int)tabs.size(); ++i) {
        auto e = text(" " + tabs[i] + " ");
        if (tabIdx[i] == activeTab)
            e = e | bold | inverted;
        else
            e = e | dim;
        els.push_back(e);
        if (i + 1 < (int)tabs.size()) els.push_back(text("|"));
    }
    return hbox(std::move(els)) | border;
}

// ----------------------------------------------------------------
// Helper: trang thai SV theo nguong
// ----------------------------------------------------------------
static std::string trangThaiSV(double tyLeVang, bool biCamThi) {
    if (biCamThi)        return "[CT] Cấm thi";
    if (tyLeVang > 0.8)  return "[!!] Nguy";
    if (tyLeVang > 0.5)  return " [!] Cần chú ý";
    return "      OK";
}

// ----------------------------------------------------------------
// screenBaoCao — main
// ----------------------------------------------------------------
void screenBaoCao(AppManager& app, const std::string& maGV) {
    auto gvOpt   = app.getGVManager().timTheoMa(maGV);
    bool isAdmin = gvOpt.has_value() && gvOpt->isAdmin();
    std::string tenGV = gvOpt.has_value() ? gvOpt->getHoTenGV() : maGV;

    // Lay danh sach LHP theo quyền
    auto getLopList = [&]() -> std::vector<LopHocPhan> {
        return isAdmin
            ? app.getLHPManager().getAll()
            : app.getLHPManager().getLopTheoGV(maGV);
    };

    // Tab hien tai: admin bat dau tab 0 (tong quan), GV bat dau tab 1
    int activeTab = isAdmin ? 0 : 1;

    // Chon lop chung (cho tab 2,3,4,5)
    int selLop = 0;

    bool thoat = false;

    while (!thoat) {
        auto screen  = ScreenInteractive::Fullscreen();
        int  luaChon = -1;
        std::string thongBao;

        auto dsLop = getLopList();

        // ----------------------------------------------------------
        // Build entries LHP cho menu ben trai
        // ----------------------------------------------------------
        std::vector<std::string> lopEntries;
        for (const auto& lhp : dsLop)
            lopEntries.push_back(lhp.getMaLHP() + " " + lhp.getTenLHP());
        if (lopEntries.empty()) lopEntries.push_back("(Chua co lop)");

        if (!dsLop.empty())
            selLop = std::min(selLop, (int)dsLop.size() - 1);

        // ----------------------------------------------------------
        // Radiobox chon loai bao cao (Tab 5)
        // ----------------------------------------------------------
        int selLoaiBaoCao = 0; // 0=toan dien, 1=SV, 2=camthi, 3=buoi, 4=tongquan
        std::vector<std::string> loaiBCEntries = {
            "Báo cáo toàn diện (4 sheet)",
            "Danh sách SV + thống kê",
            "Danh sách cấm thi",
            "Điem danh từng buổi",
            "Tổng quan tất cả lớp"
        };
        if (!isAdmin) loaiBCEntries.pop_back();

        // ----------------------------------------------------------
        // Components
        // ----------------------------------------------------------
        auto menuLop    = Menu(&lopEntries, &selLop);
        auto radioLoai  = Radiobox(&loaiBCEntries, &selLoaiBaoCao);

        auto btnXuat = Button("Xuất XLSX [X]", [&] {
            if (dsLop.empty()) { thongBao = "[ERR] Không có lớp đề xuất!"; return; }
            std::string maLHPChon = dsLop[selLop].getMaLHP();
            try {
                std::string filePath;
                switch (selLoaiBaoCao) {
                    case 0: filePath = app.getBCManager().xuatBaoCaoToanDien(maLHPChon); break;
                    case 1: app.getBCManager().xuatBaoCaoSinhVien(maLHPChon);
                            filePath = "output/" + maLHPChon + "_sinhvien_*.xlsx"; break;
                    case 2: app.getBCManager().xuatDanhSachCamThi(maLHPChon);
                            filePath = "output/" + maLHPChon + "_camthi_*.xlsx"; break;
                    case 3: app.getBCManager().xuatBaoCaoTatCaBuoi(maLHPChon);
                            filePath = "output/" + maLHPChon + "_tatca_buoi_*.xlsx"; break;
                    case 4: app.getBCManager().xuatBaoCaoTongQuan();
                            filePath = "output/tongquan_*.xlsx"; break;
                }
                // Lay ten file thuc te tu xuatBaoCaoToanDien
                if (selLoaiBaoCao == 0)
                    thongBao = "[OK] Đã xuất: " + filePath;
                else
                    thongBao = "[OK] Đã xuất vào thư mực output/";
                luaChon = 5; // refresh
                screen.Exit();
            } catch (const std::exception& e) {
                thongBao = std::string("[ERR] ") + e.what();
            }
        });

        auto layout = Container::Vertical({ menuLop, radioLoai, btnXuat });

        auto renderer = Renderer(layout, [&] {
            // ---------------------- content theo tab ----------------------
            Element content = filler();

            // --- Tab 0: Tong quan (admin) ---
            if (activeTab == 0 && isAdmin) {
                auto dsLopTK = app.getTKManager().thongKeTatCaLop();
                auto lopMax  = app.getTKManager().lopVangCaoNhat();

                Elements rows;
                rows.push_back(
                    hbox({
                        text(" Mã LHP   ") | bold | size(WIDTH, EQUAL, 12),
                        text(" Tên LHP                     ") | bold | size(WIDTH, EQUAL, 30),
                        text(" SV  ") | bold | size(WIDTH, EQUAL, 6),
                        text(" Buổi") | bold | size(WIDTH, EQUAL, 6),
                        text(" % Vắng  ") | bold | size(WIDTH, EQUAL, 9),
                        text(" SV CT") | bold | size(WIDTH, EQUAL, 7),
                    }) | inverted
                );
                rows.push_back(separator());
                for (const auto& lk : dsLopTK) {
                    int pct = (int)(lk.tyLeVangTrungBinh * 100);
                    bool warn = lopMax.has_value() && lk.maLHP == lopMax->maLHP;
                    auto row = hbox({
                        text(" " + lk.maLHP + " ") | size(WIDTH, EQUAL, 12),
                        text(" " + lk.tenLHP + " ") | size(WIDTH, EQUAL, 30),
                        text(" " + std::to_string(lk.soSinhVien) + " ") | size(WIDTH, EQUAL, 6),
                        text(" " + std::to_string(lk.soBuoiDaHoc) + " ") | size(WIDTH, EQUAL, 6),
                        text(" " + std::to_string(pct) + "%  ") | size(WIDTH, EQUAL, 9),
                        text(" " + std::to_string(lk.soSVBiCamThi) + "    ") | size(WIDTH, EQUAL, 7),
                    });
                    if (warn) row = row | color(Color::Red);
                    else if (pct > 30) row = row | color(Color::Yellow);
                    rows.push_back(row);
                }

                std::string maxInfo = lopMax.has_value()
                    ? lopMax->maLHP + " - " + lopMax->tenLHP +
                      " (" + std::to_string((int)(lopMax->tyLeVangTrungBinh*100)) + "%)"
                    : "(Chưa có dữ liệu)";

                content = vbox({
                    hbox({ text(" Lớp vắng cao nhất: ") | dim, text(maxInfo) | bold | color(Color::Red) }),
                    separator(),
                    vbox(std::move(rows)) | frame | flex
                });
            }

            // --- Tab 1: Chi tiet lop ---
            else if (activeTab == 1) {
                if (!dsLop.empty() && selLop < (int)dsLop.size()) {
                    const std::string& maLHP = dsLop[selLop].getMaLHP();
                    auto tkLop = app.getTKManager().thongKeLop(maLHP);
                    auto top5  = app.getTKManager().topSVVangNhieu(maLHP, 5);
                    auto byCa  = app.getTKManager().tyLeVangTheoCa(maLHP);

                    Elements topRows;
                    for (int i = 0; i < (int)top5.size(); ++i) {
                        const auto& sv = top5[i];
                        topRows.push_back(hbox({
                            text("  " + std::to_string(i+1) + ". ") | dim,
                            text(sv.maSV + " - " + sv.tenSV) | size(WIDTH, EQUAL, 30),
                            text("  (" + std::to_string(sv.soTietVang) + "t - " +
                                 std::to_string((int)(sv.tyLeVang*100)) + "%)") | bold
                        }));
                    }

                    Elements caRows;
                    for (const auto& [ca, ty] : byCa)
                        caRows.push_back(hbox({
                            text("  Ca " + std::to_string((int)ca) + ": ") | dim,
                            text(std::to_string((int)(ty*100)) + "%") | bold
                        }));

                    content = vbox({
                        hbox({ text(" Số SV    : ") | dim, text(std::to_string(tkLop.soSinhVien)) | bold }),
                        hbox({ text(" Số buổi  : ") | dim, text(std::to_string(tkLop.soBuoiDaHoc)) | bold }),
                        hbox({ text(" Tiết học : ") | dim, text(std::to_string(tkLop.soTietDaHoc)) | bold }),
                        hbox({ text(" % Vắng TB: ") | dim,
                               text(std::to_string((int)(tkLop.tyLeVangTrungBinh*100)) + "%") | bold }),
                        hbox({ text(" SV cấm thi: ") | dim,
                               text(std::to_string(tkLop.soSVBiCamThi)) | bold | color(Color::Red) }),
                        separator(),
                        text(" Top 5 vắng nhiều:") | bold,
                        vbox(std::move(topRows)),
                        separator(),
                        text(" Vắng theo ca:") | bold,
                        vbox(std::move(caRows)),
                        filler()
                    });
                }
            }

            // --- Tab 2: Danh sach SV ---
            else if (activeTab == 2) {
                if (!dsLop.empty() && selLop < (int)dsLop.size()) {
                    const std::string& maLHP = dsLop[selLop].getMaLHP();
                    auto dsSV = app.getTKManager().thongKeToanLop(maLHP);

                    Elements svRows;
                    svRows.push_back(
                        hbox({
                            text(" # ") | size(WIDTH, EQUAL, 4),
                            text(" Mã SV    ") | bold | size(WIDTH, EQUAL, 11),
                            text(" Họ Tên                    ") | bold | size(WIDTH, EQUAL, 28),
                            text(" Vắng") | bold | size(WIDTH, EQUAL, 6),
                            text(" Muộn") | bold | size(WIDTH, EQUAL, 6),
                            text(" % Vắng") | bold | size(WIDTH, EQUAL, 8),
                            text(" Trạng Thái    ") | bold,
                        }) | inverted
                    );
                    svRows.push_back(separator());
                    for (int i = 0; i < (int)dsSV.size(); ++i) {
                        const auto& sv = dsSV[i];
                        int pct = (int)(sv.tyLeVang * 100);
                        std::string tt = trangThaiSV(sv.tyLeVang, sv.biCamThi);
                        Color col = sv.biCamThi       ? Color::Red
                                  : sv.tyLeVang > 0.8 ? Color::RedLight
                                  : sv.tyLeVang > 0.5 ? Color::Yellow
                                  :                     Color::White;
                        auto row = hbox({
                            text(" " + std::to_string(i+1) + " ") | size(WIDTH, EQUAL, 4),
                            text(" " + sv.maSV + " ") | size(WIDTH, EQUAL, 11),
                            text(" " + sv.tenSV + " ") | size(WIDTH, EQUAL, 28),
                            text(" " + std::to_string(sv.soTietVang) + "t ") | size(WIDTH, EQUAL, 6),
                            text(" " + std::to_string(sv.soTietMuon) + "t ") | size(WIDTH, EQUAL, 6),
                            text(" " + std::to_string(pct) + "% ") | size(WIDTH, EQUAL, 8),
                            text(" " + tt) | size(WIDTH, EQUAL, 16),
                        }) | color(col);
                        svRows.push_back(row);
                    }
                    if (dsSV.empty())
                        svRows.push_back(text("  (Chưa có dữ liệu điểm danh)") | dim);

                    content = vbox(std::move(svRows)) | frame | flex;
                }
            }

            // --- Tab 3: Thong ke buoi ---
            else if (activeTab == 3) {
                if (!dsLop.empty() && selLop < (int)dsLop.size()) {
                    const std::string& maLHP = dsLop[selLop].getMaLHP();
                    auto dsBuoi = app.getTKManager().thongKeTatCaBuoi(maLHP);
                    auto maxB   = app.getTKManager().buoiVangCaoNhat(maLHP);
                    std::size_t maxIdx = maxB.has_value() ? maxB->buoiIndex : SIZE_MAX;

                    Elements bRows;
                    bRows.push_back(
                        hbox({
                            text(" Buổi") | bold | size(WIDTH, EQUAL, 6),
                            text(" Ngày          ") | bold | size(WIDTH, EQUAL, 15),
                            text(" Ca  ") | bold | size(WIDTH, EQUAL, 6),
                            text(" Tiết") | bold | size(WIDTH, EQUAL, 6),
                            text("  CM ") | bold | size(WIDTH, EQUAL, 6),
                            text(" Vắng") | bold | size(WIDTH, EQUAL, 6),
                            text(" Muộn") | bold | size(WIDTH, EQUAL, 6),
                            text(" %CM    ") | bold | size(WIDTH, EQUAL, 9),
                        }) | inverted
                    );
                    bRows.push_back(separator());
                    for (const auto& b : dsBuoi) {
                        bool isMax = (b.buoiIndex == maxIdx);
                        int pct = (int)(b.tyLeCoMat * 100);
                        auto row = hbox({
                            text(" " + std::to_string(b.buoiIndex+1) + "    ") | size(WIDTH, EQUAL, 6),
                            text(" " + b.ngay + " ") | size(WIDTH, EQUAL, 15),
                            text(" " + b.ca + "  ") | size(WIDTH, EQUAL, 6),
                            text(" " + std::to_string(b.soTiet) + "  ") | size(WIDTH, EQUAL, 6),
                            text("  " + std::to_string(b.soCoMat) + " ") | size(WIDTH, EQUAL, 6),
                            text(" " + std::to_string(b.soVang) + "  ") | size(WIDTH, EQUAL, 6),
                            text(" " + std::to_string(b.soMuon) + "  ") | size(WIDTH, EQUAL, 6),
                            text(" " + std::to_string(pct) + "%") | size(WIDTH, EQUAL, 9),
                        });
                        if (isMax) row = row | color(Color::Red);
                        bRows.push_back(row);
                    }
                    if (dsBuoi.empty())
                        bRows.push_back(text("  (Chưa có buổi điểm danh)") | dim);

                    content = vbox(std::move(bRows)) | frame | flex;
                }
            }

            // --- Tab 4: Xuat file ---
            else if (activeTab == 4) {
                std::string maLHPChon = (!dsLop.empty() && selLop < (int)dsLop.size())
                    ? dsLop[selLop].getMaLHP() : "";
                std::string tenLHPChon = (!dsLop.empty() && selLop < (int)dsLop.size())
                    ? dsLop[selLop].getTenLHP() : "";

                content = vbox({
                    text(" XUẤT BÁO CÁO XLSX ") | bold | center,
                    separator(),
                    hbox({ text(" Lớp học phần : ") | dim,
                           text(maLHPChon + " - " + tenLHPChon) | bold }),
                    separator(),
                    text(" Loại báo cáo:") | dim,
                    radioLoai->Render() | border,
                    separator(),
                    btnXuat->Render() | center,
                    separator(),
                    UiHelper::makeMessage(thongBao),
                    filler()
                });
            }

            // ---------------------- frame chung ----------------------
            Element lopPanel = vbox({
                text(" DANH SÁCH LỚP ") | bold | center,
                separator(),
                menuLop->Render() | flex
            }) | border | size(WIDTH, EQUAL, 28);

            return vbox({
                UiHelper::makeHeader("BÁO CÁO & THỐNG KÊ",
                    isAdmin ? "Admin: " + tenGV : "GV: " + tenGV),
                renderTabBar(activeTab, isAdmin),
                hbox({
                    lopPanel,
                    content | border | flex
                }) | flex,
                separator(),
                UiHelper::makeMessage(thongBao),
                UiHelper::makeFooter(isAdmin
                    ? "[1] Tổng quan  [2] Chi tiết  [3] SV  [4] Buổi  [5] Xuất  [Q] Quay lại"
                    : "[2] Chi tiết  [3] SV  [4] Buổi  [5] Xuất  [Q] Quay lại")
            });
        }) | CatchEvent([&](Event e) {
            // Chuyen tab
            if (e == Event::Character('1') && isAdmin) { activeTab = 0; return true; }
            if (e == Event::Character('2')) { activeTab = 1; return true; }
            if (e == Event::Character('3')) { activeTab = 2; return true; }
            if (e == Event::Character('4')) { activeTab = 3; return true; }
            if (e == Event::Character('5')) { activeTab = 4; return true; }
            // Xuat nhanh
            if ((e == Event::Character('x') || e == Event::Character('X'))
                    && activeTab == 4) {
                btnXuat->OnEvent(Event::Return); return true;
            }
            if (e == Event::Character('q') || e == Event::Character('Q')
                    || e == Event::Escape) {
                luaChon = 99; screen.Exit(); return true;
            }
            return false;
        });

        screen.Loop(renderer);
        if (luaChon == 99) thoat = true;
        // luaChon == 5 → loop lai (refresh sau khi xuat)
    }
}
