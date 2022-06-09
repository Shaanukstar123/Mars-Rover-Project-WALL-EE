import Obstacle from "./Obstacle"

const AddObstacles = ({Aliens}) => {
  return (

    <>
      {Aliens.map((obj, index) => (
        <Obstacle key={index} color={obj.color} Alien={obj}  />
      ))}
    </>
   
  )
}

export default AddObstacles