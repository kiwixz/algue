const path = require("path");
const { gzip } = require("@gfx/zopfli");

const { CleanWebpackPlugin } = require("clean-webpack-plugin");
const CompressionPlugin = require("compression-webpack-plugin");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const VueLoaderPlugin = require("vue-loader/lib/plugin");

module.exports = {
  entry: {
    home: "./home/index.ts",
    live: "./live/index.ts",
  },
  output: {
    filename: "[name].[contenthash].js",
    path: path.resolve(__dirname, "dist"),
    publicPath: "/",
  },
  optimization: {
    splitChunks: {
      chunks: "all",
    },
  },
  performance: {
    hints: false,
  },
  resolve: {
    extensions: [".ts", ".js"],
    alias: {
      Common: path.resolve(__dirname, "common"),
      Riot: path.resolve(__dirname, "riot/build/include"),
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
        pathRewrite: { "^/live/.*$": "/live.html" },
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
      filename: "live.html",
    }),
    new CompressionPlugin({
      test: /\.(html|js)$/,
      minRatio: 1.0,
      algorithm: gzip,
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
        test: /\.scss$/,
        use: ["style-loader", "css-loader", "sass-loader"],
      },
      {
        test: /\.css$/,
        use: ["style-loader", "css-loader"],
      },
      {
        test: /\.(woff2|png)$/,
        use: [
          {
            loader: "file-loader",
            options: {
              name: "[name].[contenthash].[ext]",
            },
          },
        ],
      },
    ],
  },
};
