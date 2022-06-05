import rover from './static/rover.png'

const Rover = (props) => {

  const joe = () =>{

    console.log(`${props.coordinates.xcoord}px`);
    console.log(`${props.coordinates.ycoord}px`);

  }

  return (
    <img src={rover} alt="circle" style={{border:"4px solid white", position:"relative", top: `${props.Coordinates.xcoord}px`, left: `${props.Coordinates.ycoord}px`}} onClick={joe} /> 
  )
}

export default Rover
