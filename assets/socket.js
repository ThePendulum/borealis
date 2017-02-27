'use strict';

import store from './store';

let ws;

if(window.location.hostname) {
    ws = new WebSocket('ws://' + window.location.hostname + ':81');

    ws.addEventListener('message', message => {
        const data = JSON.parse(message.data);

        Object.keys(data).forEach(key => {
            store.commit(key, data[key]);
        });
    });
}

const socket = {};

socket.transmit = function(property, value) {
    const msg = {};
    msg[property] = value;

    if(ws) {
        ws.send(JSON.stringify(msg));
    }
};

export default socket;
