"""Write the manifest.json that ESPHome's http_request update platform reads."""
import hashlib
import json
import sys
from pathlib import Path

dist, version, repo, summary = Path(sys.argv[1]), *sys.argv[2:]
ota = dist / "firmware.ota.bin"
manifest = {
    "name": "Ensy AHU-400",
    "version": version,
    "home_assistant_domain": "esphome",
    "builds": [{
        "chipFamily": "ESP32",
        "ota": {
            "path": "firmware.ota.bin",
            "md5": hashlib.md5(ota.read_bytes()).hexdigest(),
            "summary": summary,
            "release_url": f"https://github.com/{repo}/releases/tag/v{version}",
        },
    }],
}
(dist / "manifest.json").write_text(json.dumps(manifest, indent=2))
print(json.dumps(manifest, indent=2))
