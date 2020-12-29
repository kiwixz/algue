module.exports = {
  env: {
    browser: true,
    es2020: true,
  },
  extends: ["plugin:vue/essential", "airbnb-base", "prettier"],
  plugins: ["vue", "prettier"],
  settings: {
    "import/resolver": "webpack",
  },
  parser: "vue-eslint-parser",
  parserOptions: {
    parser: "@typescript-eslint/parser",
    ecmaVersion: 2020,
    sourceType: "module",
    ecmaFeatures: {
      impliedStrict: true,
    },
  },
  rules: {
    "prettier/prettier": "warn",
    "camelcase": "off",
    "class-methods-use-this": "off",
    "lines-between-class-members": ["error", "always", { exceptAfterSingleLine: true }],
    "no-console": "off",
    "no-new": "off",
    "no-useless-concat": "off",
  },
};
