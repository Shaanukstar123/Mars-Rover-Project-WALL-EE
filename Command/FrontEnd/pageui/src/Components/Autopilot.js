import { useState } from 'react'
import './Autopilot.css'
import Rover from './Rover';
import AddObstacles from './AddObstacles';


const Autopilot = () => {

  const [aliens, setAliens] = useState([]);

  const [intervalId, setIntervalId] = useState(0);

  const [coords, setCoords] = useState({
    xcoord:190,
    ycoord:90
  });


  const fetchObstacleData = async () => {
    try{

      const request = await fetch('http://35.176.71.115:8080/obstacles');
      const alienObj = await request.json();

    
      //console.log("Alien location: " , alienObj);
      setAliens((aliens) => [...aliens, alienObj]);
    }
    
    catch(err){
      console.log(err);
    }

  
  };

  const fetchCoordinateData = async () => {
    try{
      console.log('fetching..');

      const request = await fetch('http://35.176.71.115:8080/coordinates');
      const obj = await request.json();

      console.log("object is:", obj);

      setCoords(obj)
  

      if(obj.obstacle === 1){
        fetchObstacleData();
      }

      //console.log("object is:", obj);
    }

    catch(err){
      console.log(err);
    }
  
  };

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

  
    const newIntervalId = setInterval(fetchCoordinateData, 400);

    event.currentTarget.classList.remove(
      'btn-success',
    );

    event.currentTarget.classList.add(
      'btn-danger',
    );

    setIntervalId(newIntervalId);
  }

  const reset = () => {
    setAliens([]);
  }




  return (
    <div className="d-flex flex-row justify-content-evenly align-items-center" style={{minHeight: "93vh", maxHeight:"100vh", border:"4px solid orange"}}>
      <div style={{border:"4px solid purple", height:"70vh", width:"45vw"}} id="johnCena">
        <Rover  Coordinates={coords}  />
        <AddObstacles Aliens={aliens} />
      </div>

      Autopilot
      <button type="button" className="btn btn-success" onClick={start} > {intervalId ? "STOP" : "Let's Explore with Dora"} </button>
      <button type="button" className="btn btn-info" onClick={reset} > Reset </button>
    </div>
  )
}


export default Autopilot