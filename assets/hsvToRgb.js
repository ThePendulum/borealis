'use strict';

const hsvToRgb = function(h, s, v){
    let r, g, b;

    h = Math.abs(h % 360) / 360;
    s = Math.max(0, Math.min(1, s));
    v = Math.max(0, Math.min(1, v));

    const i = Math.floor(h * 6);
    const f = h * 6 - i;
    const p = v * (1 - s);
    const q = v * (1 - f * s);
    const t = v * (1 - (1 - f) * s);

    switch(i % 6){
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }

    return {
        r: r * 255,
        g: g * 255,
        b: b * 255
    };
};

const hsvToHex = function(h, s, v) {
    const rgb = hsvToRgb(h, s, v);

    return '#' + ('0' + Math.round(rgb.r).toString(16)).slice(-2) + ('0' + Math.round(rgb.g).toString(16)).slice(-2) + ('0' + Math.round(rgb.b).toString(16)).slice(-2);
}

export {hsvToRgb, hsvToHex};
