// (AI-assisted) N64 playback hosted by the original PS5SX2 Vulkan shelf.
#pragma once
#include "core_session.h"
#include "controller_bindings.h"
#include "resampler.h"
#include "fe_renderer.h"
#include <memory>
namespace n64 {
class ShelfAudio;
class ShelfRuntime {
public:
    explicit ShelfRuntime(const std::string& directory);
    ~ShelfRuntime();
    bool Start(const std::string& path,fe::Renderer& renderer);
    bool Build(fe::Renderer& renderer,fe::FrameDesc& frame,uint32_t buttons,double now,uint8_t lx=128,uint8_t ly=128,uint8_t rx=128,uint8_t ry=128);
    void Stop(fe::Renderer& renderer);
    bool Active()const{return active;}
    bool UseLeftStickDpad()const{return bindings.left_stick_dpad;}
    std::string error;
private:
    Session core;
    ControllerBindings bindings;
    Resampler resampler;
    std::unique_ptr<ShelfAudio> audio;
    std::string root;
    fe::Texture* menu_atlas=nullptr;
    fe::Texture* game_textures[3]={nullptr,nullptr,nullptr};
    VkDescriptorSet game_sets[3]={VK_NULL_HANDLE,VK_NULL_HANDLE,VK_NULL_HANDLE};
    unsigned texture_slot=0;
    bool active=false,sound=true;
    std::string aspect="16:9";
    double audio_rate=48000;
    double previous=0,accumulated=0,last_save=0;
};
}
