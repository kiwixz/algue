#!/usr/bin/env python3

import multiprocessing
import os
import subprocess


CLANG_FORMAT = os.getenv("CLANG_FORMAT", "clang-format")


def format_cpp_file(path):
    subprocess.check_call([CLANG_FORMAT, "-i", path])


def format_cpp():
    git_files = subprocess.check_output(["git", "ls-files"], text=True).splitlines()
    todo_files = [
        f
        for f in git_files
        if (f.endswith(".cpp") or f.endswith(".h")) and not f.startswith("cmake/") and os.path.exists(f)
    ]
    with multiprocessing.Pool(min(len(todo_files), os.cpu_count() + 2)) as p:
        p.map(format_cpp_file, todo_files)
    print(f"formatted {len(todo_files)} cpp files")


def format_web():
    subprocess.check_call(["npx", "prettier", "-w", "."], cwd="web", stdout=subprocess.DEVNULL, shell=True)
    subprocess.check_call(["npx", "eslint", "--fix", "."], cwd="web", stdout=subprocess.DEVNULL, shell=True)
    print("formatted web")


def main():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    format_cpp()
    format_web()


if __name__ == "__main__":
    main()
