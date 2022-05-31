const express = require('express');
const cors = require('cors')
app = express();

app.use(cors())

app.get("/battery",(req,res)=>{
  let randomNumber = Math.floor(Math.random() * 100);
  res.json({percentage:randomNumber})
})


app.listen(8080, () => {
  console.log('Listening on port 8080');
})