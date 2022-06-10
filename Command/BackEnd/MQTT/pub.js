const mqtt = require("mqtt");
var client = mqtt.connect('mqtt://35.176.71.115');

client.on("connect",function(){
    setInterval(function(){
        var random = Math.random()* 50;
        console.log(random); //random value to publish (until I get some actual data)

        //topic name is 'epic'

        client.publish('epic',"MarsRover: "+random.toString()+'.'); //publishing to topic test

    }), 30000; //30 seconds
});