const express = require('express')
const router = express.Router()

router.get("/",(req,res)=>{
    console.log("Here")
    res.json({"coordinates":[100,100]})
})

module.exports = router
