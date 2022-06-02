import rover from './rover.png'
import './Autopilot.css'

const Autopilot = () => {
  return (
    <div className="d-flex flex-row justify-content-evenly align-items-center" style={{minHeight: "93vh", maxHeight:"100vh", border:"4px solid purple"}}>
      <div style={{border:"4px solid purple", height:"70vh", width:"45vw"}}>
      <img src={rover} className="App-logo" alt="circle" />
      </div>
      Autopilot
    </div>
  )
}

export default Autopilot