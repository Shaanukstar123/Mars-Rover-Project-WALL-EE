const express = require('express')
const app = express()

app.set("view engine", "ejs")
app.get('/',(req,res)=>{
    console.log("main screen")
    res.send("WALL-EE Advanced Cybernetic Mars Rover")
})

const locationRouter = require("./routes/location")
const batteryRouter = require("./routes/battery")

app.use("/location",locationRouter)
app.use("/battery",batteryRouter)


app.listen(5555)