'use strict';

const webpack = require('webpack');
const path = require('path');
const ExtractTextPlugin = require('extract-text-webpack-plugin');
const cssnext = require('postcss-cssnext');

const config = {
    entry: {
        main: path.join(__dirname, 'assets/main.js')
    },
    output: {
        filename: 'data/bundle.js'
    },
    plugins: [
        new ExtractTextPlugin('data/style.css'),
        new webpack.LoaderOptionsPlugin({
            minimize: true,
            debug: true
        }),
        new webpack.DefinePlugin({
            'process.env': {
                NODE_ENV: '"production"'
            }
        })
    ],
    devtool: 'cheap-source-map',
    module: {
        rules: [{
            test: /\.vue$/,
            exclude: /node_modules/,
            loader: 'vue-loader',
            options: {
                loaders: {
                    css: ExtractTextPlugin.extract({
                        use: 'css-loader',
                        fallback: 'vue-style-loader'
                    })
                }
            }
        }, {
            test: /\.json$/,
            exclude: /node_modules/,
            loader: 'json'
        }, {
            test: /\.js$/,
            exclude: /node_modules/,
            loader: 'babel-loader',
            query: {
                presets: ['es2015-native-modules', 'stage-3'],
                plugins: ['transform-object-rest-spread']
            },
        }, {
            test: /\.scss$/,
            loader: ExtractTextPlugin.extract({loader: 'css-loader!sass-loader'})
        }, {
            test: /\.(woff(2)?|ttf|eot)$/,
            loader: 'url-loader?name=public/fonts/[name].[ext]'
        }, {
            test: /\.svg/,
            loader: 'raw-loader'
        }]
    },
    resolve: {
        alias: {
            config: path.join(__dirname, 'assets/js/config.js'),
            'vue$': 'vue/dist/vue.common.js'
        }
    }
};

module.exports = config;
