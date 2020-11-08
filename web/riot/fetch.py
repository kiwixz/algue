#!/usr/bin/env python3

import json
import os
import shutil
import urllib.request
from pathlib import Path


def get_json(url):
    res = urllib.request.urlopen(url)
    if res.status != 200:
        raise RuntimeError(f"http error {res.status} ({res.reason})")
    return json.load(res)


def main():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    include = Path("include")
    include.mkdir(exist_ok=True)

    shutil.copy("teams.ts", include)

    versions = get_json("https://ddragon.leagueoflegends.com/api/versions.json")
    latest = versions[0]
    print(f"lastest version: {latest}")

    champions = get_json(f"https://ddragon.leagueoflegends.com/cdn/{latest}/data/en_US/champion.json")
    


if __name__ == "__main__":
    main()
