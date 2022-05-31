

const Controller = () => {


  const formControl = (evt) => {
    evt.preventDefault();
    console.log(evt.target.id);
  }

  const mouseControl = (evt) => {
    evt.preventDefault();
    console.log("Mouse Down Started: ", evt.target.id);
  }




  return (
    <div className="d-flex flex-row justify-content-evenly align-items-center"  style={{minHeight: "93vh", maxHeight:"100vh"}} >

    <button type="button" className="btn btn-primary btn-lg" id="F" onClick={formControl}  onMouseDown={mouseControl}> Forward </button>
    <button type="button" className="btn btn-secondary btn-lg" id="B" onClick={formControl} > Back </button>
    <button type="button" className="btn btn-danger btn-lg" id="R" onClick={formControl} > Right </button>
    <button type="button" className="btn btn-success btn-lg" id="L" onClick={formControl} > Left </button>



    </div>
  )
}

export default Controller


/* <form className="d-flex flex-column justify-content-evenly align-items-center"  method='post' style={{minHeight: "53vh", maxHeight:"60vh", border:"2px solid orange"}} onSubmit={formControl} >
      <button type="submit" className="btn btn-primary btn-lg" id="Forward">Forward </button>
      <button type="submit" className="btn btn-secondary btn-lg"> Reverse </button>
      <button type="submit" className="btn btn-danger btn-lg"> Right </button>
      <button type="submit" className="btn btn-success btn-lg"> Left </button>
</form> */