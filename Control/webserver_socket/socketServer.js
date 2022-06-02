const app = require('express')();
const appWs = require('express-ws')(app); //install both of these using npm first

app.ws('/echo', ws=>{
    ws.on('message',msg =>{
        console.log("Received from rover: ",msg); //receive message from client
        ws.send(msg); //send same message back to client
    });
});

app.listen(3000, () => console.log("Server started"));
