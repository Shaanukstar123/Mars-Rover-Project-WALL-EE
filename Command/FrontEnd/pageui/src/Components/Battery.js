import { useState } from "react";
import logo from './logo192.png'

const Battery = () => {
  const [batteryLevel, setBatteryLevel] = useState(0);


  const whenClicked = () =>{ 
    console.log(batteryLevel);
    return setBatteryLevel(Math.floor(Math.random() * 100));
  };

  return (

    <div className="d-flex flex-row justify-content-evenly align-items-center" style={{minHeight: "93vh", maxHeight:"100vh"}} >
      <div className="p-2 mr-5" style={{ border:"4px solid green"}} > {`${batteryLevel} %`}  </div>
      <img src={logo} className="App-logo" alt="logo" />
     <button type="button" className="btn btn-primary btn-lg" onClick={whenClicked} > Check Health </button>
    </div>

  )
}

export default Battery

