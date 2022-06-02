/*const app = require('express')();
const appWs = require('express-ws')(app); //install both of these using npm first

app.ws('/echo', ws=>{
    ws.on('message',msg =>{
        console.log("Received from rover: ",msg); //receive message from client
        ws.send(msg); //send same message back to client
    });
});

app.listen(3000, () => console.log("Server started"));*/

const express = require('express');
const ws = require('ws');

const app = express();

// Set up a headless websocket server that prints any
// events that come in.
const wsServer = new ws.Server({ noServer: true });
wsServer.on('connection', socket => {
  socket.on('message', message => console.log(message));
});

// `server` is a vanilla Node.js HTTP server, so use
// the same ws upgrade process described here:
// https://www.npmjs.com/package/ws#multiple-servers-sharing-a-single-https-server
const server = app.listen(3000);
server.on('upgrade', (request, socket, head) => {
  wsServer.handleUpgrade(request, socket, head, socket => {
    wsServer.emit('connection', socket, request);
  });
});
