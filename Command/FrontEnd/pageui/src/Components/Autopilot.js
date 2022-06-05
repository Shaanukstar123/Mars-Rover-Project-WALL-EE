import { useState } from 'react'
import './Autopilot.css'
import Rover from './Rover';


const Autopilot = () => {

  const [intervalId, setIntervalId] = useState(0);
  const [coords, setCoords] = useState({
    xcoord:90,
    ycoord:60
  });


  const fetchData = () => {
    
    let x = Math.floor((Math.random() * 234));
    let y = Math.floor((Math.random() * 355));
    
    let obj = {
      xcoord : x,
      ycoord : y
    };

    console.log(obj);
    setCoords(obj);
  }

  const start = (event) => {

    if(intervalId) {
      clearInterval(intervalId);
      
      event.currentTarget.classList.remove(
        'btn-danger',
      );

      event.currentTarget.classList.add(
        'btn-success',
      );

      setIntervalId(0);
      return;
    }

    const newIntervalId = setInterval(fetchData, 1500);

    event.currentTarget.classList.remove(
      'btn-success',
    );

    event.currentTarget.classList.add(
      'btn-danger',
    );

    setIntervalId(newIntervalId);
  }




  return (
    <div className="d-flex flex-row justify-content-evenly align-items-center" style={{minHeight: "93vh", maxHeight:"100vh", border:"4px solid purple"}}>
      <div style={{border:"4px solid purple", height:"70vh", width:"45vw"}} id="johnCena">
        <Rover  Coordinates={coords}  />
      </div>

      Autopilot
      <button type="button" className="btn btn-success" onClick={start} > {intervalId ? "STOP" : "Let's Explore with Dora"} </button>
    </div>
  )
}

export default Autopilot