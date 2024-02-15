(function () {
    class ArduinoSerialClass {
        private _socket: WebSocket | null = null;
        private _hostName: string;
        private _lastMsg: string | null = null;

        public onError: ((e: Event) => void) | null = null;
        public onClose: ((e: Event) => void) | null = null;
        public onOpen: ((e: Event) => void) | null = null;
        public onMessage: ((msg: string) => void) | null = null;

        public constructor() {
            this._hostName = window.location.host;
            this._socket = null;
        }

        // Private methods

        private socketError(event: any) {
            console.error('[AS] WebSocket error!', event);
            if (this.onError != null) this.onError(event);
        }

        private socketOpen(event: any) {
            /* if debug */
            console.log('[AS] WebSocket opened');
            /* endif */
            if (this.onOpen != null) this.onOpen(event);
        }

        private socketClose(event: any) {
            /* if debug */
            console.log('[AS] WebSocket closed');
            /* endif */
            if (this.onClose != null) this.onClose(event);
        }

        private socketMessage(event: any) {
            if (this.onMessage == null) return;

            const t = this;

            try {
                var d = event.data;
                if (typeof d == 'string') {
                    this.handleMsg(d);
                } else {
                    var reader = new FileReader();
                    reader.onload = (e) => {
                        t.handleMsg(reader.result?.toString() ?? '');
                    };
                    reader.readAsText(event.data);
                }
            } catch (error) {
                console.error(error);
                this._socket?.close();
            }
        }

        private handleMsg(str: string) {
            if (this._lastMsg !== null) {
                str = this._lastMsg + str;
            }
            var lastChar = str.substring(str.length - 1);
            if (lastChar != '\n' && lastChar != '\r') {
                this._lastMsg = str;
                return;
            }

            this._lastMsg = null;
            if (this.onMessage != null) {
                /* if serilog */
                console.log('[AS] sending', str);
                /* endif */
                this.onMessage(str);
            }
        }

        private async handleBaud(baud: number = NaN): Promise<number> {
            /* if local */
            return baud??115200;
            /* else */
            var url = '/getbaudrate';
            if (!Number.isNaN(baud)) {
                url += `?baudrate=${baud}`;
            }
            try {
                const xhr = new XMLHttpRequest();
                xhr.open('GET', url, false);
                xhr.send(null);

                return Number(xhr.responseText);
            } catch (e) {
                console.error('[AS] failed to get/set baudrate');
            }
            return NaN;
            /* endif */
        }

        // Public methods

        public connect() {
            if (this._socket?.readyState == 1) {
                /* if debug */
                console.log('[AS] WebSocket already open!');
                /* endif */
                return;
            }

            /* if debug */
            console.log('[AS] Opening WebSocket...');
            /* endif */
            var s: WebSocket;
            /* if local */
            s = new WebSocket(`ws://192.168.0.169:81`, 'arduino');
            /* else */
            s = new WebSocket(`ws://${this._hostName}:81`, 'arduino');
            /* endif */
            s.onerror = (e) => this.socketError(e);
            s.onclose = (e) => this.socketClose(e);
            s.onopen = (e) => this.socketOpen(e);
            s.onmessage = (e) => this.socketMessage(e);
            this._socket = s;
        }

        public send(msg: string) {
            if (this._socket?.readyState != 1) return;
            this._socket.send(msg);
            /* if serilog */
            console.log('[AS] sending', msg);
            /* endif */
        }

        public sendln(msg) {
            this.send(msg + '\n');
        }

        public async getBaud(): Promise<number> {
            return await this.handleBaud();
        }

        public setBaud(baud: number) {
            this.handleBaud(baud);
            /* if debug */
            console.log('[AS] Setting baud rate', baud);
            /* endif */
        }

        public async reset(): Promise<any> {
            /* if debug */
            console.log('[AS] Resetting Arduino');
            /* endif */
            /* if local */
            console.warn('[AS] Dummy reset')
            /* else */
            try {
                await fetch('/reset', { method: 'POST' });
            } catch (e) {
                console.error('[AS] Failed to reset arduino');
            }
            /* endif */
        }
    }

    /* if dummyserial */
    class DummySerialClass {

        public onError: ((e: Event) => void) | null = null;
        public onClose: ((e: Event) => void) | null = null;
        public onOpen: ((e: Event) => void) | null = null;
        public onMessage: ((msg: string) => void) | null = null;

        private int: number = setInterval(()=> this.onMessage && this.onMessage("dummy message"), 5000);

        public connect() {
            console.log('[AS] Opening WebSocket...');
            this.onOpen(new Event("ws-open"));
        }

        public send(msg: string) {
            console.log('[AS] sending', msg);
            setTimeout(() => this.onMessage && this.onMessage(`Response for "${msg}"`), 100);
        }

        public sendln(msg) {
            this.send(msg + '\n');
        }

        public async getBaud(): Promise<number> {
            return new Promise<number>((r,rj) => {r(9600)});
        }

        public setBaud(baud: number) {
            console.log('[AS] Setting baud rate', baud);
        }

        public async reset(): Promise<any> {
            console.log('[AS] Resetting Arduino');
        }
    }
    globalThis.ArduinoSerial = new DummySerialClass();
    /* else */
    /* if dummygrbl */
    class DummyGrblClass {

        public onError: ((e: Event) => void) | null = null;
        public onClose: ((e: Event) => void) | null = null;
        public onOpen: ((e: Event) => void) | null = null;
        public onMessage: ((msg: string) => void) | null = null;

        private int: number = setInterval(()=> this.onMessage && this.onMessage("<Run|MPos:0.000,0.000,0.000|WCO:12.000,15.000,0.000>\n[GC:G91 G20 F10000 S0]"), 5000);

        public connect() {
            console.log('[AS] Opening WebSocket...');
            this.onOpen(new Event("ws-open"));
        }

        public send(msg: string) {
            console.log('[AS] sending', msg);
            var rsp = "ok";
            if (msg == "$"){
                rsp = "$32=1\n$30=1000";
            } else if (msg = "$G") {
                rsp = "[GC:G91 G20 F10000 S0]";
            } else if (msg == "?") {
                rsp = "<Run|MPos:0.000,0.000,0.000|WCO:12.000,15.000,0.000>"
            } else if (msg == "$I") {
                rsp = "[0.8g.asdgash:]"
            }

            setTimeout(() => this.onMessage && this.onMessage(rsp), 300);
        }

        public sendln(msg) {
            this.send(msg + '\n');
        }

        public async getBaud(): Promise<number> {
            return new Promise<number>((r,rj) => {r(9600)});
        }

        public setBaud(baud: number) {
            console.log('[AS] Setting baud rate', baud);
        }

        public async reset(): Promise<any> {
            console.log('[AS] Resetting Arduino');
        }
    }
    globalThis.ArduinoSerial = new DummyGrblClass();
    /* else */
    globalThis.ArduinoSerial = new ArduinoSerialClass();
    /* endif */
    /* endif */
})();
