const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const mongoose = require('mongoose');
app = express();

const mqtt = require("mqtt");
var client = mqtt.connect('mqtt://35.176.71.115');

app.use(cors());
app.use(bodyParser.json())
app.use(bodyParser.urlencoded({ extended: true }))

//JSON variables
var location ={
  xcoord:0,
  ycoord:0,
  obstacle:0
};
var battery = {percentage: 0};
var direction = {direction: 0};

var alien = {
  color:-1, // -1 means no new alien detected
  xcoord:0,
  ycoord:0
}


// MongoDB Schemas and Models
const roverSchema = new mongoose.Schema({
  id: Number,
  xcoord: Number,
  ycoord: Number,
  obstacle: Number 
});

const Rover = mongoose.model('coordinates', roverSchema);

const batterySchema = new mongoose.Schema({
  id: Number,
  percentage: Number,
});

const Battery = mongoose.model("batterylevels", batterySchema);

const alienSchema = new mongoose.Schema({
  color: String,
  xcoord: Number,
  ycoord: Number,
});

const Alien = mongoose.model("obstacles", alienSchema);



//Connection  String
let connection = "mongodb+srv://temp:marsrover@cornflakes.k6p9c.mongodb.net/sample-db?retryWrites=true&w=majority";

// Making sure app doesn't lisen before database is connected
mongoose.connect(connection, { useNewUrlParser: true, UseUnifiedTopology: true})
  .then(app.listen(8080, () => {
    console.log('Listening on port 8080');

    Battery.deleteMany({})
      .then(console.log("Deleted Battery Collection"))
      .catch((err) => console.log(err))
    ;


    Rover.deleteMany({})
      .then(console.log("Deleted Rover Collection"))
      .catch((err) => console.log(err))
    ;

    Alien.deleteMany({})
      .then(console.log("Deleted Aliens Collection"))
      .catch((err) => console.log(err))
    ;

    new Battery({
      id: 732,
      percentage: 999
    }).save();

    new Rover({
      id: 732,
      xcoord: 250,
      ycoord: 250,
      obstacle: 1
    }).save();


  }))

  .catch((err) => console.log(err))
;


//MQTT callback functions

client.on('connect', function() { //MQTT subscribe
  client.subscribe("#");
  console.log("Subscribed Successfully")
});

client.on('message', function(topic,message){
  if (topic =="battery"){
    battery = JSON.parse(message);
  }
  if (topic =="location"){
    location = JSON.parse(message); //updates global JSON variables
  }
  if (topic =="aliens"){
    alien = JSON.parse(message);
  }
  
});



// Routes
app.get("/batteryMQTT",(req,res)=>{
  let randomNumber = Math.floor(Math.random() * 100);

  const filter = { id: 732 };
  const update = { percentage: randomNumber };

  Battery.findOneAndUpdate(filter, update, {returnOriginal: false})
    .then((obj) => res.json(obj))
    .catch((err) => console.log(err))
  ;

});

app.get("/battery",(req,res)=>{
  const filter = { id: 732 };
  const update = battery; //sets update to global battery var

  Battery.findOneAndUpdate(filter, update, {returnOriginal: false})
    .catch((err) => console.log(err))
  ;
  
  Battery.findOne({ id: 732 }, 'percentage')
    .then( function (result){
      console.log("New data:", result);
      return res.json(result);
    })
    .catch((err) => console.log(err))
    
  ;


  // const battery = new Battery({
  //   percentage: randomNumber,
  // });

  // battery.save()
  //   .then((obj) => res.json(obj))
  //   .catch((err) => console.log(err));
  


  //res.json({percentage:randomNumber})
});



app.post("/rControl", (req, res) =>{
  console.log(req.body);
  client.publish('direction',JSON.stringify(req.body));
  res.json({"Received" : req.body.directionMove });
} )



app.get("/coordinates",(req,res)=>{
  const filter = { id: 732 };
  const update = location;

  Rover.findOneAndUpdate(filter, update, {returnOriginal: false})
    .catch((err) => console.log(err))
  ;

 
  Rover.findOne({ id: 732 }, 'xcoord ycoord obstacle')
    .then( function (result){
      console.log("New data:", result);
      return res.json(result);
    })
    .catch((err) => console.log(err))
    
  ;

  //console.log(position);
  //res.json(position);

  // const rover = new Rover({
  //   xcoord: x,
  //   ycoord: y,
  //   obstacle: z
  // });

  // rover.save()
  //   .then((obj) => res.json(obj))
  //   .catch((err) => console.log(err));
  
  // let obj = {
  //   xcoord : x,
  //   ycoord : y,
  //   obstacle: z
  // };

  //console.log("Server Coords:", obj);
  
});


app.get("/obstacles",(req,res)=>{
  let colors = ['red', 'green', 'blue', 'pink'];
  console.log(alien.color);
  if (alien.color!= -1){
    
    const alienObj = new Alien({
      color: alien.color,
      xcoord: alien.xcoord,
      ycoord: alien.ycoord
    });
    alienObj.save()
      .catch((err) => console.log(err))
    ;
    alien.color = -1; //resets to null alien

  }
  


  Alien.find({}, 'color xcoord ycoord')
    .then( function (result){
      console.log("New Alien:", result)
      return res.json(result);
    })
    .catch((err) => console.log(err))
  ;

  // let alienObj = {
  //   color: colorRandom,
  //   xcoorda : x2,
  //   ycoorda : y2,
  // };

  //console.log("Server alienObj:", alienObj);

})

// app.listen(8080, () => {
//   console.log('Listening on port 8080');
// })