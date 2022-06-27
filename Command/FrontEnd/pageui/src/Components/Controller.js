import up from './static/up.png'
import down from './static/down.png'
import left from './static/left.png'
import right from './static/right.png'

const Controller = () => {


  const mouseLeaveControl = async (evt) => {
    evt.preventDefault();

    await fetch('http://35.176.71.115:8080/rControl', {
      method: "POST",
      headers: {
        'Content-type': "application/json"
      },
      body: JSON.stringify({'directionMove': "S"})
    });

    //await randomNumber.json();

    console.log("Mouse Down Ended: S");
    //console.log(data);
  }

  const mouseClickControl = async (evt) => {
    evt.preventDefault();

    await fetch('http://35.176.71.115:8080/rControl', {
      method: "POST",
      headers: {
        'Content-type': "application/json"
      },
      body: JSON.stringify({'directionMove': evt.target.id})
    });
    //const data = await randomNumber.json();

    console.log("Mouse Down Started: ", evt.target.id);
    //console.log(data);
  }




  return (
    <div className="d-flex flex-row justify-content-evenly align-items-center"  style={{minHeight: "93vh", maxHeight:"100vh"}} >

    <button type="button" className="btn btn-primary btn-lg" id="F" onMouseUp={mouseLeaveControl}  onMouseDown={mouseClickControl}> 
      <img src={up} alt="up" id="F" />
    </button>

    <button type="button" className="btn btn-secondary btn-lg" id="B" onMouseUp={mouseLeaveControl}  onMouseDown={mouseClickControl} >
    <img src={down} alt="up" id="B" />
    </button>

    <button type="button" className="btn btn-danger btn-lg" id="R" onMouseUp={mouseLeaveControl}  onMouseDown={mouseClickControl} >
      <img src={right} alt="up" id="R" /> 
    </button>

    <button type="button" className="btn btn-success btn-lg" id="L" onMouseUp={mouseLeaveControl}  onMouseDown={mouseClickControl} >
    <img src={left} alt="up" id="L" />
    </button>




    </div>
  )
}

export default Controller


