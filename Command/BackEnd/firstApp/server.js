const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const mongoose = require('mongoose');
app = express();

app.use(cors());
app.use(bodyParser.json())
app.use(bodyParser.urlencoded({ extended: true }))

//Connection  String
let connection = 'mongodb+srv://shaanu:<psswd>@cornflakes.k6p9c.mongodb.net/sample-db?retryWrites=true&w=majority';
mongoose.connect(connection, { useNewUrlParse: true, UseUnifiedTopology: true})
  .then(app.listen(8080, () => {
    console.log('Listening on port 8080');
  }))
  .catch((err) => console.log(err));

app.get("/battery",(req,res)=>{
  let randomNumber = Math.floor(Math.random() * 100);
  res.json({percentage:randomNumber})
})

app.post("/rControl", (req, res) =>{
  console.log(req.body)
  res.json({"Received" : req.body.directionMove });
} )



// app.listen(8080, () => {
//   console.log('Listening on port 8080');
// })