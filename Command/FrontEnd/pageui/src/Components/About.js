import './About.css'

const About = () => {

  const clickZ = () => {
    alert('hi');
    console.log('hi');
  };

  return (
    <main className="w-screen h-screen">

   
    <div className="grid grid-cols-2 gap-10 rounded-sm mt-10 h-5/6">

        <div className="col-span-2 h-full rounded-lg bg-slate-200 hover:shadow-2xl  transition ease-in-out delay-100 flex flex-row justify-center" onClick={clickZ} >
          <h1 className="text-4xl font-extrabold lg:text-6xl flex flex-col justify-center">
            <span className="border-2 border-green-100"> AutoPilot </span>
          </h1>
        </div>

        <div className="h-full rounded-lg bg-slate-100 hover:bg-gradient-to-br from-sky-500 to-indigo-500 hover:scale-110 flex flex-row justify-center transition ease-in-out delay-100 ">
          <h1 className="text-4xl font-extrabold lg:text-6xl flex flex-col justify-center">
            <span className="border-2 border-green-100"> TouristMode </span>
          </h1>
        </div>


        <div className="h-full rounded-lg bg-slate-100 hover:scale-110 flex flex-row justify-center transition ease-in-out delay-100  hover:bg-gradient-to-tr from-green-400 to-blue-500">
          <h1 className="text-4xl font-extrabold lg:text-6xl flex flex-col justify-center">
            <span className="border-2 border-green-100"> Battery </span>
          </h1>
        </div>


      </div>

      
    </main>
  )
}

export default About



// export default function Modal() {
//   const [showModal, setShowModal] = React.useState(false);

//   return (
//     <>
//       <button
//         classNameName="bg-pink-500 text-white active:bg-pink-600 font-bold uppercase text-sm px-6 py-3 rounded shadow hover:shadow-lg outline-none focus:outline-none mr-1 mb-1 ease-linear transition-all duration-150"
//         type="button"
//         onClick={() => setShowModal(true)}
//       >
//         Open regular modal
//       </button>
//       {showModal ? (
//         <>
//           <div
//             classNameName="justify-center items-center flex overflow-x-hidden overflow-y-auto fixed inset-0 z-50 outline-none focus:outline-none"
//           >
//             <div classNameName="relative w-auto my-6 mx-auto max-w-3xl">
//               {/*content*/}
//               <div classNameName="border-0 rounded-lg shadow-lg relative flex flex-col w-full bg-white outline-none focus:outline-none">
//                 {/*header*/}
//                 <div classNameName="flex items-start justify-between p-5 border-b border-solid border-slate-200 rounded-t">
//                   <h3 classNameName="text-3xl font-semibold">
//                     Modal Title
//                   </h3>
//                   <button
//                     classNameName="p-1 ml-auto bg-transparent border-0 text-black opacity-5 float-right text-3xl leading-none font-semibold outline-none focus:outline-none"
//                     onClick={() => setShowModal(false)}
//                   >
//                     <span classNameName="bg-transparent text-black opacity-5 h-6 w-6 text-2xl block outline-none focus:outline-none">
//                       ×
//                     </span>
//                   </button>
//                 </div>
//                 {/*body*/}
//                 <div classNameName="relative p-6 flex-auto">
//                   <p classNameName="my-4 text-slate-500 text-lg leading-relaxed">
//                     I always felt like I could do anything. That’s the main
//                     thing people are controlled by! Thoughts- their perception
//                     of themselves! They're slowed down by their perception of
//                     themselves. If you're taught you can’t do anything, you
//                     won’t do anything. I was taught I could do everything.
//                   </p>
//                 </div>
//                 {/*footer*/}
//                 <div classNameName="flex items-center justify-end p-6 border-t border-solid border-slate-200 rounded-b">
//                   <button
//                     classNameName="text-red-500 background-transparent font-bold uppercase px-6 py-2 text-sm outline-none focus:outline-none mr-1 mb-1 ease-linear transition-all duration-150"
//                     type="button"
//                     onClick={() => setShowModal(false)}
//                   >
//                     Close
//                   </button>
//                   <button
//                     classNameName="bg-emerald-500 text-white active:bg-emerald-600 font-bold uppercase text-sm px-6 py-3 rounded shadow hover:shadow-lg outline-none focus:outline-none mr-1 mb-1 ease-linear transition-all duration-150"
//                     type="button"
//                     onClick={() => setShowModal(false)}
//                   >
//                     Save Changes
//                   </button>
//                 </div>
//               </div>
//             </div>
//           </div>
//           <div classNameName="opacity-25 fixed inset-0 z-40 bg-black"></div>
//         </>
//       ) : null}
//     </>
//   );
// }