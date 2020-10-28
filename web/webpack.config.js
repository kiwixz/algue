const path = require("path");

const { CleanWebpackPlugin } = require("clean-webpack-plugin");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const VueLoaderPlugin = require("vue-loader/lib/plugin");

module.exports = {
  entry: {
    home: "./home/index.ts",
    live: "./live/index.ts",
  },
  output: {
    filename: "[name]/[contenthash].js",
    path: path.resolve(__dirname, "dist"),
  },
  optimization: {
    splitChunks: {
      chunks: "all",
    },
  },
  devServer: {
    compress: false,
    contentBase: false,
    hot: true,
    proxy: {
      "/api": {
        target: "http://localhost:4444",
        pathRewrite: { "^/api": "" },
      },
      "/live": {
        target: "http://localhost:8080",
        pathRewrite: { "^/live/.*$": "/live" },
      },
    },
  },
  plugins: [
    new CleanWebpackPlugin(),
    new VueLoaderPlugin(),
    new HtmlWebpackPlugin({
      chunks: ["home"],
      template: "common/index.html",
    }),
    new HtmlWebpackPlugin({
      chunks: ["live"],
      template: "common/index.html",
      filename: "live/index.html",
    }),
  ],
  module: {
    rules: [
      {
        test: /\.vue$/,
        use: "vue-loader",
      },
      {
        test: /\.ts$/,
        loader: "ts-loader",
        options: {
          appendTsSuffixTo: [/\.vue$/],
        },
      },
      {
        test: /\.css$/,
        use: ["style-loader", "css-loader"],
      },
      {
        test: /\.scss$/,
        use: ["style-loader", "css-loader", "sass-loader"],
      },
    ],
  },
};
