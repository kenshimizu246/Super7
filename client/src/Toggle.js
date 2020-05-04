import React, { useState } from 'react';
import logo from './arrow.svg'


function DriveButton(handlerOnClick, command, label, initVal){
  const [value, setValue] = useState(initVal);
  const size = 64;

  function handlerWrapper(){
    console.log('handlerWrapper');
    if(handlerOnClick){
      handlerOnClick(value);
    }
  }

  return (
    <div>
      <div>
        <input type="button"
           id={command}
           value={label}
           onClick={() => {handlerOnClick(command);}}
           width={size}
           height={size}
        />
      </div>
      <div>{value}</div>
    </div>
  );
//        <input type="range"
//          min={minVal} max={maxVal} step={stepVal} defaultValue={initVal}
//          onClick={() => {handlerWrapper();}}
//          onChange={(e) => {setValue(e.target.value);}}
//        />
}

export default DriveButton;
