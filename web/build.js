#!/usr/bin/env node

const path = require("path");
const { execSync } = require("child_process");

execSync(path.join("riot", "make.py"), { stdio: "inherit" });
execSync("webpack", { stdio: "inherit" });
