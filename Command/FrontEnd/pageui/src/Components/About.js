import './About.css'
import { useState, useEffect } from "react";
import Rover from './Rover';
import AddObstacles from './AddObstacles';
import one from './static/1.png'
import two from './static/2.png'
import three from './static/3.png'


const About = () => {


  const [aliens, setAliens] = useState([]);

  const [intervalId, setIntervalId] = useState(0);

  const [coords, setCoords] = useState({
    xcoord:590,
    ycoord:90
  });

  const [batteryLevel, setBatteryLevel] = useState();

  const [imgSrc, setImgSrc] = useState('');


  const fetchObstacleData = async () => {
    try{

      const request = await fetch('http://35.176.71.115:8080/obstacles');
      const alienObj = await request.json();

    
      console.log("Alien location: " , alienObj);
      setAliens(alienObj);
      //setAliens((aliens) => [...aliens, alienObj]);

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

      setCoords(obj)
  
      if(obj.obstacle === 1){
        fetchObstacleData();
      }

      console.log("object is:", obj);
    }

    catch(err){
      console.log(err);
    }
  
  };

  const start = (event) => {

    console.log("start called");
    if(intervalId) {
      clearInterval(intervalId);
      
      event.currentTarget.classList.remove(
        'animate-pulse',
      );

      // event.currentTarget.classList.add(
      //   'hover:blur-sm',
      // );
    
      setIntervalId(0);
      return;
    }

  
    const newIntervalId = setInterval(fetchCoordinateData, 500);

    // event.currentTarget.classList.remove(
    //   'hover:blur-sm',
    // );

    event.currentTarget.classList.add(
      'animate-pulse',
    );

    setIntervalId(newIntervalId);
  }

  const reset = () => {
    setAliens([]);
  }

  const startclicket = (evt) =>{
    start(evt);
    mouseClickControl(evt);
  }

  const endclicket = (evt) =>{
    start(evt);
    mouseLeaveControl(evt);
  }

  const mouseLeaveControl = async (evt) => {
    evt.preventDefault();
   
    await fetch('http://35.176.71.115:8080/rControl', {
      method: "POST",
      headers: {
        'Content-type': "application/json"
      },
      body: JSON.stringify({'directionMove': "S"})
    });

    //const data = await randomNumber.json();

    console.log("Mouse Down Ended: S");
    //console.log(data);
  }

  const mouseClickControl = async (evt) => {
    evt.preventDefault();


    await fetch('http://35.176.71.115:8080/rControl', {
      method: "POST",
      headers: {
        'Content-type': "application/json"
      },
      body: JSON.stringify({'directionMove': evt.target.id})
    });
    //const data = await randomNumber.json();

    console.log("Mouse Down Started: ", evt.target.id);
    //console.log(data);
  }


  const whenClicked = async () => { 
    try{
      console.log("batteryclick");

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
    <main className="w-screen h-screen flex flex-row justify-center bg-apple">

   
    <div className="grid grid-cols-3 gap-8 rounded-sm mt-10 h-5/6 w-11/12">

        <div className="flex flex-col justify-between items-center h-full col-span-2 row-span-2 rounded-lg bg-white hover:animate-spin" >

          <h1 className="text-3xl font-extrabold flex flex-row justify-start w-full">
            <span className=""> AutoPilot </span>
          </h1>

          <div className="h-5/6 w-4/6 relative border-dashed border-2 border-orange-500 bg-cyan-500">
            <Rover  Coordinates={coords}  />
            <AddObstacles Aliens={aliens} />
          </div>
          
          <div className="h-4 w-4">

          </div>

    
        </div>



        <div className="h-full rounded-lg bg-white flex flex-col justify-between">

          <h1 className="text-3xl font-extrabold flex flex-col content-center justify-center">
            <span className="hover:animate-pulse"> TouristMode </span>
          </h1>
          
          <div className="h-auto w-auto flex flex-row justify-around">
            <button type="button" className="btn btn-primary btn-lg " id="F" onMouseUp={endclicket}  onMouseDown={startclicket}> F </button>
            <button type="button" className="btn btn-secondary btn-lg" id="B" onMouseUp={endclicket}  onMouseDown={startclicket} > B </button>
            <button type="button" className="btn btn-danger btn-lg" id="R" onMouseUp={endclicket}  onMouseDown={startclicket} > R </button>
            <button type="button" className="btn btn-success btn-lg" id="L" onMouseUp={endclicket}  onMouseDown={startclicket} > L </button>
          </div>
          

          <div className="h-auto w-auto flex flex-row justify-evenly items-center">
            <button className="bg-sky-500 hover:bg-sky-700 px-2 py-2 text-sm rounded-lg font-semibold text-white" onClick={start}>
              {intervalId ? "STOP" : "AutoPilot"}
            </button>

            <button className="bg-fuchsia-500 hover:bg-fuchsia-700 px-2 py-2 text-sm rounded-lg font-semibold text-white" onClick={reset}>
              Reset
            </button>
          </div>

          <div className="h-1 w-1 relative">

          </div>
          
  
        </div>




        <div className="h-full rounded-lg bg-black flex flex-col justify-between items-center hover:bg-gradient-to-bl from-green-400 to-blue-500 hover:scale-110">

          <h1 className="text-5xl font-extrabold flex flex-row justify-start  w-11/12">
            <span className="text-white"> Battery </span>
          </h1>

          <div className="d-flex flex-row justify-content-evenly align-items-center w-10/12">

            <div className="p-2 text-3xl text-white" > {`${batteryLevel} %`}  </div>
            <img src={imgSrc} className="App-logo" alt="logo" />


            <button className="bg-sky-500 hover:bg-sky-700 px-2 py-2 text-sm rounded-lg font-semibold text-white" onClick={whenClicked}>
              Check Health
            </button>

          </div>

          <div className="h-3 w-3">

          </div>

        </div>
      

      </div>

      
    </main>
  )
}

export default About






