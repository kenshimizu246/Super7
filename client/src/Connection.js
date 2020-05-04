import EventEmitter from "events";

export default class Connection {
  constructor(url, protocols) {
    console.log("Connection.constructor:" + url + ":" + protocols);
    this.url = url;
    this.protocols = protocols;
    this.emitter = new EventEmitter();
    this.errorCount = 0;
    this.maxErrorCount = 0;
    setInterval(this._heartbeat.bind(this),1000*10);
  }

  send(msg) {
    console.log("Connection.send:"+msg);
    if(this.connection && this.connection.readyState === WebSocket.OPEN){
      this.connection.send(msg);
      console.log("send:"+msg);
    } else {
      alert("connection is not open:"+this.connection.readyState);
    }
  }

  on(eventName, listener) {
    console.log("Connection.on:"+eventName);
    this.emitter.on(eventName, listener);
  }

  removeEventListener(eventName, listener) {
    console.log("Connection.removeEventListener:"+eventName);
    this.emitter.removeListener(eventName, listener);
  }

  removeAllEventListeners(eventName) {
    console.log("Connection.removeAllEventListeners:"+eventName);
    var lls = this.emitter.listeners(eventName);
    for(var i = 0; i < lls.length; i++) {
      this.emitter.removeListener(eventName, lls[i]);
    }
  }

  connect() {
    console.log("Connection.connect");
    //this.connection = new WebSocket(this.url, this.protocols);
    this.connection = new WebSocket(this.url);

    console.log('connect():'+this.url);

    this.connection.onopen = this._onopen.bind(this);
    this.connection.onerror = this._onerror.bind(this);
    this.connection.onmessage = this._onmessage.bind(this);
    this.connection.onclose = this._onclose.bind(this);
  }

  close() {
    console.log("Connection.close");
    this.connection.close();
  }

  // 0 CONNECTING
  // 1 OPEN
  // 2 CLOSING
  // 3 CLOSED
  readyState() {
    console.log("Connection.readyState:"+this.connection.readyState);
    return this.connection.readyState;
  }

  _heartbeat() {
    if(this.connection && this.connection.readyState === WebSocket.OPEN) {
//      var msg = MessageFactory.createHeartbeat();
//      this.send(msg);
    }
  }

  _onopen() {
    this.errorCount = 0;
    this.emitter.emit('SYS', 'ONOPEN');
  }

  _onerror(error) {
    var ss = JSON.stringify(error);
    console.log("error:"+ss);
    this.errorCount++;
    this.emitter.emit('SYS', 'ERROR:' + ss);
    if(this.connection.readyState === WebSocket.CLOSED && this.errorCount < this.maxErrorCount) {
      this.connect();
    }
  }

  _onmessage(e) {
    //console.log("connector._onmessage:"+e.data);
    var obj = JSON.parse(e.data);
    this.emitter.emit('MSG', obj);
  }

  _onclose(e) {
    var ss = JSON.stringify(e);
    console.log('onclose:'+ss);
    this.emitter.emit('SYS', 'ONCLOSE:'+ss);
    // setTimeout(this.connect.bind(this), 1000);
    this.connect();
  }

//  connect() {
//    this.connection = new WebSocket(this.url, this.protocols);
//
//    console.log('connect():'+this.url);
//
//    this.connection.onopen = this._onopen.bind(this);
//    this.connection.onerror = this._onerror.bind(this);
//    this.connection.onmessage = this._onmessage.bind(this);
//    this.connection.onclose = this._onclose.bind(this);
//  }

  close() {
    this.connection.close();
  }

  // 0 CONNECTING
  // 1 OPEN
  // 2 CLOSING
  // 3 CLOSED
  readyState() {
    return this.connection.readyState;
  }
}
