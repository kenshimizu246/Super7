import React from 'react';
import './App.css';
import Range from './Range';
import ControlButton from './ControlButton';
import Distance from './Distance';
import Compass from './Compass';
import DriveButton from './DriveButton';
import Connection from './Connection';
import ServoSlider from './ServoSlider';

var msgcnt = 0;
var stop_count = 0;
var conn = new Connection('ws://' + window.location.hostname + ':9009', ['alexo']);


function onSystemEvent(msg){
  //var s = JSON.stringify(msg);
  //console.log('onSystemEvent:'+s);
}

function onMessage(msg){
  //var s = JSON.stringify(msg);
  //console.log('onMessage:'+s);
}

//conn.on('MSG', onMessage.bind(this));
//conn.on('SYS', onSystemEvent.bind(this));
//console.log("add handler");

conn.connect();


function App() {
  function handlerRangeCamY(value){
    msgcnt++;
    const message = {
      header: {
        target: 'servo',
        seq: msgcnt,
      },
      body: {
        id: 0,
        value: parseInt(value, 10),
      },
    }
    const str = JSON.stringify(message)
    conn.send(str);
  }

  function handlerRangeCamX(value){
    const message = {
      header: {
        target: 'servo',
        seq: msgcnt,
      },
      body: {
        id: 1,
        value: parseInt(value, 10),
      },
    }
    const str = JSON.stringify(message)
    conn.send(str);
  }

  function handlerDriveButton(cmd){
    const message = {
      header: {
        target: 'drive',
        seq: msgcnt,
      },
      body: {
        command: cmd,
        count: stop_count,
      },
    }
    const str = JSON.stringify(message)
    //console.log("message:"+str)
    conn.send(str);
  }

  function handlerStopCounter(value){
    stop_count = Number(value);
  }

  function handlerControlButton(value){
    //console.log("handleControlButton:value:"+value);
  }

  let controlb = ControlButton(handlerControlButton, true);
  let rangeCamX = Range(handlerRangeCamY, 125, 540, 1, 300, true);
  let rangeCamY = Range(handlerRangeCamX, 125, 450, 1, 300, true);
  let rangeStopCount = Range(handlerStopCounter, 0, 500, 1, 0, false);

  let servoSlider = ServoSlider(handlerStopCounter, 'srv01', conn, 100, 550, 1, 325, 320, 20);

  let vl53l0x_f = Distance(conn, 'vl53l0x', 0, 'VL53L0X', 'Time of Flight distance sensor');
  let headingXYZ = Compass(conn, 'gy271', 0, 'GY271', 'Three Axis Magnetic Field Sensor');
  let stop = DriveButton(handlerDriveButton, 'STOP', 'STOP', false);
  let autodrive = DriveButton(handlerDriveButton, 'AUTO', 'AUTO', false);
  let forward = DriveButton(handlerDriveButton, 'FORWARD', 'FORWARD', false);
  let backward = DriveButton(handlerDriveButton, 'BACKWARD', 'BACKWARD', false);
  let left = DriveButton(handlerDriveButton, 'LEFT', 'LEFT', false);
  let right = DriveButton(handlerDriveButton, 'RIGHT', 'RIGHT', false);
  let forward_right = DriveButton(handlerDriveButton, 'FORWARD_RIGHT', 'RIGHT', false);
  let forward_left = DriveButton(handlerDriveButton, 'FORWARD_LEFT', 'LEFT', false);
  let backward_right = DriveButton(handlerDriveButton, 'BACKWARD_RIGHT', 'RIGHT', false);
  let backward_left = DriveButton(handlerDriveButton, 'BACKWARD_LEFT', 'LEFT', false);
  let slide_right = DriveButton(handlerDriveButton, 'SLIDE_RIGHT', 'SLIDE', false);
  let slide_left = DriveButton(handlerDriveButton, 'SLIDE_LEFT', 'SLIDE', false);

  return (
    <div className="App">
     <div className="title">Ashlynn</div>
     <div className="monitorControlX">
       <div className="monitorControlY">
         <div className='monitor'>
           <img src="http://192.168.1.90:8081" alt='monitor'/>
         </div>
         <div className="rangeCamY">
           {rangeCamY}
         </div>
       </div>
       <div className="rangeCamX">
         {rangeCamX}
       </div>
     </div>
     <div className="rangeStopCountBox">
       <div className="rangeStopCount">
         {rangeStopCount}
       </div>
       <div className="rangeStopCountText">
         {stop_count}
       </div>
     </div>
     <div className='controllerRow'>
       <div>{forward_left}</div>
       <div>{forward}</div>
       <div>{forward_right}</div>
     </div>
     <div className='controllerRow'>
       <div>{left}</div>
       <div>{stop}</div>
       <div>{right}</div>
     </div>
     <div className='controllerRow'>
       <div>{slide_left}</div>
       <div>{autodrive}</div>
       <div>{slide_right}</div>
     </div>
     <div className='controllerRow'>
       <div>{backward_left}</div>
       <div>{backward}</div>
       <div>{backward_right}</div>
     </div>
     <div>
       {vl53l0x_f}
     </div>
     <div>
       {headingXYZ}
     </div>
    </div>
  );
}

export{conn};
export default App;
