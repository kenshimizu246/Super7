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
  function handlerDrive(cmd){
    msgcnt++;
    const message = {
      header: {
        target: 'drive',
        seq: msgcnt,
      },
      body: {
        command: cmd,
      },
    }
    const str = JSON.stringify(message)
    conn.send(str);
  }

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
      },
    }
    const str = JSON.stringify(message)
    //console.log("message:"+str)
    conn.send(str);
  }

  function handlerRangeSensor(value){
    const message = {
      header: {
        target: 'servo',
        seq: msgcnt,
      },
      body: {
        id: 2,
        value: parseInt(value, 10),
      },
    }
    const str = JSON.stringify(message)
    conn.send(str);
  }

  function handlerControlButton(value){
    //console.log("handleControlButton:value:"+value);
  }

  let controlb = ControlButton(handlerControlButton, true);
  let rangeCamX = Range(handlerRangeCamY, 125, 540, 1, 300, true);
  let rangeCamY = Range(handlerRangeCamX, 125, 450, 1, 300, true);
  let rangeSens = Range(handlerRangeSensor, 100, 550, 1, 300, false);

  let servoSlider = ServoSlider(handlerRangeSensor, 'srv01', conn, 100, 550, 1, 325, 320, 20);

  let vl53l0x = Distance(conn, 'vl53l0x', 0, 'VL53L0X', 'Time of Flight distance sensor');
  let hcsr04 = Distance(conn, 'hcsr04', 0, 'HCSR04', 'Ultrasonic Sensor');
  let headingXYZ = Compass(conn, 'gy271', 0, 'GY271', 'Three Axis Magnetic Field Sensor');
  let stop = DriveButton(handlerDriveButton, 'STOP', 'STOP', false);
  let forward = DriveButton(handlerDriveButton, 'FORWARD', 'FORWARD', false);
  let backward = DriveButton(handlerDriveButton, 'BACKWARD', 'BACKWARD', false);
  let left = DriveButton(handlerDriveButton, 'LEFT', 'LEFT', false);
  let right = DriveButton(handlerDriveButton, 'RIGHT', 'RIGHT', false);

  return (
    <div className="App">
     <div className="title">Ashlynn</div>
     <div className="monitorControlX">
       <div className="monitorControlY">
         <div className='monitor'>
           <img src="http://192.168.1.68:8081" alt='monitor'/>
         </div>
         <div className="rangeCamY">
           {rangeCamY}
         </div>
       </div>
       <div className="rangeCamX">
         {rangeCamX}
       </div>
     </div>
     <div className="rangeSens">
       {rangeSens}
     </div>
     <div className='controllerRow'>
       <div></div>
       <div>{forward}</div>
       <div></div>
     </div>
     <div className='controllerRow'>
       <div>{left}</div>
       <div>{stop}</div>
       <div>{right}</div>
     </div>
     <div className='controllerRow'>
       <div></div>
       <div>{backward}</div>
       <div></div>
     </div>
     <div>
       {vl53l0x}
     </div>
     <div>
       {hcsr04}
     </div>
     <div>
       {headingXYZ}
     </div>
     <div>
       {servoSlider}
     </div>
    </div>
  );
}

export{conn};
export default App;
