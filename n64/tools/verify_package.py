#!/usr/bin/env python3
"""(AI-assisted) Fail release build if PS5 installer package is incomplete."""
from pathlib import Path
import hashlib
import json
import zipfile

root = Path(__file__).resolve().parents[2]
package = root / "n64/build-shelf/N64XY-v1.0.9-by-YAHBALLON.zip"
required = {
    "PPSA99206/eboot.bin",
    "PPSA99206/sce_sys/param.json",
    "PPSA99206/sce_sys/icon0.png",
    "PPSA99206/sce_module/libc.prx",
    "PPSA99206/lapy.elf",
    "PPSA99206/lapy-manifest.json",
    "PPSA99206/build-manifest.json",
}
with zipfile.ZipFile(package) as z:
    bad = z.testzip()
    assert bad is None, f"Bad ZIP entry: {bad}"
    names = set(z.namelist())
    assert required <= names, f"Missing installer files: {sorted(required - names)}"
    assert any(x.startswith("PPSA99206/licenses/") for x in names)
    param = json.loads(z.read("PPSA99206/sce_sys/param.json"))
    assert param["titleId"] == "PPSA99206"
    assert param["localizedParameters"]["en-US"]["titleName"] == "N64XY"
    manifest = json.loads(z.read("PPSA99206/build-manifest.json"))
    assert manifest["title"] == "PPSA99206"
    for name, expected in manifest["files"].items():
        actual = hashlib.sha256(z.read("PPSA99206/" + name)).hexdigest()
        assert actual == expected, f"SHA mismatch: {name}"
print("PASS (AI-assisted): signed native app package layout, ZIP CRC, metadata and manifest file hashes")
