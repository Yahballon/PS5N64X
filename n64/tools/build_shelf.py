"""(AI-assisted) PS5N64 out-of-tree core build, native link and signed test package."""
from pathlib import Path
import concurrent.futures,hashlib,json,os,shutil,subprocess,zipfile
project=Path(__file__).resolve().parents[2]
nes=project/'n64';out=nes/'build-shelf';out.mkdir(exist_ok=True)
core=nes/'third_party/parallel-n64';sdk=Path(os.environ['PS5_PAYLOAD_SDK'])
deps=Path(os.environ['PS5N64_DEPS']);nfs=Path(os.environ['LIBNFS_PREFIX']);z=Path(os.environ['LIBZ_PREFIX'])
native=project/'ps5/proto/native';wrapper=native/'tooling/prospero-clang18'
sources=subprocess.check_output(['make','-s','-f',str(nes/'tools/core-sources.mk'),'list'],cwd=core,text=True).splitlines()
coreflags=subprocess.check_output(['make','-s','-f',str(nes/'tools/core-sources.mk'),'flags'],cwd=core,text=True).splitlines()
incs=['-I'+str(nes/'ps5-compat'),'-I'+str(nes/'include')]
coreflags=[('-I'+str(core/flag[2:])) if flag.startswith('-I') else flag for flag in coreflags]
base=['-O2','-march=znver2','-ffunction-sections','-fdata-sections','-fno-stack-protector','-fno-plt','-fcommon']
cflags=['-target','x86_64-sie-ps5','-isysroot',str(sdk),'-isystem',str(sdk/'target/include'),'-std=gnu11',*base,*coreflags]
fingerprint=hashlib.sha256((' '.join(coreflags)).encode())
for header in sorted(core.rglob('*')):
    if header.is_file() and header.suffix in ('.h','.c','.cpp','.hpp','.inc'):
        fingerprint.update(str(header.relative_to(core)).encode());fingerprint.update(header.read_bytes())
stamp=nes/'build/core-fingerprint.txt';digest=fingerprint.hexdigest()
force_core=not stamp.exists() or stamp.read_text()!=digest
objects=[]
def compile_c(name):
    path=core/name;obj=nes/'build/obj/core'/Path(name).with_suffix('.o');obj.parent.mkdir(parents=True,exist_ok=True)
    if force_core or not obj.exists() or obj.stat().st_mtime < path.stat().st_mtime:
        if path.suffix=='.cpp':
            flags=[*base,'-fexceptions','-frtti','-std=c++11',*coreflags]
            subprocess.run(['sh',str(wrapper),*flags,'-c',str(path),'-o',str(obj)],check=True)
        else:subprocess.run(['clang-18',*cflags,'-c',str(path),'-o',str(obj)],check=True)
    return obj
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
    objects=list(pool.map(compile_c,sources))
stamp.parent.mkdir(parents=True,exist_ok=True);stamp.write_text(digest)
print(f'N64 core: {len(objects)} PS5 objects (AI-assisted)',flush=True)
cpp=[*base,'-std=c++20','-fno-exceptions','-fno-rtti','-pthread','-DORBIS_NFS=1','-I'+str(nfs/'include'),
     '-I'+str(z/'include'),'-DORBIS_VULKAN=1','-DORBIS_NES=1','-DORBIS_CURL=1','-DCURL_STATICLIB',
     '-I'+str(project/'3rdparty/vulkan/include'),'-I'+str(project/'3rdparty/include'),
     '-DFE_FONT_DIR="'+str(project/'bin/resources/fonts')+'"','-DFE_ASSET_DIR="'+str(project/'ps5/frontend/assets')+'"','-I'+str(nes/'src'),'-I'+str(project/'ps5/frontend'),
     '-I'+str(project/'ps5/coreorbis/orbis-shims/filesystem-elevation'),*incs]
cpp_sources=[nes/'src'/n for n in ['core_session.cpp','games.cpp','shelf_games.cpp','shelf_runtime.cpp','main_shelf.cpp','loading.cpp','library_cache.cpp','controller_bindings.cpp']]
cpp_sources += [project/'ps5/frontend'/f'{name}.cpp' for name in ['fe_app','fe_covers','fe_renderer','fe_sound','fe_text','fe_vk','fe_ps5','fe_web','fe_i18n','fe_settings','fe_options','fe_exact_porpoise']]
cpp_sources += [project/'ps5/frontend/fe_nfs.cpp',project/'ps5/coreorbis/orbis-shims/filesystem-elevation/src/elevation.cpp']
cpp_sources += [project/'ps5/porpoise_original/src/ui_gfx.cpp']
cpp_sources += [native/'tooling/native'/f'{name}.cpp' for name in ['app_crt','app_cpp_runtime']]
for src in cpp_sources:
    obj=out/'obj'/f'{src.stem}.o';obj.parent.mkdir(exist_ok=True)
    subprocess.run(['sh',str(wrapper),*cpp,'-c',str(src),'-o',str(obj)],check=True);objects.append(obj)

qrcode=out/'obj/qrcodegen.o'
subprocess.run(['clang-18',*cflags,'-I'+str(project/'ps5/frontend/third_party/qrcodegen'),'-c',str(project/'ps5/frontend/third_party/qrcodegen/qrcodegen.c'),'-o',str(qrcode)],check=True)
objects.append(qrcode)
vulkan=deps/'PS5_Vulkan'
mesa=Path(os.environ.get('PS5N64_MESA_UTIL_DIR',str(out/'mesa-util')))
if not all((mesa/name).exists() for name in ['u_thread.o','anon_file.o','os_file.o']):
    environment=dict(os.environ,PS5_VULKAN_DIR=str(vulkan))
    subprocess.run(['bash',str(project/'ps5/tools/build-mesa-util.sh'),str(mesa),str(sdk)],check=True,env=environment)
for name in ['u_thread.o','anon_file.o','os_file.o']:objects.append(mesa/name)
(out/'vulkan').mkdir(exist_ok=True)
vkarchive=out/'vulkan/libps5vk.ps5.a'
shutil.copy2(vulkan/'build/driver/ps5/libps5vk.ps5.a',vkarchive)
subprocess.run(['llvm-objcopy-18','--redefine-sym','vkGetInstanceProcAddr=ps5vk_driver_vkGetInstanceProcAddr',str(vkarchive)],check=True)
vklibs=[vkarchive,vulkan/'.deps/native/vulkan-runtime/lib/libvk_runtime.ps5.a',vulkan/'build/driver/ps5/libpsbc_driver.ps5.a',vulkan/'.deps/native/psbc/lib/libpsbc_support.ps5.a']
environment=dict(os.environ,PS5_VULKAN_DIR=str(vulkan),PS5N64_DEPS=str(deps))
subprocess.run(['python3',str(nes/'tools/build_generated_psbc.py')],env=environment,check=True)
generated=out/'ps5-core/generated-psbc.ps5.a'
assert generated.is_file() and generated.stat().st_size>100
vklibs.append(generated)
subprocess.run(['python3',str(nes/'tools/build_agc_imports.py')],env=environment,check=True)
agc_dir=out/'ps5-core/agc-imports'
assert all((agc_dir/x).exists() for x in ('libSceAgc.so','libSceAgcDriver.so'))
netlibs=[z/'lib'/f'lib{name}.a' for name in ['curl','psl','ssl','crypto','zstd','z']]

stubs=sorted(str(p) for p in (sdk/'target/lib').glob('*.so') if p.name not in ('libScePosixForWebKit.so','libSceGLSlimVSH.so'))
stubs.extend(str(agc_dir/x) for x in ('libSceAgc.so','libSceAgcDriver.so'))
pie=out/'llvm-pie.elf'
link=['ld.lld-18','-m','elf_x86_64','-pie','-z','max-page-size=0x4000','-mllvm','-emulated-tls',
      '--hash-style=gnu','-T',str(native/'tooling/native/ps5-pie.ld'),
      '-T',str(project/'ps5/coreorbis/orbis-shims/ehframe.ld'),'--eh-frame-hdr',
      '--version-script',str(native/'tooling/native/app-symbols.map'),'--no-dynamic-linker','-e','_start',
      '-o',str(pie),*(str(p) for p in objects),str(nfs/'lib/libnfs.a'),'--whole-archive',*(str(p) for p in vklibs),'--no-whole-archive','--start-group',*(str(p) for p in netlibs),'--end-group',
      '--as-needed',*stubs,'-L'+str(sdk/'target/lib'),
      *(str(sdk/'target/lib'/f'lib{lib}.a') for lib in ['c++','c++abi','unwind','c']),'-lpthread']
subprocess.run(link,check=True)
exports=out/'orbis-exports.map'
subprocess.run(['python3',str(project/'ps5/genmap.py'),str(pie),str(sdk/'target/lib'),str(exports)],check=True)
subprocess.run(link+['--version-script',str(exports)],check=True)
# (AI-assisted) Build the project's PS5SX2 native writer, never the driver helper:
# it emits the already-tested flexible/internal memory process parameters.
tool=out/'host/ps5-native-tool';tool.parent.mkdir(exist_ok=True)
subprocess.run(['clang++-18','-std=c++20','-O2','-o',str(tool),
                *(str(native/'tooling/native'/f'{name}.cpp') for name in
                  ['native_app_builder','elf_object','self_container','sce_module_writer']),'-lz'],check=True)
subprocess.run([str(tool),'link','--in',str(pie),'--out',str(out/'eboot.elf'),'--stub-dir',str(sdk/'target/lib'),
                '--stub',str(agc_dir/'libSceAgc.so'),'--stub',str(agc_dir/'libSceAgcDriver.so'),
                '--module-sdk','0x02000009','--companion-sdk','0x08050001','--file-name','eboot.elf'],check=True)
subprocess.run(['python3',str(nes/'tools/check_memory.py'),str(out/'eboot.elf')],check=True)
subprocess.run(['python3',str(project/'ps5/tools/prepare_console_elf.py'),'--input',str(out/'eboot.elf'),
                '--output',str(out/'eboot-console-ready.elf')],check=True)
app=out/'package/PPSA99206';(app/'sce_module').mkdir(parents=True,exist_ok=True)
subprocess.run([str(tool),'self','--sign','--in',str(out/'eboot.elf'),'--out',str(app/'eboot.bin'),'--magic','0x1D3D154F'],check=True)
subprocess.run([str(tool),'self','--inspect','--file',str(app/'eboot.bin')],check=True)
shutil.copytree(nes/'sce_sys',app/'sce_sys',dirs_exist_ok=True)
shutil.copy2(deps/'PS5_Vulkan/runtime/libc.prx',app/'sce_module/libc.prx')
baseline=os.environ.get('PS5N64_LAPY_BASELINE')
if not baseline:raise RuntimeError('PS5N64_LAPY_BASELINE missing (verified release helper)')
with zipfile.ZipFile(baseline) as archive:
    for name in ('lapy.elf','lapy-manifest.json'):
        (app/name).write_bytes(archive.read('PPSA99206/'+name))
licenses=app/'licenses';licenses.mkdir(exist_ok=True)
for path,name in [(nes/'licenses/parallel-n64-GPLv2.txt','parallel-n64-GPLv2.txt'),(project/'COPYING.GPLv3','GPLv3.txt'),
                  (nes/'licenses/Lapy-MIT.txt','Lapy-MIT.txt'),(deps/'libnfs/COPYING','libnfs.txt')]:
    if path.exists():shutil.copy2(path,licenses/name)
# Include the upstream component notices with the binaries.
for part in ['mupen64plus-core','mupen64plus-rsp-hle','mupen64plus-rsp-cxd4']:
    for notice in ['LICENSES','COPYING']:
        source=core/part/notice
        if source.exists():shutil.copy2(source,licenses/(part+'-'+notice+'.txt'))
if (nes/'licenses').exists():
    shutil.copytree(nes/'licenses',licenses,dirs_exist_ok=True)
revision_file=core/'UPSTREAM_REVISION.txt'
if revision_file.exists():commit=revision_file.read_text().strip()
else:commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=core,text=True).strip()
manifest={'ai_assisted':True,'name':'N64XY v1.0.8 by YAHBALLON','title':'PPSA99206','core':'ParaLLEl N64 (cached interpreter, Angrylion)',
          'frontend':'N64XY 3D Porpoise CoverFlow with 5 original themes','cover_index_sha256':hashlib.sha256((nes/'src/cover_urls.h').read_bytes()).hexdigest(),'cover_source':'https://thumbnails.libretro.com/Nintendo%20-%20Nintendo%2064/Named_Boxarts/',
          'frontend_source_sha256':{str(p.relative_to(project)):hashlib.sha256(p.read_bytes()).hexdigest() for p in cpp_sources if '/ps5/frontend/' in str(p)},
          'core_commit':commit,'core_patch_sha256':{str(p.relative_to(core)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [core/'libretro/libretro.c',core/'mupen64plus-core/src/main/main.c',core/'mupen64plus-core/src/device/r4300/r4300_core.c',core/'mupen64plus-core/src/device/r4300/pure_interp.c',core/'libretro-common/rthreads/retro_eventcount.c']},'core_source':'https://github.com/libretro/parallel-n64','console_tested':False,
          'files':{str(p.relative_to(app)):hashlib.sha256(p.read_bytes()).hexdigest() for p in app.rglob('*')
                   if p.is_file() and p.name != 'build-manifest.json'}}
(app/'build-manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
shutil.copy2(nes/'INSTALL.md',out/'package/INSTALL.md')
bundle=out/'N64XY-v1.0.8-by-YAHBALLON.zip'
with zipfile.ZipFile(bundle,'w',zipfile.ZIP_DEFLATED) as archive:
    for path in app.rglob('*'):
        if path.is_file():archive.write(path,Path('PPSA99206')/path.relative_to(app))
    archive.write(nes/'INSTALL.md','INSTALL.md')
print('PS5N64 test package (AI-assisted):',bundle,flush=True)
