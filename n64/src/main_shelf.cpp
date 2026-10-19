// (AI-assisted) Original PS5SX2 frontend startup, with a N64 playback backend.
#include "fe_ps5.h"
#include "elevation.hpp"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
extern "C" int sceNetInit();
extern "C" int sceSystemServiceLoadExec(const char* path, const char* const* argv);
extern "C" int sceKernelUsleep(unsigned int microseconds);
// PS5 rejects generic libc exit()/main return with SIGSYS (12).
extern "C" [[noreturn]] void n64xy_exit_to_shell(const char* reason) {
    std::printf("[N64XY] Shell exit: %s\n",reason?reason:"no reason");
    std::fflush(nullptr);
    const int rc=sceSystemServiceLoadExec("exit",nullptr);
    std::printf("[N64XY] shell returned 0x%08x; waiting\n",unsigned(rc));
    std::fflush(nullptr);
    for(;;)sceKernelUsleep(100000);
}
extern "C" [[noreturn]] void catchReturnFromMain(int status) {
    std::printf("[N64XY] CRT returned %d\n",status);
    n64xy_exit_to_shell("CRT main returned");
}
int main() {
    sceNetInit();
    const auto access=elevation::request(elevation::Capability::filesystem);
    std::string root="/data/PS5N64";
    if(mkdir(root.c_str(),0777)!=0&&errno!=EEXIST)root="/download0/PS5N64";
    mkdir(root.c_str(),0777);
    for(const char* dir:{"games","saves","settings","covers","cache","logs"})mkdir((root+"/"+dir).c_str(),0777);
    mkdir((root+"/cache/ps5vk-shader-cache").c_str(),0777);
    setenv("PS5VK_SHADER_CACHE_DIR",(root+"/cache/ps5vk-shader-cache").c_str(),1);
    int fd=open((root+"/logs/boot.log").c_str(),O_WRONLY|O_CREAT|O_TRUNC|O_APPEND,0666);
    if(fd>=0){dup2(fd,fileno(stdout));dup2(fd,fileno(stderr));close(fd);setvbuf(stdout,nullptr,_IOLBF,4096);}
    std::printf("[boot] N64XY v1.0.8 by YAHBALLON, original PS5SX2 shelf, filesystem=%u\n",unsigned(access));
    OrbisFrontendPaths paths;paths.top_dir=root;paths.games_dir=root+"/games";paths.settings_dir=root+"/settings";
    paths.gs_ini=root+"/gs.ini";paths.covers_dir=root+"/covers";paths.cache_dir=root+"/cache";
    paths.logs_dir=root+"/logs";paths.settings_log=root+"/logs/settings.log";paths.serial_cache=root+"/cache/serials.txt";
    paths.usb_list=root+"/cache/usb-games.txt";paths.allow_download=true;paths.sound=true;
    paths.report_header="N64XY v1.0.8 by YAHBALLON";
    for(int i=0;i<8;++i){auto usb="/mnt/usb"+std::to_string(i);
        for(const char* folder:{"/homebrew/PS5N64/games","/homebrew/PS5N64/roms","/PS5N64/games","/PS5N64/roms","/homebrew/PPSA99206/games","/homebrew/PPSA99206/roms","/N64"})paths.usb_dirs.push_back(usb+folder);}
    for(const char* local:{"/data/PS5N64/games","/data/PS5N64/roms","/data/N64","/data/homebrew/PPSA99206/games","/data/homebrew/PPSA99206/roms"})paths.usb_dirs.push_back(local);
    orbis_frontend_set_language("/download0/english-language");
    orbis_web_start(paths,"v1.0.8 by YAHBALLON");
    bool ran=false;orbis_frontend_run(paths,"v1.0.8 by YAHBALLON",&ran);
    n64xy_exit_to_shell("frontend completed");
}
