
// If image is not working then, try just using a div and making it a small circle
const Obstacle = (props) => {
  
  return (
    <div style={{borderRadius:"50%", position:"relative", top: `${props.Alien.xcoorda}px`, left: `${props.Alien.ycoorda}px`, backgroundColor: `${props.color}`, height:'15px', width:'15px' }}>

    </div>
  )
}

//<img src={red} alt={props.Alien.color} style={{border:"4px solid white", position:"relative", top: `${props.Alien.xcoorda}px`, left: `${props.Alien.ycoorda}px`}} />
export default Obstacle

