const mqtt = require("mqtt");
var client = mqtt.connect('mqtt://35.176.71.115');

client.on('connect', function() {
    client.subscribe("test");
    console.log("Subscribed Successfully")

});
    client.on('message', function(topic,message){
        console.log(message.toString());

    });