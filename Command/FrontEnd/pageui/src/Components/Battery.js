import { useState, useEffect } from "react";

import one from './1.png'
import two from './2.png'
import three from './3.png'


const Battery = () => {

  const [batteryLevel, setBatteryLevel] = useState();
  const [imgSrc, setImgSrc] = useState('');

  const whenClicked = async () => { 
    const randomNumber = await fetch('http://localhost:8080/battery');
    const data = await randomNumber.json();
    setBatteryLevel(data.percentage);
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
    console.log('hi');
    setImgSrc(effectz());
  }, [batteryLevel]);


  return (

    <div className="d-flex flex-row justify-content-evenly align-items-center" style={{minHeight: "93vh", maxHeight:"100vh"}} >
      <div className="p-2 mr-5" style={{ border:"4px solid green"}} > {`${batteryLevel} %`}  </div>
      <img src={imgSrc} className="App-logo" alt="logo" />
     <button type="button" className="btn btn-primary btn-lg" onClick={whenClicked} > Check Health </button>
    </div>

  )
}

export default Battery

// .pulsingButton {
//   width: 220px;
//   text-align: center;
//   white-space: nowrap;
//   display: block;
//   margin: 50px auto;
//   padding: 10px;
//   box-shadow: 0 0 0 0 rgba(232, 76, 61, 0.7);
//   border-radius: 10px;
//   background-color: #FF0000;
//   -webkit-animation: pulsing 1.25s infinite cubic-bezier(0.66, 0, 0, 1);
//   -moz-animation: pulsing 1.25s infinite cubic-bezier(0.66, 0, 0, 1);
//   -ms-animation: pulsing 1.25s infinite cubic-bezier(0.66, 0, 0, 1);
//   animation: pulsing 1.25s infinite cubic-bezier(0.66, 0, 0, 1);
//   font-size: 22px;
//   font-weight: normal;
//   font-family: sans-serif;
//   text-decoration: none !important;
//   color: #ffffff;
//   transition: all 300ms ease-in-out;
// }


// /* Comment-out to have the button continue to pulse on mouseover */

// a.pulsingButton:hover {
//   -webkit-animation: none;
//   -moz-animation: none;
//   -ms-animation: none;
//   animation: none;
//   color: #ffffff;
// }


// /* Animation */

// @-webkit-keyframes pulsing {
//   to {
//     box-shadow: 0 0 0 30px rgba(232, 76, 61, 0);
//   }
// }

// @-moz-keyframes pulsing {
//   to {
//     box-shadow: 0 0 0 30px rgba(232, 76, 61, 0);
//   }
// }

// @-ms-keyframes pulsing {
//   to {
//     box-shadow: 0 0 0 30px rgba(232, 76, 61, 0);
//   }
// }

// @keyframes pulsing {
//   to {
//     box-shadow: 0 0 0 30px rgba(232, 76, 61, 0);
//   }
// }