'use strict';

import socket from '../socket.js';

export default {
    hue(context, hue) {
        context.commit('hue', hue);
        socket.transmit('hue', hue);
    },
    hueMode(context, hueMode) {
        context.commit('hueMode', hueMode);
        socket.transmit('hueMode', hueMode);
    },
    hueSpeed(context, hueSpeed) {
        context.commit('hueSpeed', hueSpeed);
        socket.transmit('hueSpeed', hueSpeed);
    },
    saturation(context, saturation) {
        context.commit('saturation', saturation);
        socket.transmit('saturation', saturation);
    },
    saturationMode(context, saturationMode) {
        context.commit('saturationMode', saturationMode);
        socket.transmit('saturationMode', saturationMode);
    },
    saturationSpeed(context, saturationSpeed) {
        context.commit('saturationSpeed', saturationSpeed);
        socket.transmit('saturationSpeed', saturationSpeed);
    },
    value(context, value) {
        context.commit('value', value);
        socket.transmit('value', value);
    },
    valueMode(context, valueMode) {
        context.commit('valueMode', valueMode);
        socket.transmit('valueMode', valueMode);
    },
    valueSpeed(context, valueSpeed) {
        context.commit('valueSpeed', valueSpeed);
        socket.transmit('valueSpeed', valueSpeed);
    }
};
