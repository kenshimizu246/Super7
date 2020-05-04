import React, { useState, useEffect } from 'react';


function StatusBar(conn, source, id, label){
  const [value, setValue] = useState(0);

  function onMessage(msg){
    var s = JSON.stringify(msg);
    if(msg.header.source === source){
      //console.log("onMessageX:"+String(msg.header.source-id.toString()));
      //console.log("onMessageY:"+String(msg.body.status));
      if(0 < msg.body.distance){
        setValue(msg.body.distance);
      }
    }
  }

  useEffect(() => {
    conn.on('MSG',onMessage.bind(this));

    return () => {conn.removeEventListener('MSG', onMessage)};
  }, []);
  
  return (
    <div className='statusBar'>
      <div>{value}</div>
    </div>
  );
}

export default StatusBar;
