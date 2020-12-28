#!/usr/bin/env python3

import json
import os
import shutil
import urllib.request
from pathlib import Path
from tarfile import TarFile


def get_json_file(path):
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def get_json_url(url):
    res = urllib.request.urlopen(url)
    if res.status != 200:
        raise RuntimeError(f"http error {res.status} ({res.reason})")
    return json.load(res)


def main():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    build = Path("../build/riot")
    include = build / "include"
    include.mkdir(parents=True, exist_ok=True)

    shutil.copy("teams.ts", include)

    versions = get_json_url("https://ddragon.leagueoflegends.com/api/versions.json")
    latest = versions[0]
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

    print("making includes")
    with open(include / "info.ts", "w", newline="\n") as f:
        f.write("export default {\n" + f'  version: "{latest}",\n' + "};\n")

    ddragon_data = ddragon / latest / "data/en_US"
    ddragon_img = ddragon / latest / "img"

    champions = get_json_file(ddragon_data / "champion.json")
    with open(include / "champions.ts", "w", newline="\n") as f:
        for c in champions["data"]:
            f.write(
                f'import {c}Img from "../' + (ddragon_img / "champion" / c).relative_to(build).as_posix() + '.png";\n'
            )
        f.write("\nexport default {\n")
        for c in champions["data"].values():
            f.write("  " + c["key"] + ': { name: "' + c["name"] + '", image: ' + c["id"] + "Img },\n")
        f.write("};\n")

    queues = get_json_url("http://static.developer.riotgames.com/docs/lol/queues.json")
    with open(include / "queues.ts", "w", newline="\n") as f:
        f.write("export default {\n")
        for q in queues:
            name = "Custom" if q["queueId"] == 0 else q["description"]
            if name.endswith(" games"):
                name = name[: -len(" games")]
            f.write("  " + str(q["queueId"]) + ': { name: "' + name + '", map: "' + q["map"] + '" },\n')
        f.write("};\n")


if __name__ == "__main__":
    main()
