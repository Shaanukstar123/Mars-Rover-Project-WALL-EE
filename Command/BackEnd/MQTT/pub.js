const mqtt = require("mqtt");
var client = mqtt.connect('mqtt://35.176.71.115');
var i = 0;

client.on("connect",function(){
    setInterval(function(){
        i = i+1;

        var random = Math.random()* 50;
        random = i;
        console.log(random); //random value to publish (until I get some actual data)

        //topic name is 'epic'

        client.publish('epic',"MarsRover: "+random.toString()+'.'); //publishing to topic test

    }), 30000; //30 seconds
});