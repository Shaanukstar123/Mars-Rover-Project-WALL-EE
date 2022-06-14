const mqtt = require("mqtt");
var client = mqtt.connect('mqtt://35.176.71.115');
var i = 0;
var location = {
    x: 100,
    y: 200,
    objectDetected: false
};

client.on("connect",function(){
    setInterval(function(){
        i = i+1;
        location = {
            x:i,
            y:i+10,
            objectDetected: false
        };

        var random = Math.random()* 50;
        //setTimeout(() => {}, 1000);
        console.log(location); //random value to publish (until I get some actual data)
        

        client.publish('location',JSON.stringify(location)); //publishing to topic test

    },1000); //1 second interval between pubs
});