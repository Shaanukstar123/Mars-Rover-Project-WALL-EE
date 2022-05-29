const express = require("express")
const router  = express.Router()

router.get("/",(req,res)=>{
    res.json({"Health":"Optimal"})
})

module.exports = router
