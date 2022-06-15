const mqtt = require("mqtt");
var client = mqtt.connect('mqtt://35.176.71.115');
var i = 0;
var location = {
    x: 100,
    y: 200,
    objectDetected: false
};
var direction = {direction: 1};
var battery = {battery: 0};

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
        battery = {battery: i};

        console.log(location); //random value to publish (until I get some actual data)
        console.log(direction);
        console.log(battery);

        client.publish('Control',JSON.stringify(direction));
        client.publish('location',JSON.stringify(location)); //publishing to topic test
        client.publish('battery',JSON.stringify(battery));

    },1000); //1 second interval between pubs
});
