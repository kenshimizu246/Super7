import React, { useState, useEffect } from 'react';



function Range(handlerOnClick, minVal, maxVal, stepVal, initVal, flip, showValue){
  const [value, setValue] = useState(initVal);
  const [eventDesc, setEventDesc] = useState("");
  const range = maxVal - minVal;
  const center = range/2;

  function convert(value) {
    if(flip){
      value = range - (value - minVal) + minVal
    }
    return value;
  }

  function getValue(){
    return value;
  }

  function handlerWrapper(){
    if(handlerOnClick){
      handlerOnClick(convert(value));
    }
  }

  function handleOnChange(e){
    setValue(e.target.value);
  }

  function handleOnInput(e){
    setValue(e.target.value);
  }
  function handleOnTouchStart(e){
    setEventDesc('OnTouchStart');
  }
  function handleOnTouchEnd(e){
    setEventDesc('OnTouchEnd');
    handlerWrapper();
  }
  var valueText = ""
  if(showValue){
    valueText = (<div className="rangeValueText">{value}</div>);
  }

  // {value} : {eventDesc}
  return (
    <div>
      <input type="range"
        min={minVal} max={maxVal} step={stepVal} defaultValue={initVal}
        onClick={() => {handlerWrapper();}}
        onChange={(e) => {handleOnChange(e);}}
        onInput={(e) => {handleOnInput(e);}}
        onTouchStart={(e) => {handleOnTouchStart(e);}}
        onTouchEnd={(e) => {handleOnTouchEnd(e);}}
      />
      {valueText}
    </div>
  );
}

export default Range;
