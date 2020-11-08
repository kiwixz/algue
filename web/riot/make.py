#!/usr/bin/env python3

import json
import os
import shutil
import urllib.request
from pathlib import Path
from tarfile import TarFile


def main():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    build = Path("build")
    include = build / "include"
    include.mkdir(parents=True, exist_ok=True)

    shutil.copy("teams.ts", include)

    versions_res = urllib.request.urlopen("https://ddragon.leagueoflegends.com/api/versions.json")
    if versions_res.status != 200:
        raise RuntimeError(f"http error {versions_res.status} ({versions_res.reason})")
    latest = json.load(versions_res)[0]
    print(f"lastest version: {latest}")

    ddragon = build / f"ddragon-{latest}"
    if not ddragon.exists():
        ddragon_tgz = build / f"ddragon-{latest}.tgz"
        if not ddragon_tgz.exists():
            print("downloading ddragon")
            with open(ddragon_tgz, "wb") as f:
                shutil.copyfileobj(
                    urllib.request.urlopen(f"https://ddragon.leagueoflegends.com/cdn/dragontail-{latest}.tgz"), f
                )

        print("extracting ddragon")
        ddragon_tar = TarFile.open(ddragon_tgz, "r:gz")
        if any(ddragon not in (ddragon / n).parents for n in ddragon_tar.getnames()):
            raise RuntimeError("suspicious path in ddragon archive")
        ddragon_tar.extractall(ddragon)

    ddragon_data = ddragon / latest / "data/en_US"
    ddragon_img = ddragon / latest / "img"

    with open(ddragon_data / "champion.json", encoding="utf-8") as f:
        champions = json.load(f)
    with open(include / "champions.ts", "w", newline="\n") as f:
        for c in champions["data"]:
            f.write(
                f'import {c}Img from "../' + (ddragon_img / "champion" / c).relative_to(build).as_posix() + '.png";\n'
            )
        f.write("\nexport default {\n")
        for c in champions["data"].values():
            f.write(c["key"] + ': { name: "' + c["name"] + '", image: ' + c["id"] + "Img },\n")
        f.write("}\n")


if __name__ == "__main__":
    main()
