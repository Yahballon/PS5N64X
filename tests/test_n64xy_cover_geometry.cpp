#include "fe_n64xy_cover_aspect.h"
#include <cassert>
#include <cmath>
#include <iostream>
int main(){
 auto square=fe::N64XYCaseSize(1.f,448.f,505.f);
 assert(std::abs(square.width-square.height)<.01f);
 auto portrait=fe::N64XYCaseSize(.72f,448.f,505.f);
 assert(portrait.width<portrait.height);
 auto wide=fe::N64XYCaseSize(1.42f,448.f,505.f);
 assert(wide.width>wide.height && wide.width<=505.f);
 assert(std::abs(wide.width/wide.height-1.42f)<.01f);
 auto fall=fe::N64XYFitInside(1.f,wide.width,wide.height);
 assert(std::abs(fall.width-fall.height)<.01f);
 std::cout<<"PASS N64XY square, landscape and portrait native 3D cases\n";
}
