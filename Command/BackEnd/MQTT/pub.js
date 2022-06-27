const mqtt = require("mqtt");
var client = mqtt.connect('mqtt://35.176.71.115');
var i = 0;
var coordinates ={
    xcoord: -5,
    ycoord: 0
};
var centralCommand ={
    mode: 2
};
var location = {
    x: 100,
    y: 200,
    objectDetected: false
};
var direction = {directionMove: 'F'};
var battery = {battery: 0};
var z;
var alien = {
    color: 2,
    xcoord:0,
    ycoord:0
  };

var fan = {
    is_new: 1,
    xcoord: 0,
    ycoord:0
} 

var building = {
    is_new: 1,
    xcoord: 0,
    ycoord:0
}
// client.publish('centralCommand',JSON.stringify(centralCommand));

// client.publish('rControl',JSON.stringify(direction));
centralCommand.mode = 1;
//client.publish('centralCommand',JSON.stringify(centralCommand));
// client.publish('rControl',JSON.stringify(direction));
// centralCommand.mode = 3;*/
client.publish('centralCommand',JSON.stringify(centralCommand));
client.publish('coordinates',JSON.stringify(coordinates));
client.publish('aliens',JSON.stringify(alien));

client.on("connect",function(){
    setInterval(function(){
        let z = Math.floor((Math.random() * 2));
        i = i+1;
        location = {
            xcoord:i,
            ycoord:i+10,
            obstacle: z
        };
        alien.color = -1;
        alien.xcoord = i;
        alien.ycoord = i;

        building.xcoord = i+50;
        building.ycoord = i+25;

        fan.xcoord = i+50;
        fan.ycoord = i+25;
        

        var random = Math.random()* 50;
        //setTimeout(() => {}, 1000);
        battery = {percentage: i};

        //console.log(location); //random value to publish (until I get some actual data)
        //console.log(direction);
        //console.log(battery);
        //console.log(alien);
        client.publish('location',JSON.stringify(location)); //publishing to topic test
        client.publish('battery',JSON.stringify(battery));
        //client.publish('aliens',JSON.stringify(alien));
        client.publish('fans',JSON.stringify(fan));
        client.publish('buildings',JSON.stringify(building));
        client.publish('centralCommand',JSON.stringify(centralCommand));
        client.publish('coordinates',JSON.stringify(coordinates));

    },1000); //1 second interval between pubs
});
