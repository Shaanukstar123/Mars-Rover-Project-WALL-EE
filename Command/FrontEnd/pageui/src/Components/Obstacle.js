import React from 'react'

const Obstacle = (props) => {
  return (
    <div>
      {props.Alien.xcoorda}px {props.Alien.ycoorda}px
    </div>
  )
}

export default Obstacle