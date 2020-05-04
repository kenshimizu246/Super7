import React, { useState, useEffect } from 'react';


function Distance(conn, source, id, label, description){
  const [value, setValue] = useState(0);
  const [timestamp, setTimestamp] = useState("");

  function onMessage(msg){
    var s = JSON.stringify(msg);
    if(msg.header.source === source){
      //console.log("onMessageX:"+String(msg.header.source-id.toString()));
      //console.log("onMessageY:"+String(msg.body.status));
      if(0 < msg.body.distance){
        setValue(msg.body.distance);
        //setTimestamp(msg.body.timestamp);
      }
    }
  }

  useEffect(() => {
    conn.on('MSG',onMessage.bind(this));

    return () => {conn.removeEventListener('MSG', onMessage)};
  }, []);
  
  return (
    <div className='distanceBox'>
      <div className='distanceBoxLabel'>{label}({id})</div>
      <div className='distanceBoxDescr'>{description}</div>
      <div className='distanceBoxValue'>{value}</div>
    </div>
  );
}

export default Distance;
