// N64XY: ORBITAL GATEWAY - original animated 64-bit ROM library loading scene.
// Native UI geometry and the existing Vulkan shader; no prerecorded video.
#include "loading.h"
#include "n64xy_build_version.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <string>
namespace n64 {
namespace {
constexpr float pi=3.1415926535897932f;
constexpr float tau=2.0f*pi;
uint32_t C(uint8_t r,uint8_t g,uint8_t b,uint8_t a=255){
    return uint32_t(r)|(uint32_t(g)<<8)|(uint32_t(b)<<16)|(uint32_t(a)<<24);
}
float pulse(float t){return 0.5f+0.5f*std::sin(t);}
float clamp01(float v){return std::clamp(v,0.f,1.f);}
} // namespace

void BuildLoadingFrame(fe::FrameDesc& frame,const fe::Fonts& font,unsigned width,unsigned height,
                       const ScanProgress& progress,double elapsed,bool done) {
    frame=fe::FrameDesc{};
    if(width==0||height==0)return;
    const float W=float(width),H=float(height),k=H/2160.f,t=float(elapsed);
    const float bgTop[4]={0.006f,0.013f,0.036f,1.f};
    const float bgMid[4]={0.015f,0.033f,0.053f,1.f};
    const float bgBottom[4]={0.004f,0.011f,0.020f,1.f};
    std::copy(bgTop,bgTop+4,frame.bg.top);
    std::copy(bgMid,bgMid+4,frame.bg.mid);
    std::copy(bgBottom,bgBottom+4,frame.bg.bottom);
    frame.bg.glow_color[0]=0.10f;frame.bg.glow_color[1]=0.77f;
    frame.bg.glow_color[2]=0.38f;frame.bg.glow_color[3]=0.40f;
    frame.bg.glow_pos[0]=0.49f;frame.bg.glow_pos[1]=0.42f;
    frame.bg.glow_pos[2]=0.65f;frame.bg.glow_pos[3]=0.52f;
    frame.bg.floor_glow[0]=0.32f;frame.bg.floor_glow[1]=0.67f;
    frame.bg.floor_glow[2]=0.29f;frame.bg.floor_glow[3]=0.075f;
    frame.bg.misc[0]=W/H;frame.bg.misc[1]=0.84f;frame.bg.misc[2]=0.53f;frame.bg.misc[3]=t;
    frame.time=t;
    auto& ui=frame.ui;
    ui.reserve(2700);
    const auto white=C(248,251,255),muted=C(160,192,213);
    const std::array<uint32_t,4> colors={C(99,255,106),C(62,200,255),
        C(252,85,94),C(255,215,102)};
    auto rect=[&](float x,float y,float w,float h,float radius,uint32_t color) {
        if(w>0&&h>0)fe::Fonts::AddRoundedRect(ui,x,y,w,h,radius,color);
    };
    auto label=[&](const std::string& str,float x,float y,float size,uint32_t color,
                   float weight=0.f,fe::Fonts::Align align=fe::Fonts::Left){
        font.AddText(ui,str.c_str(),x,y,size*k,color,weight,align);
    };
    // Raster grid glides below the gateway; low alpha avoids obscuring art.
    for(int i=0;i<11;++i){
        const float y=H*(0.75f+0.021f*i);
        rect(0,y,W,1.6f*k,0,C(75,150,132,29));
    }
    const float sweep=std::fmod(t*380.f*k,W+200*k)-100*k;
    rect(0,0,W,6*k,0,C(14,97,88));
    rect(sweep,0,175*k,8*k,2*k,colors[0]);
    rect(95*k,107*k,11*k,129*k,4*k,colors[0]);
    label("N64XY",150*k,194*k,106,white,0.70f);
    label(std::string("by YAHBALLON  /  v")+n64xy_build::kVersion,
          153*k,256*k,35,muted,0.17f);
    label(done?"CARTRIDGE SYSTEM ONLINE":"64-BIT ORBITAL GATEWAY",
          W-112*k,198*k,34,done?colors[0]:colors[1],0.35f,fe::Fonts::Right);

    const float cx=W/2,cy=H*0.424f;
    const float active=0.54f+0.46f*pulse(t*2.15f);
    // Orbital gateway: 4 tilted ellipses built from animated pixel stones.
    // This is not the NESYX reactor, SNESXY cube or GBAXY ribbons.
    for(int ring=0;ring<4;++ring){
        const float phase=t*(ring%2?-.53f:.46f)+ring*pi/4.f;
        const float radius=(310.f+float(ring)*34.f)*k;
        const float flatten=ring%2?0.62f:0.91f;
        for(int j=0;j<36;++j){
            const float a=tau*float(j)/36.f+phase;
            const float x=cx+std::cos(a)*radius;
            const float y=cy+std::sin(a)*radius*flatten;
            const float wave=pulse(t*2.f+float(j)*0.37f+ring);
            const float diameter=(7.0f+9.0f*wave)*k;
            const auto col=ring%2?colors[(ring+1)%4]:colors[ring];
            rect(x-diameter/2,y-diameter/2,diameter,diameter,3*k,col);
        }
    }
    // Four 64-bit satellites orbit the gate on a wider elliptical track.
    for(int j=0;j<4;++j){
        const float a=float(j)*pi*0.5f-t*0.23f;
        const float x=cx+std::cos(a)*492*k,y=cy+std::sin(a)*295*k;
        const float d=(24.f+12.f*pulse(t*2.f+j))*k;
        rect(x-d,y-d,d*2,d*2,11*k,C(11,28,45,212));
        rect(x-d*0.60f,y-d*0.60f,d*1.2f,d*1.2f,5*k,colors[j]);
    }
    // A pulsing portal plane with a numerical 64 rather than reused logos.
    const float gateW=265*k,gateH=242*k;
    const float x=cx-gateW/2,y=cy-gateH/2;
    rect(x-20*k,y-20*k,gateW+40*k,gateH+40*k,36*k,C(83,245,113,uint8_t(37+58*active)));
    rect(x,y,gateW,gateH,31*k,C(3,20,32,248));
    rect(x+20*k,y+17*k,gateW-40*k,8*k,4*k,colors[0]);
    rect(x+20*k,y+gateH-27*k,gateW-40*k,8*k,4*k,colors[1]);
    label("64",cx,cy+41*k,149,white,0.85f,fe::Fonts::Center);
    label("NATIVE",cx,cy+87*k,27,colors[0],0.4f,fe::Fonts::Center);

    // Actual folder and file progress: never pretend an NFS share has known
    // total work before source discovery publishes it.
    label(done?"GAMES READY":"DISCOVERING N64 CARTRIDGES",
          cx,H*0.707f,77,white,0.6f,fe::Fonts::Center);
    label(std::to_string(progress.games)+" GAMES   /   "+
          std::to_string(progress.entries)+" FILES READ",
          cx,H*0.756f,38,muted,0.15f,fe::Fonts::Center);
    const float bx=cx-805*k,bw=1610*k,by=H*0.797f;
    rect(bx,by,bw,18*k,8*k,C(30,64,77,225));
    const bool known=progress.sources_total>0;
    const float ratio=done?1.f:(known?
        clamp01(float(progress.sources_done)/float(progress.sources_total)):0.f);
    if(done||known){
        if(ratio>0)rect(bx,by,bw*ratio,18*k,8*k,colors[0]);
        if(!done&&ratio>0.0f)rect(bx+bw*ratio-16*k,by-5*k,32*k,28*k,5*k,colors[1]);
    } else {
        const float pos=pulse(t*2.8f);
        rect(bx+pos*(bw-250*k),by,250*k,18*k,7*k,colors[1]);
    }
    std::string where=progress.directory.empty()?"Searching internal, USB and NFS sources...":progress.directory;
    if(where.size()>100)where="..."+where.substr(where.size()-97);
    label(where,cx,H*0.844f,32,muted,0.10f,fe::Fonts::Center);
    label("FOLDERS "+std::to_string(progress.sources_done)+" / "+
          std::to_string(progress.sources_total),
          cx,H*0.901f,35,done?colors[0]:colors[1],0.29f,fe::Fonts::Center);
    label("N64XY  //  ORBITAL GATEWAY",W-108*k,H*0.966f,26,C(120,155,164),0.21f,fe::Fonts::Right);
}
} // namespace n64
