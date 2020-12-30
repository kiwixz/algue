const path = require("path");

const CompressionPlugin = require("compression-webpack-plugin");
const HtmlPlugin = require("html-webpack-plugin");
const VueLoaderPlugin = require("vue-loader/lib/plugin");
const { CleanWebpackPlugin } = require("clean-webpack-plugin");
const { gzip } = require("@gfx/zopfli");

module.exports = (_, argv) => {
  const dev = argv.mode === "development";

  return {
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
        Generated: path.resolve(__dirname, "build/include"),
        Riot: path.resolve(__dirname, "build/riot/include"),
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
      dev ? false : new CleanWebpackPlugin(),
      new VueLoaderPlugin(),
      new HtmlPlugin({
        chunks: ["home"],
        template: "common/index.html",
      }),
      new HtmlPlugin({
        chunks: ["live"],
        template: "common/index.html",
        filename: "live.html",
      }),
      dev
        ? false
        : new CompressionPlugin({
            test: /\.(html|js)$/,
            minRatio: 1.0,
            algorithm: gzip,
          }),
    ].filter(Boolean),
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
          test: /\.png$/i,
          use: [
            {
              loader: "file-loader",
              options: {
                name: "i/[name].[contenthash].webp",
              },
            },
            {
              loader: "image-webpack-loader",
              options: {
                disable: dev,

                gifsicle: { enabled: false },
                mozjpeg: { enabled: false },
                optipng: { enabled: false },
                pngquant: { enabled: false },
                svgo: { enabled: false },
                webp: {
                  lossless: true,
                  method: 6,
                  quality: 100,
                },
              },
            },
          ],
        },
        {
          test: /\.woff2$/,
          use: [
            {
              loader: "file-loader",
              options: {
                name: "a/[name].[contenthash].[ext]",
              },
            },
          ],
        },
      ],
    },
  };
};
