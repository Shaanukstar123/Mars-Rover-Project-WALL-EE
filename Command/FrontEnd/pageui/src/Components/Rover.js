import rover from './static/rover.png'

const Rover = (props) => {


  return (
    <>
      <img src={rover} alt="circle" style={{position:"relative", top: `${props.Coordinates.xcoord*2}px`, left: `${props.Coordinates.ycoord*2}px`}} /> 
    </>

  )
}

export default Rover
 
