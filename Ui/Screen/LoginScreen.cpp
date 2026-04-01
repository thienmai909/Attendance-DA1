#include <LoginScreen.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include <chrono>
#include <thread>
#include <atomic>

std::optional<std::string> screenLogin(AppManager &app)
{
    auto screen = ScreenInteractive::Fullscreen();

    std::string username, password;
    std::string thongBao;
    std::optional<std::string> ketQua = std::nullopt;
    std::atomic<bool> dangChay = true;

    InputOption usernameOpt;
    usernameOpt.multiline = false;

    InputOption passOpt;
    passOpt.password = true;
    passOpt.multiline = false;

    auto inputUser = Input(&username, "Tên tài khoản...", usernameOpt);
    auto inputPass = Input(&password, "Mật khẩu...", passOpt);

    std::thread timerThread([&] {
        while(dangChay) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (!thongBao.empty()) {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                thongBao.clear();
                screen.PostEvent(Event::Custom);
            }
        }
    });

    auto xuLiDangNhap = [&] {
        auto giangVien = app.getGVManager().dangNhap(username, password);
        if (giangVien.has_value()) {
            ketQua = giangVien->getMaGV();
            LOG_INFO("Login", "Đăng nhập thành công: " + username);
            screen.Exit();
        } else {
            thongBao = "[ERR] Sai tài khoản hoặc mật khẩu!";
            LOG_WARNING("Login", "Đăng nhập thất bại: " + username);
            password.clear();
        }
    };

    ButtonOption btnDangNhapOpt;
    btnDangNhapOpt.transform = [] (const EntryState& s) {
        auto element = text(s.label) 
            | center
            | size(WIDTH, EQUAL, 15) 
            | size(HEIGHT, EQUAL, 1)
            | borderRounded;
        if (s.active) element |= bold;
        if (s.focused) element |= inverted;
        return element;
    };

    auto btnDangNhap = Button("Đăng Nhập", xuLiDangNhap, btnDangNhapOpt);
    auto btnThoat = Button("Thoát", [&] {
        screen.Exit();
    }, btnDangNhapOpt);

    auto layout = Container::Vertical({
        inputUser,
        inputPass,
        Container::Horizontal({btnDangNhap, btnThoat})
    });

    auto bgColorHeader = Color::HSV(
            185 * 255 / 360,
            15 * 255 / 100, 
            100 * 255 / 100
    );

    auto renderer = Renderer(layout, [&] {
        return vbox({
            filler(),
            vbox({
                UiHelper::makeHeader("HỆ THỐNG ĐIỂM DANH", "Vui lòng đăng nhập") | bgcolor(bgColorHeader),
                separator(),
                hbox({ text(" Tài khoản : "), inputUser->Render() | flex }),
                hbox({ text(" Mật khẩu  : "), inputPass->Render() | flex }),
                separator(),
                hbox({
                    btnDangNhap->Render(),
                    text(" ") | size(WIDTH, EQUAL, 20),
                    btnThoat->Render()
                }) | center,
                separator(),
                !thongBao.empty() ? UiHelper::makeMessage(thongBao) : filler()
            }) | border | size(WIDTH, EQUAL, 100) | center,
            filler(),
            UiHelper::makeFooter("[Tab] Chuyển ô  [Enter] Xác nhận  [Q] Thoát")
        });
    }) | CatchEvent([&](Event e) {
        if (e == Event::Custom) return false;
        if (e == Event::Character('q') || e == Event::Character('Q') || e == Event::Escape) {
            screen.Exit();
            return true;
        }
        if (e == Event::Return) {
            if (layout->ActiveChild() == inputPass || layout->ActiveChild() == inputUser) {
                xuLiDangNhap();
                return true;
            }
        }
        return false;
    });

    screen.Loop(renderer);

    dangChay = false;
    if (timerThread.joinable())
        timerThread.join();

    return ketQua;

}