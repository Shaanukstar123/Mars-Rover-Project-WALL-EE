import rover from './static/rover.png'

const Rover = (props) => {


  return (
    <div>
      {props.Coordinates.xcoord}px {props.Coordinates.ycoord}px
    </div>

  )
}

export default Rover

//<img src={rover} alt="circle" style={{border:"4px solid white", position:"relative", top: `${props.Coordinates.xcoord}px`, left: `${props.Coordinates.ycoord}px`}} onClick={joe} /> 
