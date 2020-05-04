import React, { useState, useEffect } from 'react';


function HeadingXYZ(conn, source, id, label, description){
  const [status, setStatus] = useState(0);
  const [heading, setHeading] = useState(0);
  const [x, setX] = useState(0);
  const [y, setY] = useState(0);
  const [z, setZ] = useState(0);
  const [timestamp, setTimestamp] = useState("");

  function onMessage(msg){
    var s = JSON.stringify(msg);
    if(msg.header.source === source){
      if(0 < msg.body.status == 0){
        setHeading(msg.body.heading);
        setX(msg.body.x);
        setY(msg.body.y);
        setZ(msg.body.z);
        setStatus(msg.body.status);
        //setTimestamp(msg.body.timestamp);
      }
    }
  }

  useEffect(() => {
    conn.on('MSG',onMessage.bind(this));

    return () => {conn.removeEventListener('MSG', onMessage)};
  }, []);
  
  return (
    <div className="compassBox">
      <div className="compassBoxLabel">{label}</div>
      <div className="compassBoxDescr">{description}</div>
      <div className="compassBoxValue">
        <div>{heading}</div>
        <div className="compassXyz">
          <div className="compassXyzLabel">X:</div>
          <div>{x}</div>
          <div className="compassXyzLabel">Y:</div>
          <div>{y}</div>
          <div className="compassXyzLabel">Z:</div>
          <div>{z}</div>
        </div>
      </div>
    </div>
  );
}

export default HeadingXYZ;
