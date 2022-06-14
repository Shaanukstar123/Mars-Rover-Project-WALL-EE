const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const mongoose = require('mongoose');
app = express();

const mqtt = require("mqtt");
var client = mqtt.connect('mqtt://35.176.71.115');
var location ={
  x:0,
  y:0,
  objectDetected:false
};

// MongoDB URL connection string
const dbURI = 'mongodb+srv://shaanu:<password>@cluster0.k6p9c.mongodb.net/?retryWrites=true&w=majority';

app.use(cors());
app.use(bodyParser.json())
app.use(bodyParser.urlencoded({ extended: true }))

//Connection  String
// let connection = 'mongodb+srv://shaanu:<psswd>@cornflakes.k6p9c.mongodb.net/sample-db?retryWrites=true&w=majority';
// mongoose.connect(connection, { useNewUrlParse: true, UseUnifiedTopology: true})
//   .then(app.listen(8080, () => {
//     console.log('Listening on port 8080');
//   }))
//   .catch((err) => console.log(err));


client.on('connect', function() { //MQTT subscribe
  client.subscribe("location");
  console.log("Subscribed Successfully")

});

app.get("/battery",(req,res)=>{
  let randomNumber = Math.floor(Math.random() * 100);
  res.json({percentage:randomNumber})
})

app.post("/rControl", (req, res) =>{
  console.log(req.body)
  res.json({"Received" : req.body.directionMove });
} )

app.get("/coordinates",(req,res)=>{
  //let x = Math.floor((Math.random() * 234));
  //let y = Math.floor((Math.random() * 355));
  //let z = Math.floor((Math.random() * 2));
  client.on('message', function(topic,message){
    console.log(message.toString());
    location = JSON.parse(message); //updates global location JSON
  });

  console.log("Server Coords:", location);
  res.json(location)
})

app.get("/obstacles",(req,res)=>{
  let colors = ['red', 'green', 'blue', 'pink'];

  let x2 = Math.floor((Math.random() * 234));
  let y2 = Math.floor((Math.random() * 355));
  let colorRandom = colors[ Math.floor(Math.random() * colors.length)]

  let alienObj = {
    color: colorRandom,
    xcoorda : x2,
    ycoorda : y2,
  };

  console.log("Server alienObj:", alienObj);
  res.json(alienObj);

})

app.listen(8080, () => {
  console.log('Listening on port 8080');
})