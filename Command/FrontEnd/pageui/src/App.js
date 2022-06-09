import './App.css';
import Navbar from './Components/Navbar';
import About from './Components/About';
import Battery from './Components/Battery';
import Controller from './Components/Controller';
import Autopilot from './Components/Autopilot';
import { BrowserRouter as Router, Route, Routes } from 'react-router-dom'

function App() {
  return (
  <Router> 

    <body>
      <Navbar />

      <Routes>
        <Route path='/' element={<About /> } />

        <Route path='/battery' element={<Battery /> } />

        <Route path='/touristmode' element={<Controller /> } />

        <Route path='/autopilot' element={<Autopilot /> } />

      </Routes>

    </body>

  </Router>
  );
}

export default App;
