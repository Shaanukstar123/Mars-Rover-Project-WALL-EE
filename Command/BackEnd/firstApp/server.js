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
var centralCommand = {
  mode: 0
};

var location ={
  obstacle:0,
  xcoord:0,
  ycoord:0
};

var battery = {percentage: 0};

var alien = {
  color:-1, // -1 means no new alien detected. Default value
  xcoord:0,
  ycoord:0
}

var fan = {
  is_new : 0, // 0 if variable not changed, 1 if new fan is detected
  xcoord : -1,
  ycoord : -1
};

var building = {
  is_new : 0, 
  xcoord : -1,
  ycoord : -1
};

// MongoDB Schemas and Models

const alienSchema = new mongoose.Schema({
  color : String,
  xcoord: Number,
  ycoord: Number,
});

const fanSchema = new mongoose.Schema({
  is_new: Number,
  xcoord: Number,
  ycoord: Number,
});

const buildingSchema = new mongoose.Schema({
  is_new: Number,
  xcoord: Number,
  ycoord: Number,
});

const Alien = mongoose.model("aliens", alienSchema);
const Fan = mongoose.model("fans", fanSchema);
const Building = mongoose.model("buildings", buildingSchema);


//Connection  String
let connection = "mongodb+srv://temp:marsrover@cornflakes.k6p9c.mongodb.net/sample-db?retryWrites=true&w=majority";

// Making sure app doesn't lisen before database is connected
mongoose.connect(connection, { useNewUrlParser: true, UseUnifiedTopology: true})
  .then(app.listen(8080, () => {
    console.log('Listening on port 8080');


    Alien.deleteMany({})
      .then(console.log("Deleted Aliens Collection"))
      .catch((err) => console.log(err))
    ;

    Fan.deleteMany({})
    .then(console.log("Deleted Aliens Collection"))
    .catch((err) => console.log(err))
    ;

    Building.deleteMany({})
    .then(console.log("Deleted Aliens Collection"))
    .catch((err) => console.log(err))
    ;


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
  if (topic =="fans"){
    fan = JSON.parse(message);
  }
  if (topic =="buildings"){
    building = JSON.parse(message);
  }

});

// Routes

app.get("/battery",(req,res)=>{
  res.json(battery);
});

app.get("/coordinates",(req,res)=>{ //constantly updates location coordinates of rover
  return res.json(location);
});

app.post("/rControl", (req, res) =>{
  console.log(req.body);
  console.log("THIS IS CENTRAL COMMAND: ");
  console.log(centralCommand);
  if (centralCommand.mode!=1){
    centralCommand.mode = 1;
    client.publish('centralCommand',JSON.stringify(centralCommand));
  }
    
  client.publish('rControl',JSON.stringify(req.body)); //publishes direction straight from front-end request without saving to var
  res.json({"Received" : req.body.directionMove });
})

app.get("/reset",(req,res)=>{

  Alien.deleteMany({})
    .then(console.log("Deleted Aliens Collection"))
    .catch((err) => console.log(err))
  ;
  Building.deleteMany({})
    .then(console.log("Deleted Aliens Collection"))
    .catch((err) => console.log(err))
  ;
  Fan.deleteMany({})
    .then(console.log("Deleted Aliens Collection"))
    .catch((err) => console.log(err))
  ;
  
});

 app.post("/shortestDistance", (req, res) =>{
   console.log(req.body);
   client.publish('coordinates',JSON.stringify(req.body));
   //res.json({"Received" : req.body.directionMove });
 });

app.post("/autoPilot", (req, res) =>{
  console.log(req.body);
  if (centralCommand !=3){
    centralCommand = 3;
    client.publish('centralCommand',JSON.stringify(req.body));
  }
  res.json({"Received" : req.body.directionMove });
 });

app.post("/sendShortestDistance", (req, res) =>{
  console.log(req.body);
  centralCommand = req.body;
  client.publish('centralCommand',JSON.stringify(req.body));
  res.json({"Received" : req.body.directionMove });
});

app.get("/obstacles",(req,res)=>{
  let colors = ["red", "green", "blue", "pink", "yellow", "lightgreen"];
  if (alien.color!=-1){
    const alienObj = new Alien({
      color: colors[alien.color],
      xcoord: alien.xcoord,
      ycoord: alien.ycoord
    });

    alienObj.save()
      .then( function (result){
        console.log("Updated in database")
      })
      .catch((err) => console.log(err))
    ;
    console.log("Sent to db: ",alienObj);
    alien.color = -1; //resets to null alien

  }
  
  Alien.find({}, 'color xcoord ycoord')
    .then( function (result){
      console.log("New Alien:", result)
      return res.json(result);
    })
    .catch((err) => console.log(err))
  ;
})

app.get("/fans",(req,res)=>{
  if (fan.is_new!==0){
    
    const fanObj = new Fan({
      xcoord: fan.xcoord,
      ycoord: fan.ycoord
    });

    fanObj.save()
      .then( function (result){
        console.log("Updated in database")
      })
      .catch((err) => console.log(err))
    ;
    console.log("Sent to db: ",fanObj);
    fan.is_new = 0; //resets to null fan

  }
  
  Fan.find({}, 'xcoord ycoord')
    .then( function (result){
      console.log("New Fan:", result)
      return res.json(result);
    })
    .catch((err) => console.log(err))
  ;
})

app.get("/buildings",(req,res)=>{
  //console.log(alien.color);
  if (building.is_new!==0){
    
    const buildingObj = new Building({
      xcoord: building.xcoord,
      ycoord: building.ycoord
    });

    buildingObj.save()
      .then( function (result){
        console.log("Updated in database")
      })
      .catch((err) => console.log(err))
    ;
    console.log("Sent to db: ",buildingObj);
    building.is_new = 0; //resets to null building

  }
  
  Building.find({}, 'xcoord ycoord')
    .then( function (result){
      console.log("New Building:", result)
      return res.json(result);
    })
    .catch((err) => console.log(err))
  ;
})
