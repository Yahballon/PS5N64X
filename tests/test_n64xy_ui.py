#!/usr/bin/env python3
from pathlib import Path
import json
root=Path(__file__).resolve().parents[1]
app=json.loads((root/'app.json').read_text())
param=json.loads((root/'n64/sce_sys/param.json').read_text())
assert app['name']=='N64XY' and app['version']=='1.0.8'
assert app['titleId']=='PPSA99206' and param['titleId']=='PPSA99206'
assert param['localizedParameters']['en-US']['titleName']=='N64XY'
urls=(root/'n64/src/cover_urls.h').read_text()
assert 'thumbnails.libretro.com/Nintendo%20-%20Nintendo%2064/Named_Boxarts/' in urls
assert 'Globaleliteee/Gaming_Console_Covers_Art_Repository' in urls
assert 'libretro-thumbnails/Nintendo_-_Nintendo_64/' in urls
ui=(root/'ps5/frontend/fe_exact_porpoise.cpp').read_text()
options=(root/'ps5/frontend/fe_options.cpp').read_text()
for theme in ('quantum-lime','nebula-purple','retro-red','ice-crystal','golden-era'):
    assert theme in ui and theme in options
assert 'N64XYCaseSize(' in ui
assert (root/'ps5/porpoise_original/src/ui_gfx.cpp').exists()
assert (root/'ps5/frontend/assets/n64xy-mark.png').exists()
loading=(root/'n64/src/loading.cpp').read_text()
assert 'ORBITAL GATEWAY' in loading and 'n64xy_build::kVersion' in loading
assert all(x in loading for x in ('progress.games','progress.entries','progress.directory','progress.sources_done'))
runtime=(root/'n64/src/shelf_runtime.cpp').read_text()
assert 'parallel-n64-gfxplugin' in runtime and 'angrylion' in runtime
assert 'frame.game_set=game_sets[texture_slot]' in runtime
assert 'frame.game_vertices=6' in runtime
assert 'sceSystemServiceLoadExec("exit",nullptr)' in (root/'n64/src/main_shelf.cpp').read_text()
print('PASS N64XY preserved ParaLLEl core, ROM progress, themed CoverFlow and N64 Boxart sources')
