
// If image is not working then, try just using a div and making it a small circle
const Obstacle = (props) => {
  
  return (
    <div className="flex flex-col justify-start items-center" style={{borderRadius:"50%", position:"relative", top: `${props.Alien.xcoord*2}px`, left: `${props.Alien.ycoord*2}px`, backgroundColor: `${props.color}`, height:'15px', width:'15px', color:"black" }}>

      <span className="text-xs pt-3"> {props.Alien.xcoord},{props.Alien.ycoord}  </span>
    </div>
  )
}


export default Obstacle

