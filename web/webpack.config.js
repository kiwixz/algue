const path = require("path");

const { CleanWebpackPlugin } = require("clean-webpack-plugin");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const VueLoaderPlugin = require("vue-loader/lib/plugin");

module.exports = {
  entry: {
    home: "./home/index.js",
    live: "./live/index.js",
  },
  output: {
    filename: "[name]/[contenthash].js",
    path: path.resolve(__dirname, "dist"),
  },
  devServer: {
    compress: false,
    contentBase: false,
    hot: true,
  },
  plugins: [
    new CleanWebpackPlugin(),
    new VueLoaderPlugin(),
    new HtmlWebpackPlugin({
      template: "common/index.html",
      chunks: ["home"],
    }),
    new HtmlWebpackPlugin({
      template: "common/index.html",
      filename: "live/index.html",
      chunks: ["live"],
    }),
  ],
  module: {
    rules: [
      {
        test: /\.vue$/,
        use: "vue-loader",
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
