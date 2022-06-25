
// If image is not working then, try just using a div and making it a small circle
const Obstacle = (props) => {
  
  return (
    <div style={{borderRadius:"50%", position:"relative", top: `${props.Alien.xcoord}px`, left: `${props.Alien.ycoord}px`, backgroundColor: `${props.color}`, height:'15px', width:'15px' }}>

    </div>
  )
}


export default Obstacle

