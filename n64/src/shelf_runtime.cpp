// (AI-assisted) N64 core/video/audio integration, preserving the original shelf renderer.
#include "shelf_runtime.h"
#include "games.h"
#include "audio_buffer.h"
#include "fe_settings.h"
#include <array>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstring>
#include <mutex>
#include <thread>
#include <time.h>
extern "C" {int sceAudioOutInit();int sceAudioOutOpen(int,int,int,unsigned,unsigned,unsigned);
int sceAudioOutOutput(int,const void*);int sceAudioOutClose(int);}
namespace n64 {
class ShelfAudio {
    AudioBuffer buffer;int handle=-1;std::atomic<bool> quit{false};std::thread thread;
public:
    bool Open(){sceAudioOutInit();handle=sceAudioOutOpen(255,0,0,AudioBuffer::grain_frames,48000,1);if(handle<0)return false;
        thread=std::thread([this]{alignas(64) int16_t block[AudioBuffer::grain_frames*2];while(!quit){buffer.Read(block,AudioBuffer::grain_frames);
            if(sceAudioOutOutput(handle,block)<0){timespec pause{0,5000000};nanosleep(&pause,nullptr);}}});return true;}
    void Push(const std::vector<int16_t>& samples){buffer.Push(samples);}
    ~ShelfAudio(){quit=true;if(thread.joinable())thread.join();if(handle>=0){sceAudioOutOutput(handle,nullptr);sceAudioOutClose(handle);}}
};
ShelfRuntime::ShelfRuntime(const std::string& directory):core(directory+"/saves"),root(directory){}
ShelfRuntime::~ShelfRuntime()=default;
bool ShelfRuntime::Start(const std::string& path,fe::Renderer& renderer) {
    error.clear();std::vector<uint8_t> rom;if(!ReadRom(path,rom,error))return false;
    std::map<std::string,std::string> options;
    auto read=[&](const std::string& file){std::string text;fe::settings::ReadFile(file,text);
        for(const auto& item:fe::settings::ReadState(text).kv)options[item.first]=item.second;};
    read(root+"/gs.ini");std::string file=path.substr(path.find_last_of('/')+1);file.resize(file.find_last_of('.'));
    read(root+"/settings/"+file+".ini");
    auto get=[&](const std::string& key,const std::string& fallback){auto at=options.find(key);return at==options.end()?fallback:at->second;};
    core.SetOption("parallel-n64-cpucore",get("N64/CPU","cached_interpreter"));
    core.SetOption("parallel-n64-gfxplugin","angrylion");
    core.SetOption("parallel-n64-rspplugin","hle");
    core.SetOption("parallel-n64-angrylion-multithread",get("N64/Threads","2"));
    core.SetOption("parallel-n64-angrylion-upscaling","1x");
    core.SetOption("parallel-n64-pak1","memory");
    core.SetOption("parallel-n64-alt-map","enabled");
    aspect=get("N64/Aspect","16:9");bindings.Load(options);sound=get("N64/Sound","true")=="true";
    if(!core.Load(path,std::move(rom))){error=core.error;return false;}
    resampler.Reset(core.sample_rate);audio_rate=core.sample_rate;
    if(sound){audio=std::make_unique<ShelfAudio>();if(!audio->Open()){error="N64 audio port failed";audio.reset();}}
    menu_atlas=renderer.Atlas();active=true;previous=0;accumulated=0;last_save=0;
    return true;
}
void ShelfRuntime::Stop(fe::Renderer& renderer) {
    if(!core.Unload())error=core.error;
    audio.reset();renderer.SetAtlas(menu_atlas);
    for(unsigned i=0;i<3;++i) {
        if(game_sets[i])renderer.FreeTextureSet(game_sets[i]);
        if(game_textures[i])renderer.DestroyTexture(game_textures[i]);
        game_sets[i]=VK_NULL_HANDLE;game_textures[i]=nullptr;
    }
    texture_slot=0;active=false;
}
bool ShelfRuntime::Build(fe::Renderer& renderer,fe::FrameDesc& frame,uint32_t raw,double now,uint8_t lx,uint8_t ly,uint8_t rx,uint8_t ry) {
    const auto input=bindings.Update(raw,now);
    if(input.menu){Stop(renderer);return false;}
    if(input.save||input.load) {
        const bool okay=input.save?core.SaveState():core.LoadState();
        if(!okay)error=core.error;
        std::printf("[n64] state %s: %s\n",input.save?"save":"load",okay?"OK":core.error.c_str());
    }
    core.buttons[0]=input.joypad;
    auto axis=[](uint8_t v)->int16_t {int a=int(v)-128;if(std::abs(a)<12)return 0;return static_cast<int16_t>(std::clamp(a*258,-32767,32767));};
    core.axes[0][0][0]=axis(lx);core.axes[0][0][1]=axis(ly);
    core.axes[0][1][0]=axis(rx);core.axes[0][1][1]=axis(ry);

    if(previous==0){previous=now;last_save=now;accumulated=1/core.fps;}
    accumulated+=std::clamp(now-previous,0.0,0.1);previous=now;
    bool changed=false;unsigned runs=0;
    while(accumulated>=1/core.fps&&runs++<2){core.Run();if(audio){if(audio_rate!=core.sample_rate){resampler.Reset(core.sample_rate);audio_rate=core.sample_rate;}audio->Push(resampler.Process(core.audio));}accumulated-=1/core.fps;changed=true;}
    if(now-last_save>=5){if(!core.SaveBattery())error=core.error;last_save=now;}
    // (AI-assisted) Deferred texture retirement uses the existing renderer's fenced frame slots.
    // Continuous menu/game transitions still need proper testing on PS5 hardware.
    if(changed&&!core.pixels.empty()) {
        const unsigned next=(texture_slot+1)%3;
        fe::Texture* fresh=renderer.CreateTexture(core.width,core.height,VK_FORMAT_R8G8B8A8_UNORM,core.pixels.data());
        if(!fresh){error="N64 frame upload failed";Stop(renderer);return false;}
        const VkDescriptorSet set=renderer.AllocTextureSet(fresh,fresh,fresh);
        if(!set){renderer.DestroyTexture(fresh);error="N64 frame descriptor failed";Stop(renderer);return false;}
        if(game_sets[next])renderer.FreeTextureSet(game_sets[next]);
        if(game_textures[next])renderer.DestroyTexture(game_textures[next]);
        game_sets[next]=set;game_textures[next]=fresh;texture_slot=next;
    }
    frame=fe::FrameDesc{};frame.fade=1;
    frame.bg.glow_pos[2]=frame.bg.glow_pos[3]=1;
    frame.bg.floor_glow[2]=frame.bg.floor_glow[3]=1;
    frame.bg.misc[0]=float(renderer.width())/renderer.height();frame.bg.misc[1]=0.5f;
    float w=renderer.width(),h=renderer.height();float width=w,height=h;
    if(aspect=="4:3"){width=std::min(w,h*4.0f/3.0f);height=width*3.0f/4.0f;}
    if(aspect=="Integer"){float scale=std::max(1.0f,std::floor(std::min(w/core.width,h/core.height)));width=core.width*scale;height=core.height*scale;}
    const float x=(w-width)/2,y=(h-height)/2;
    auto vertex=[](float x,float y,float u,float v){return fe::UiVertex{x,y,u,v,0xffffffff,0,0,0,2};};
    frame.ui={vertex(x,y,0,0),vertex(x+width,y,1,0),vertex(x,y+height,0,1),
              vertex(x,y+height,0,1),vertex(x+width,y,1,0),vertex(x+width,y+height,1,1)};
    frame.game_set=game_sets[texture_slot];frame.game_vertices=6;
    return true;
}
}
