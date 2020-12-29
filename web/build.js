#!/usr/bin/env node

const path = require("path"); // eslint-disable-line import/no-extraneous-dependencies
const { execSync } = require("child_process");
const { mkdirSync, writeFileSync } = require("fs");

const include_dir = "build/include";
mkdirSync(include_dir, { recursive: true });
const sha = execSync("git rev-parse HEAD").toString().trimEnd();
writeFileSync(`${include_dir}/info.ts`, "export default {\n" + `  version: "${sha}",\n` + "};\n");

execSync(path.join("riot", "make.py"), { stdio: "inherit" });
execSync("webpack  --mode=production", { stdio: "inherit" });
