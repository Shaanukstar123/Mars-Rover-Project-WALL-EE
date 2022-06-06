import { useState } from 'react'
import red from './static/red.png'
import blue from './static/blue.png'
import pink from './static/pink.png'

// If image is not working then, try just using a div and making it a small circle
const Obstacle = (props) => {
  const [image, setImage] = useState(['./static/blue.png', './static/red.png', './static/pink.png']);
  return (
    <div>
      <img src={require(image[0])} alt={props.Alien.color} style={{border:"4px solid white", position:"relative", top: `${props.Alien.xcoorda}px`, left: `${props.Alien.ycoorda}px`}} /> 
    </div>
  )
}

export default Obstacle