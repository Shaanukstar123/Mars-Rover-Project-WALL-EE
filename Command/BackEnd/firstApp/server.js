const express = require('express');
const cors = require('cors')
const bodyParser = require('body-parser')
app = express();

// MongoDB URL connection string
const dbURI = 'mongodb+srv://shaanu:<password>@cluster0.k6p9c.mongodb.net/?retryWrites=true&w=majority';

app.use(cors());
app.use(bodyParser.json())
app.use(bodyParser.urlencoded({ extended: true }))

app.get("/battery",(req,res)=>{
  let randomNumber = Math.floor(Math.random() * 100);
  res.json({percentage:randomNumber})
})

app.post("/rControl", (req, res) =>{
  console.log(req.body)
  res.json({"Received" : req.body.directionMove });
} )


app.listen(8080, () => {
  console.log('Listening on port 8080');
})