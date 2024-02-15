window.addEventListener('load', () => {
    const $ = globalThis.$;
    const _LSK = 'SerialMonitor.History';
    const Ser = globalThis.ArduinoSerial;

    class SerialMonitor {
        
        private _baudTxt: HTMLInputElement = $.id('baudtxt');
        private _msg: HTMLInputElement = $.id('msg');
        private _msgDiv: HTMLElement = $.id('msgDiv');
        private _ovrly: HTMLElement = $.id('oly');
        private _hstC: HTMLElement = $.id('hstc');
        
        private _clr: HTMLElement = $.id('clr');
        private _snd: HTMLElement = $.id('snd');
        private _rst: HTMLElement = $.id('rst');
        private _hst: HTMLElement = $.id('hst');

        private history = [
            /* if debug */
            'G1X0F10000',
            'G1X300',
            'G1Y351',
            'G1X0',
            /* endif */
        ];
        private historyIdx = -1;
        private historyTemp = '';
    
        private clear() {
            this._msgDiv.innerHTML = '';
        }

        private baudChanged() {
            Ser.setBaud(this._baudTxt.value);
        }

        private keyUp(ev: KeyboardEvent) {
            if (ev.code == "Enter") {
                this.sendMsg();
            }
        }

        private keyDown(ev: KeyboardEvent){
            const t = this;
            
            t._hstC.style.display = '';
            if (ev.key == 'ArrowUp') {
                if (t.historyIdx < 0) {
                    t.historyIdx = t.history.length - 1;
                    t.historyTemp = t._msg.value;
                } else if (t.historyIdx > 0) {
                    t.historyIdx--;
                }
    
                if (t.historyIdx >= 0 && t.historyIdx < t.history.length) {
                    t._msg.value = t.history[t.historyIdx];
                }
                /* if debug */
                console.log(`[SMon] history ${t.historyIdx}`);
                /* endif */
            }
            if (ev.key == 'ArrowDown') {
                if (t.historyIdx >= 0 && t.historyIdx < t.history.length)
                    t.historyIdx++;
                if (t.historyIdx >= t.history.length) {
                    t._msg.value = t.historyTemp;
                    t.historyIdx = -1;
                } else if (
                    t.historyIdx > 0 &&
                    t.historyIdx < t.history.length
                ) {
                    t._msg.value = t.history[t.historyIdx];
                }
                /* if debug */
                console.log(`[SMon] history ${t.historyIdx}`);
                /* endif */
            }
        }

        private sendMsg() {
            const t = this;
            var text = this._msg.value;
            var line_ending = $.id('lef').value;
            if (!text) return;
            switch (line_ending) {
                case 'n':
                    text += '\n';
                    break;
                case 'r':
                    text += '\r';
                    break;
                case 'rn':
                    text += '\n\r';
            }
            Ser.send(text);
            t._msg.focus();
            t._msg.select();

            var last = t.history[t.history.length - 1];
            if (last != text) {
                t.history.push(text);
                if (t.history.length > 60) {
                    t.history = t.history.slice(t.history.length - 50);
                }
                t._hstC.innerHTML = t.history.map((h, i) => `<div data-idx="${i}">${h}</div>`).join('');
            }
            t.historyIdx = t.history.length - 1;
            t.historyTemp = '';

            localStorage.setItem(_LSK, JSON.stringify(t.history));
        }

        private socketClosed() {
            document.title = 'Serial Monitor Disconnected';
            this._ovrly.style.cssText = '';
        };
    
        private displayMessage(msg) {
            var el = this._msgDiv;
            var scroll =
                el.scrollHeight - el.clientHeight - el.scrollTop;
    
            var date = new Date();
    
            var stampEl = document.createElement('span');
            stampEl.classList.add('stamp');
            stampEl.innerHTML =
                date.getHours().toFixed().padStart(2, '0') +
                ':' +
                date.getMinutes().toFixed().padStart(2, '0') +
                ':' +
                date.getSeconds().toFixed().padStart(2, '0');
    
            var msgEl = document.createElement('span');
            msgEl.classList.add('msg');
            msgEl.innerText = msg;
    
            var lineEl = document.createElement('div');
            lineEl.classList.add('msgl');
            lineEl.appendChild(stampEl);
            lineEl.appendChild(msgEl);
    
            el.appendChild(lineEl);
    
            if (scroll < lineEl.offsetHeight * 1.5) {
                if (el.childNodes.length > 300) {
                    while (el.childNodes.length > 200)
                        el.removeChild(el.childNodes[0]);
                }
                el.scrollTop = el.scrollHeight;
            }
        };

        private toggleHistory() {
            this._hstC.style.display = 
            this._hstC.style.display == "" ? "block" : "";
        }

        private historyCLicked(e: Event){
            const t = this;
            var el = e.target as HTMLElement;
            if (el == t._hstC) return;
            t._msg.value = el.innerText;
            t.historyIdx = Number(el.dataset['idx']);
            t._hstC.style.display = '';
            t._msg.focus();
        }

        public constructor() {
            const t = this;

            t._snd.onclick = () => t.sendMsg()
            t._clr.onclick = () => t.clear();
            t._rst.onclick = () => Ser.reset();
            t._hst.onclick = () => t.toggleHistory();

            t._hstC.onclick = (e) => t.historyCLicked(e);

            t._msg.onkeyup = e => t.keyUp(e);
            t._msg.onkeydown = e => t.keyDown(e); 

            t._baudTxt.onchange = () => t.baudChanged();

            Ser.onError = () => t.socketClosed;
            Ser.onClose = () => t.socketClosed;
            Ser.onOpen = () => {
                var host = document.location.hostname;
                t.displayMessage(
                    `----------- Connected to ${host} -----------`
                );
                document.title = `Serial Monitor at ${host}`;
                t._ovrly.style.display = 'none';
            };
            Ser.onMessage = (m) => t.displayMessage(m);
            Ser.getBaud().then((v) => {
                t._baudTxt.value = v;
            });
        
            Ser.connect();

            try {
                const h = JSON.parse(localStorage.getItem(_LSK)) as string[];
                t.history = Array.isArray(h) ? h : [];
            } catch(e) {
                console.warn('[SMon] history load failed', e)
            }
        }
    }   
    
    globalThis.SerialMon = new SerialMonitor();
});