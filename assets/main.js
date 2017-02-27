'use strict';

import './style.scss';

import Vue from 'vue';
import App from './app.vue';
import store from './store';

new Vue({
    el: '#container',
    store,
    render(createElement) {
        return createElement(App);
    }
});
