import { useState, useEffect } from "react";

import one from './static/1.png'
import two from './static/2.png'
import three from './static/3.png'


const Battery = () => {

  const [batteryLevel, setBatteryLevel] = useState();
  const [imgSrc, setImgSrc] = useState('');

  const whenClicked = async () => { 
    try{
      const randomNumber = await fetch('http://35.176.71.115:8080/battery');
      const data = await randomNumber.json();
      setBatteryLevel(data.percentage);
    }
    catch(err){
      console.log(err);
    }


  };

  const effectz = () => {
    let logo = one;
    
    if(batteryLevel >= 70) {
      logo = three;
    }

    else if(batteryLevel >= 40){
      logo = two;
    }

    return logo;
  };

  // Same principle as below hook but this is called when page is loaded first time
  useEffect(() => {
    whenClicked();
  }, []);

  // This function is called immediately after the render of setBatteryLevel
  //  so it contains the latest batteryLevel and then it re-renders the image based on new level
  // but React is so fast that you don't notice the delay between the 2 renderings.
  useEffect(function() {
    setImgSrc(effectz());
  }, [batteryLevel]);




  return (

    <div className="d-flex flex-row justify-content-evenly align-items-center" style={{minHeight: "93vh", maxHeight:"100vh"}} >
      <div className="p-2 mr-5" style={{ border:"4px solid green"}} > {`${batteryLevel} %`}  </div>
      <img src={imgSrc} className="App-logo" alt="logo" />
     <button type="button" className="btn btn-primary btn-lg pulsing-button" onClick={whenClicked} > Check Health </button>
    </div>

  )
}

export default Battery
