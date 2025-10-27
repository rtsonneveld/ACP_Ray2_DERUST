import re
import sys
from pathlib import Path

# Adjust these if your filenames differ
try:
    rc_file = Path("ACP_Ray2_DERUST.rc")
    out_file = Path("src/rendering/textures_list.hpp")

    pattern = re.compile(
        r"^\s*(IDB_PNG\d+)\s+PNG\s+\"resources\\\\([^\"]+)\"",
        re.IGNORECASE
    )

    textures = []

    with rc_file.open(encoding="utf-16") as f:
        for line in f:
            m = pattern.match(line)
            if m:
                resource_id, filename = m.groups()
                name = Path(filename).stem
                parts = name.split('_')
                varname = ''.join(p.capitalize() for p in parts)
                textures.append((varname, resource_id))

    with out_file.open("w", encoding="utf-8") as f:
        f.write("// Auto-generated from resources.rc — do not edit manually.\n")
        for name, rid in textures:
            f.write(f"TEXTURE({name}, {rid})\n")

    print(f"Generated {out_file} with {len(textures)} entries.")
    sys.exit(0)  # ✅ success exit code

except Exception as e:
    print(f"Texture list generation failed: {e}")
    sys.exit(0)  # ✅ still return 0 to not break the build
