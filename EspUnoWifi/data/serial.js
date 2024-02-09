(function() {
    // var l = console.log;
    var l = function(){};
    var _socket = {readyState: -1};
    var _hostName = document.location.hostname;

    window.ArduinoSerial = {
        connect: __connect
    };

    var __connect= function() {
        if (_socket.readyState == 1) {
            l('[AS] WebSocket already open!');
            return;
        }

        l('[AS] Opening WebSocket...');
        // _socket = new WebSocket(`ws://192.168.0.169:81`, 'arduino');
        _socket = new WebSocket(`ws://${_hostName}:81`, 'arduino');
        var _lastMsg = null;

        var as = window.ArduinoSerial;
        _socket.onerror = function(event) { console.error('[AS] WebSocket error!', event); if (typeof(as.onError) == "function") as.onError(event); };
        _socket.onclose = function(event) { l('[AS] WebSocket closed'); if (typeof(as.onClose) == "function") as.onClose(event); };
        _socket.onopen = function(event) { l('[AS] WebSocket opened'); if (typeof(as.onOpen) == "function") as.onOpen(event); };
        
        
        var msgReceived = function(str){
            if (_lastMsg !== null){
                str = _lastMsg + str;
            }
            var lastChar= str.substr(str.length - 1);
            if (lastChar != '\n' && lastChar != '\r'){
                _lastMsg = str;
                return;
            }

            _lastMsg = null;
            as.onMessage(str);
        }

        _socket.onmessage = function(event) {
            if (typeof(as.onMessage) != "function")
                return;

            try {
                var d = event.data;
                if (typeof(d) == "string") {
                    msgReceived(d);
                }
                else {
                    var reader = new FileReader();
                    reader.onload = function (e) {
                        msgReceived(reader.result);
                    }
                    reader.readAsText(event.data);
                }
            }
            catch (error){
                console.error(error);
                _socket.close();
            }
        };
    };

    var __send = function(msg){
        if(_socket.readyState != 1) return;
        _socket.send(msg);
    };

    var __handleBaudInt = function(baud, callback) {
        var xhr = new XMLHttpRequest();

        xhr.onload = function () {
            l('[AS] Baud rate', baud);
            if(callback !== undefined)
                callback(xhr.responseText);
        };
        var url = '/getbaudrate';
        if (baud) {
            baud = parseInt(baud, 10);
            if(baud >= 9600)
                url += `?baudrate=${baud}`;
        }
        xhr.open('GET', url);
        xhr.send();
    };

    var __getBaud = function(callback) {
        var baud = __handleBaudInt(false, callback);
    };

    var __setBaud = function(baud){
         __handleBaudInt(baud);
        l('[AS] Setting baud rate', baud);
    }

    var __reset = function() {
        l('[AS] Resetting Arduino');
        var xhr = new XMLHttpRequest();
        xhr.open('POST', '/reset', false);
        xhr.send();
    };
    
    window.ArduinoSerial.connect = __connect;
    window.ArduinoSerial.send = __send;
    window.ArduinoSerial.getBaud = __getBaud;
    window.ArduinoSerial.setBaud = __setBaud;
    window.ArduinoSerial.reset = __reset;
})();