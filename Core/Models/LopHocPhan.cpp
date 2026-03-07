#include <LopHocPhan.hpp>

LopHocPhan::LopHocPhan(
    std::string maLHP, 
    std::string tenLHP, 
    int soTC, 
    int tongSoTiet, 
    double nguongCamThi, 
    HocKi hocKi
) :
    _maLHP(maLHP), 
    _tenLHP(tenLHP), 
    _soTC(soTC), 
    _tongSoTiet(tongSoTiet), 
    _nguongCamThi(nguongCamThi),
    _hocKi(hocKi)
{
}