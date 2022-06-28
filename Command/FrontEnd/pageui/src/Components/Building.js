import React from 'react'

const Building = (props) => {
  return (
    <div className="flex flex-col justify-start items-center" style={{borderRadius:"3%", position:"relative", top: `${props.Building.xcoord}px`, left: `${props.Building.ycoord}px`, backgroundColor: "black", height:'10px', width:'15px', color:"black" }}>
      <span className="text-xs pt-3"> {props.Building.xcoord},{props.Building.ycoord}  </span>
    </div>
  )
}

export default Building