import './About.css'

const About = () => {
  return (
    <main>
      <div className="position-relative overflow-hidden p-5 p-md-3 m-md-3 text-center bg-light">
        <div className="col-md-5 p-lg-5 mx-auto my-5 rounded-pill jcena">
          <h1 className="display-4 fw-normal">Something about Rover</h1>
          <p className="lead fw-normal"> Welcome to Mars... etc etc</p>
          <a className="btn btn-outline-secondary" href="/">Launching soon</a>
        </div>
        <div className="product-device shadow-sm d-none d-md-block"></div>
        <div className="product-device product-device-2 shadow-sm d-none d-md-block"></div>
      </div>

      <div className="d-md-flex flex-md-equal w-100 my-md-3 ps-md-3">
        <div className="bg-dark me-md-3 pt-3 px-3 pt-md-5 px-md-5 text-center text-white overflow-hidden rounded-3 jcena">
          <div className="my-3 py-3">
            <h2 className="display-5">Team </h2>
            <p className="lead">And an even wittier subheading.</p>
          </div>
          <div className="bg-light shadow-sm mx-auto prevent-react-error"></div>
        </div>
        <div className="bg-light me-md-3 pt-3 px-3 pt-md-5 px-md-5 text-center overflow-hidden rounded-3 jcena">
          <div className="my-3 p-3">
            <h2 className="display-5">Battery</h2>
            <p className="lead">And an even wittier subheading.</p>
          </div>
          <div className="bg-dark shadow-sm mx-auto prevent-react-error"></div>
        </div>
      </div>

      <div className="d-md-flex flex-md-equal w-100 my-md-3 ps-md-3">
        <div className="bg-info me-md-3 pt-3 px-3 pt-md-5 px-md-5 text-center overflow-hidden rounded-3 jcena">
          <div className="my-3 p-3">
            <h2 className="display-5">AutoPilot</h2>
            <p className="lead">And an even wittier subheading.</p>
          </div>
          <div className="bg-dark shadow-sm mx-auto prevent-react-error"></div>
        </div>
        <div className="bg-danger me-md-3 pt-3 px-3 pt-md-5 px-md-5 text-center text-white overflow-hidden rounded-3 jcena">
          <div className="my-3 py-3">
            <h2 className="display-5">Another headline</h2>
            <p className="lead">And an even wittier subheading.</p>
          </div>
        </div>
      </div>
    </main>
  )
}

export default About