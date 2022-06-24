import rover from './static/rover.png'

const Rover = (props) => {


  return (
    <>
      <img src={rover} alt="circle" style={{position:"relative", top: `${props.Coordinates.xcoord}px`, left: `${props.Coordinates.ycoord}px`}} /> 
    </>

  )
}

export default Rover
 
