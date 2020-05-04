import React, { useState } from 'react';
import logo from './arrow.svg'


function Toggle(handlerOnClick, command, label, initVal){
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
}

export default Toggle;
