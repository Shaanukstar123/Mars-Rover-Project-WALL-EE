import './About.css'
import { useState, useEffect } from "react";


import Rover from './Rover';
import AddObstacles from './AddObstacles';
import one from './static/1.png'
import two from './static/2.png'
import three from './static/3.png'
import up from './static/up.png'
import down from './static/down.png'
import left from './static/left.png'
import right from './static/right.png'


// Core modules imports are same as usual
import { Navigation, Pagination} from 'swiper';

import { Swiper, SwiperSlide } from 'swiper/react';

import 'swiper/scss'
import 'swiper/scss/navigation'
import 'swiper/scss/pagination'



const About = () => {


  const [inputs, setInputs] = useState({});

  const [aliens, setAliens] = useState([]);

  const [intervalId, setIntervalId] = useState(0);

  const [coords, setCoords] = useState({
    xcoord:0,
    ycoord:0
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

  
    const newIntervalId = setInterval(fetchCoordinateData, 800);

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

  const handleChange = (event) => {
    const name = event.target.name;
    const value = event.target.value;
    setInputs(values => ({...values, [name]: parseInt(value)}))
  }

  const hello = async (evt) => {
    evt.preventDefault();
    //console.log(inputs);

    await fetch('http://35.176.71.115:8080/shortestDistance', {
      method: "POST",
      headers: {
        'Content-type': "application/json"
      },
      body: JSON.stringify(inputs)
    });


    console.log("CoordData Sent");
  }

  const shortestDistance = async (evt) => {
    evt.preventDefault();

    await fetch('http://35.176.71.115:8080/sendShortestDistance', {
      method: "POST",
      headers: {
        'Content-type': "application/json"
      },
      body: JSON.stringify({'move': 2})
    });

    //console.log("Data Sent");
  }

  return (
    <main className="w-screen h-screen flex flex-row justify-center bg-gradient-to-tr from-pink-500 via-red-500 to-yellow-500">

   
    <div className="grid grid-cols-3 gap-8 rounded-sm mt-10 h-5/6 w-11/12">

        <div className="flex flex-col justify-between items-center h-full col-span-2 row-span-2 rounded-lg bg-white" >

          <h1 className="text-3xl font-extrabold flex flex-row justify-start w-full">
            <span className=""> AutoPilot </span>
          </h1>

          <div className="h-5/6 w-4/6 relative border-2 border-black border-dashed" >
            <Rover  Coordinates={coords}  />
            <AddObstacles Aliens={aliens} />
          </div>
          
          <div className="h-4 w-4">

          </div>

    
        </div>



        <div className="h-full rounded-lg  bg-gradient-to-bl from-indigo-200 via-red-200 to-yellow-100 flex flex-row justify-center items-center hover:drop-shadow-2xl">

          <Swiper pagination={{ type: "fraction"}} navigation={true} modules={[Pagination, Navigation]} className="mySwiper h-full w-full" >

            <SwiperSlide className="flex flex-row justify-center">
              
              <div className="h-auto w-10/12 flex flex-col justify-between items-center">

                <h1 className="text-3xl font-extrabold flex flex-col content-center justify-center w-full">
                  <span className="hover:animate-pulse"> TouristMode </span>
                </h1>
                
                <div className="h-auto w-6/12 grid grid-cols-3 grid-rows-3">
                  <button type="button" className="btn btn-primary  col-start-2" id="F" onMouseUp={endclicket}  onMouseDown={startclicket}> F </button>
                  <button type="button" className="btn btn-secondary  col-start-2 row-start-3" id="B" onMouseUp={endclicket}  onMouseDown={startclicket} > B </button>
                  <button type="button" className="btn btn-danger  col-start-3 row-start-2" id="R" onMouseUp={endclicket}  onMouseDown={startclicket} > R </button>
                  <button type="button" className="btn btn-success row-start-2" id="L" onMouseUp={endclicket}  onMouseDown={startclicket} > L </button>
                </div>

                <div className='h-12'>

                </div>

              </div>

            </SwiperSlide>


            <SwiperSlide className="flex flex-row justify-center">
              
              <div className="h-auto w-10/12 flex flex-col justify-between items-center">

                <h1 className="text-3xl font-extrabold flex flex-col content-center justify-center w-full">
                  <span className="hover:animate-pulse"> VenueMode </span>
                </h1>
                
                <form className="flex flex-row justify-evenly flex-wrap h-2/6" onSubmit={hello}>
        
                  <div className="relative z-0 w-2/6 mb-1.5 group">
                    <input type="number" name="xcoord" value={inputs.xcoord || ""} onChange={handleChange}  className="block py-2.5 px-0 w-full text-sm  bg-transparent border-b-2 border-teal-300 appearance-none  focus:outline-none focus:ring-0 focus:border-blue-600 peer" placeholder=" " required />
                    <label className="peer-focus:font-medium absolute text-sm text-gray-500  duration-300 transform -translate-y-6 scale-75 top-3 -z-10 origin-[0] peer-focus:left-0 peer-focus:text-blue-600 peer-focus:dark:text-blue-500 peer-placeholder-shown:scale-100 peer-placeholder-shown:translate-y-0 peer-focus:scale-75 peer-focus:-translate-y-6">X</label>
                  </div>

                  <div className="relative z-0 w-2/6 mb-1.5 group">
                    <input type="number" name="ycoord" value={inputs.ycoord || ""} onChange={handleChange}  id="floating_password" className="block py-2.5 px-0 w-full text-sm text-gray-900 bg-transparent border-b-2 border-red-300 appearance-none focus:outline-none focus:ring-0 focus:border-blue-600 peer" placeholder=" " required />
                    <label  className="peer-focus:font-medium absolute text-sm text-gray-500 duration-300 transform -translate-y-6 scale-75 top-3 -z-10 origin-[0] peer-focus:left-0 peer-focus:text-blue-600 peer-focus:dark:text-blue-500 peer-placeholder-shown:scale-100 peer-placeholder-shown:translate-y-0 peer-focus:scale-75 peer-focus:-translate-y-6">Y</label>
                  </div>

                  <button type="submit" className="text-white bg-gradient-to-r from-indigo-500 via-purple-500 to-pink-500 hover:bg-blue-800 focus:ring-4 focus:outline-none focus:ring-blue-300 font-medium rounded-lg text-med w-auto  px-5 py-2 text-center">Submit</button>
                </form>

                <div className='h-12'>
                  <button onClick={shortestDistance} className="text-white bg-gradient-to-r from-indigo-500 via-purple-500 to-pink-500 hover:bg-blue-800 focus:ring-4 focus:outline-none focus:ring-blue-300 font-medium rounded-lg text-med w-auto  px-4 py-2 text-center"> Send </button>
                </div>

              </div>

            </SwiperSlide>


            <SwiperSlide className="flex flex-row justify-center">
              
              <div className="h-auto w-10/12 flex flex-col justify-between items-center">

                <h1 className="text-3xl font-extrabold flex flex-col content-center justify-center w-full">
                  <span className="hover:animate-pulse"> AutoPilot </span>
                </h1>
                
                <div className="h-auto w-7/12 flex flex-row justify-evenly items-center">
                  <button className="bg-sky-500 hover:bg-sky-700 px-2 py-2 text-sm rounded-lg font-semibold text-white" onClick={start}>
                    {intervalId ? "STOP" : "AutoPilot"}
                  </button>

                  <button className="bg-fuchsia-500 hover:bg-fuchsia-700 px-2 py-2 text-sm rounded-lg font-semibold text-white" onClick={reset}>
                    Reset
                  </button>
                </div>

                <div className="h-10 w-1 relative">

                </div>

              </div>

            </SwiperSlide>

          </Swiper>
             
        </div>




        <div className="h-full rounded-lg bg-purple-400 flex flex-col justify-between items-center hover:bg-gradient-to-bl from-green-400 to-blue-500 hover:scale-110">

          <h1 className="text-3xl font-extrabold flex flex-row justify-start  w-11/12">
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






