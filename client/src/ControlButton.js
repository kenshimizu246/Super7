import React, { useState } from 'react';



function ControlButton(handlerOnChange, initVal){
  const [value, setValue] = useState(initVal);

  function handlerWrapper(newValue){
    console.log('handlerWrapper');
    if(handlerOnChange){
      handlerOnChange(newValue);
    }
  }


  return (
    <div>
      <input type="checkbox"
        checked={value}
        onChange={(e) => {
          setValue(e.target.checked);
          handlerWrapper(e.target.checked);
          console.log('aaa:'+e.target.checked);
        }}
      />
    </div>
  ) 
  ;
}

export default ControlButton;
