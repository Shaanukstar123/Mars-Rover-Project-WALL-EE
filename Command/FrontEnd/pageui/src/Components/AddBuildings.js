import React from 'react'

const AddBuildings = ({Buildings}) => {
  return (
    <>
      {Buildings.map((obj, index) => (
        <Buildings key={index} Building={obj}  />
      ))}
    </>
  )
}

export default AddBuildings