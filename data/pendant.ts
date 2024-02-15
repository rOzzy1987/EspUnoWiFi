window.addEventListener('load', () => {
    const G = globalThis;
    const $ = G.$;
    const Ser = G.ArduinoSerial;
    const _LSK = "GrblPendant.State";

    class SpindleStatus {
        isLaser: boolean = true;
        isActive: boolean = false;
        speed: number = 0;
        max: number = 1000;
        cw: boolean = true;
    }

    class FeedStatus {
        current: number = 0;
        max: number = 1000;
    }

    class JogStatus {
        supported: boolean = false;
        step: number = 10;
        feed: number = 1000;
        axis: 'x' | 'y' | 'z' = 'x';
        deleteValueOnKey: boolean = true;
        absolute: boolean = true;
    }

    class Coordinate {
        x: number = 0;
        y: number = 0;
        z: number = 0;
    }

    class GrblVersion {
        major: string = '0';
        minor: string = '8';
        plus: string = 'a';
    }

    class GrblStatus {
        spindle: SpindleStatus = new SpindleStatus();
        isMetric: boolean = true;
        isAbsolute: boolean = true;
        isCoolantActive: boolean = false;
        feed: FeedStatus = new FeedStatus();
        mpos: Coordinate = new Coordinate();
        wpos: Coordinate = new Coordinate();
        woff: Coordinate = new Coordinate();

        state: string = 'Idle';
        jog: JogStatus = new JogStatus();
        version = new GrblVersion();
    }

    class GrblPendant {
        private status: GrblStatus = new GrblStatus();
        private _int_ref: number | null = null;
        private _alarm_int_ref: number | null = null;

        private getGrblStatus() {
            if (this.status.state != 'Idle') return;
            Ser.sendln('$G');
        }
        private getVars() {
            if (this.status.state != 'Idle') return;
            Ser.sendln('$$');
        }
        private getPos() {
            Ser.sendln('?');
        }

        private getBuild() {
            Ser.sendln('$I');
        }

        private updateMetric() {
            $.set('.unit.l', this.status.isMetric ? 'mm' : 'in');
            $.seth(
                '.unit.v',
                this.status.isMetric ? '<u>mm</u><br>min' : '<u>in</u><br>min'
            );
        }
        private setMetric(val) {
            if (this.status.isMetric == val) return;
            this.status.isMetric = val;
            this.updateMetric();
        }

        private updateAbsolute() {
            $.tc('.st.abs', 'active', this.status.isAbsolute);
        }
        private setAbsolute(val) {
            if (this.status.isAbsolute == val) return;
            this.status.isAbsolute = val;
            this.updateAbsolute();
        }

        private updateSpindle() {
            $.tc('.st.spn', 'active', this.status.spindle.isActive);
        }
        private setSpindle(val) {
            if (this.status.spindle.isActive == val) return;
            this.status.spindle.isActive = val;
            this.updateSpindle();
        }

        private updateCoolant() {
            $.tc('.st.cln', 'active', this.status.isCoolantActive);
        }
        private setCoolant(val) {
            if (this.status.isCoolantActive == val) return;
            this.status.isCoolantActive = val;
            this.updateCoolant();
        }

        private updateFeed() {
            $.set('.st.feed', this.status.feed.current.toFixed());
        }
        private setFeed(val) {
            val = Number(val);
            if (this.status.feed == val) return;
            this.status.feed.current = val;
            this.updateFeed();
        }

        private updateSpindleSpeed() {
            var v = this.status.spindle.speed;
            var a = this.status.spindle.isLaser
                ? 100 / this.status.spindle.max
                : 1;
            $.set('.st.sspd', v * a);
        }
        private setSpindleSpeed(val) {
            val = Number(val);
            if (this.status.spindle.speed == val) return;
            this.status.spindle.speed = val;
            this.updateSpindleSpeed();
        }

        private updateSpindleDir() {
            $.tc('.st.spd', 'active', this.status.spindle.cw);
        }

        private updateIsLaser() {
            var val = this.status.spindle.isLaser ? '%' : 'Rpm';

            $.set('.unit.sspd', val);
        }
        private setIsLaser(val) {
            if (this.status.spindle.isLaser == val) return;
            this.status.spindle.isLaser = val;
            this.updateIsLaser();
        }

        private updateCoords() {
            const s = this.status;
            var d = s.isMetric ? 2 : 3;
            $.set('.st.wx', s.wpos.x.toFixed(d));
            $.set('.st.wy', s.wpos.y.toFixed(d));
            $.set('.st.wz', s.wpos.z.toFixed(d));
            $.set('.st.mx', s.mpos.x.toFixed(d));
            $.set('.st.my', s.mpos.y.toFixed(d));
            $.set('.st.mz', s.mpos.z.toFixed(d));
        }

        private updateState() {
            const s = this.status.state;
            $.set('.st.sttxt', s);
            if (s == 'Alarm' && !this._alarm_int_ref)
                this._alarm_int_ref = setInterval(
                    () => $.tc('.k.arm', 'active'),
                    333
                );
            if (s != 'Alarm' && this._alarm_int_ref) {
                clearInterval(this._alarm_int_ref);
                $.tc('.k.arm', 'active', false);
            }
        }

        private updateJogStep() {
            $.do('[data-cmd=js]', (e) => {
                $.tc(e, 'active', e.dataset['arg'] == this.status.jog.step);
            });
        }

        private updateJogFeed() {
            $.do('[data-cmd=jf]', (e) => {
                $.tc(e, 'active', e.dataset['arg'] == this.status.jog.feed);
            });
        }
        private updateJogAbsolute() {
            $.do('.onoff.st.jabs', (e) => {
                $.tc(e, 'active', this.status.jog.absolute);
            });
        }

        private parseGrblStatus(data) {
            const t = this;
            var codes = data.split(' ');
            for (var code of codes) {
                code = code.toUpperCase();
                if (code == 'G21') t.setMetric(true);
                if (code == 'G20') t.setMetric(false);
                if (code == 'G90') t.setAbsolute(true);
                if (code == 'G91') t.setAbsolute(false);
                if (code == 'M3' || code == 'M4') t.setSpindle(true);
                if (code == 'M5') t.setSpindle(false);
                if (code == 'M7' || code == 'M8') t.setCoolant(true);
                if (code == 'M9') t.setCoolant(false);

                var fMatch = /^F([0-9]+(?:\.[0-9]*)?)$/g.exec(code);
                if (fMatch) t.setFeed(fMatch[1]);
                var sMatch = /^S([0-9]+(?:\.[0-9]*)?)$/g.exec(code);
                if (sMatch) t.setSpindleSpeed(sMatch[1]);
            }
        }

        private parseStatus(data) {
            const parts = data.split(',');
            const s = this.status;
            for (var i = 0; i < parts.length; i++) {
                var part = parts[i];
                const match = /^([\da-z]+):/gi.exec(part);
                if (match) {
                    var kw = match[1];
                    if (kw == 'MPos') {
                        part = part.substring(5);
                        s.mpos.x = Number(part);
                        s.mpos.y = Number(parts[++i]);
                        s.mpos.z = Number(parts[++i]);
                        s.wpos.x = s.mpos.x - s.woff.x;
                        s.wpos.y = s.mpos.y - s.woff.y;
                        s.wpos.z = s.mpos.z - s.woff.z;
                    } else if (kw.startsWith('WC')) {
                        part = part.substring(kw.length + 1);
                        s.woff.x = Number(part);
                        s.woff.y = Number(parts[++i]);
                        s.woff.z = Number(parts[++i]);

                        s.wpos.x = s.mpos.x - s.woff.x;
                        s.wpos.y = s.mpos.y - s.woff.y;
                        s.wpos.z = s.mpos.z - s.woff.z;
                    } else if (kw == 'WPos') {
                        (part = part.substring(5)), (s.wpos.x = Number(part));
                        s.wpos.y = Number(parts[++i]);
                        s.wpos.z = Number(parts[++i]);
                        s.woff.x = s.wpos.x - s.mpos.x;
                        s.woff.y = s.wpos.y - s.mpos.y;
                        s.woff.z = s.wpos.z - s.mpos.z;
                    }
                }
            }

            this.updateCoords();
            this.updateState();
        }

        private parseVersion(major: string, minor: string, plus) {
            var v = this.status.version,
                _E = +$.num.u2n(`${major}.${minor}`);
            major.length > 1 &&
                minor == null &&
                ((minor = major.substring(1)), (major = major.substring(0, 1)));
            v.major = major;
            v.minor = minor;
            v.plus = plus;
            this.status.jog.supported = _E >= 1.1;
            console.log('[PND] GRBL version', v.major, v.minor, v.plus);
        }

        private parseVar(no, val) {
            val = Number(val);
            switch (Number(no)) {
                case 32:
                    this.setIsLaser(val == 1);
                    this.updateSpindleSpeed();
                    break;
                case 30:
                    this.status.spindle.max = val;
                    this.updateSpindleSpeed();
                    break;
            }
        }

        private parseMessage(msg: string) {
            const t = this;
            var lines = msg.replaceAll('\r', '\n').split('\n');
            for (var l of lines) {
                /* if gcodelog */
                if(l == "") continue;
                console.log('[PND] line received:', l);
                /* endif */
                l = l.trim();
                var gcodeStatusMatch = /^\[(?:GC:)?(.*)\]$/gi.exec(l);
                var versionMatch =
                    /^\[(?:ver:)?(\d+)\.?(\d+)([a-z]+)(.*):\]$/gi.exec(l);
                var posMatch = /^\<(.*)\>$/gi.exec(l);
                var varMatch = /^\$(\d*)=(\d+\.\d+)/gi.exec(l);

                if (versionMatch) {
                    t.parseVersion(
                        versionMatch[1],
                        versionMatch[2],
                        versionMatch[3]
                    );
                } else if (gcodeStatusMatch) {
                    t.parseGrblStatus(gcodeStatusMatch[1]);
                } else if (posMatch) {
                    var parts = posMatch[1].split('|');
                    t.status.state =
                        parts.length == 1
                            ? parts[0].substring(0, parts[0].indexOf(','))
                            : parts[0];
                    for (var part of parts) t.parseStatus(part);
                } else if (varMatch) {
                    t.parseVar(varMatch[1], varMatch[2]);
                }
            }
        }

        private handleKey(e: Event) {
            const t = this;
            const s = t.status;

            var key = e.currentTarget as HTMLElement;
            var _s = (_d) => {
                /* if gcodelog */
                console.log('[PND] GCode', _d);
                /* endif */
                Ser.sendln(_d);
            };

            if (key.dataset.hasOwnProperty('gcode')) {
                var gcode = key.dataset['gcode'];
                _s(gcode + '\n');
            }

            if (key.dataset.hasOwnProperty('cmd')) {
                var cmd = key.dataset['cmd'];
                var arg = key.dataset['arg'];
                switch (cmd) {
                    case 'wo':
                        switch (arg) {
                            case 'xh':
                                _s('G92X' + s.wpos.x / 2);
                                break;
                            case 'yh':
                                _s('G92Y' + s.wpos.y / 2);
                                break;
                            case 'zh':
                                _s('G92Z' + s.wpos.z / 2);
                                break;
                        }
                        break;
                    case 'fd':
                        var f = s.feed.current;
                        var fa = Math.pow(10, 0.2);
                        f = Math.round(arg == '+' ? f * fa : f / fa);
                        _s('F' + f.toFixed(3));
                        break;
                    case 'spn':
                        _s((s.spindle.cw ? 'M3S' : 'M4S') + ((s.spindle.max * Number(arg)) / 100).toFixed());
                        break;
                    case 'spd':
                        s.spindle.cw = !s.spindle.cw;
                        t.updateSpindleDir()
                        break;
                    case 'js':
                        s.jog.step = Number(arg);
                        t.updateJogStep();
                        break;
                    case 'jf':
                        s.jog.feed = Number(arg);
                        t.updateJogFeed();
                        break;
                    case 'jg':
                        _s(
                            `${
                                s.jog.supported ? '$J=G91' : 'G91G1'
                            }${arg}${s.jog.step.toFixed(3)}F${s.jog.feed.toFixed()}`
                        );
                        break;
                    case 'jh':
                        _s('G90G1' + arg + '0F' + s.jog.feed);
                        break;
                    case 'conn':
                        Ser.connect();
                        break;
                    case 'rst':
                        Ser.reset();
                        break;
                    default:
                        console.error('[PND] Unknown key', cmd, arg);
                        break;
                }
            }
        }

        private updateJogInputs() {
            $.do('.ji input', (e) => {
                const a = $.which(e, ['x', 'y', 'z']);
                var n = Number(e.value);
                if (e.value == '' || Number.isNaN(n)) e.value = '';
                else e.value = n.toFixed(this.status.isMetric ? 2 : 3);
            });
        }

        private updateJogAxis() {
            const x = this.status.jog.axis;

            $.tc('.ji .sa.' + x, 'active', true);
            $.tc(`.ji .sa:not(.${x})`, 'active', false);
        }

        private jogAxisChanged() {
            const t = this;
            const x = t.status.jog.axis;
            t.updateJogAxis();
            t.updateJogInputs();
            t.status.jog.deleteValueOnKey =
                $.sng('.ji input.' + x).value ==
                t.status.wpos[x].toFixed(t.status.isMetric ? 2 : 3);
        }

        private jog() {
            const t = this;
            const s = t.status;
            t.updateJogInputs();
            const x = $.num.n2u($.sng('.ji  input.x').value);
            const y = $.num.n2u($.sng('.ji  input.y').value);
            const z = $.num.n2u($.sng('.ji  input.z').value);

            const abs = s.jog.absolute ? 'G90' : 'G91';
            const gcode =
                `${s.jog.supported ? '$J=' + abs : abs + 'G1'}` +
                `${x == '' ? '' : 'X' + x}${y == '' ? '' : 'Y' + y}${z == '' ? '' : 'Z' + z}F${s.jog.feed}`;

            /* if gcodelog */
            console.log('[PND] GCode', gcode);
            /* endif */
            Ser.sendln(gcode);
        }

        private handleJogKey(e: Event) {
            const t = this;
            const s = t.status;

            var k = e.target as HTMLElement;
            const a = $.which(k, ['x', 'y', 'z']) as 'x' | 'y' | 'z';
            const i = $.sng('.ji input.' + s.jog.axis);
            const ia = $.sng('.ji input.' + a);
            switch (
                $.which(k, ['sa', 'lit', 'rld', 'bks', 'jabs', 'dec', 'jog', 'c', 'ac'])
            ) {
                case 'sa':
                    if (a == s.jog.axis) return;
                    s.jog.axis = a;
                    t.jogAxisChanged();
                    break;
                case 'rld':
                    ia.value = s.wpos[a];
                    t.updateJogInputs();
                    break;
                case 'lit':
                    i.value += k.innerText;
                    break;
                case 'bks':
                    i.value = i.value.substring(0, i.value.length - 1);
                    break;
                case 'dec':
                    i.value += $.num.dp;
                    break;
                case 'jabs':
                    s.jog.absolute = !s.jog.absolute;
                    t.updateJogAbsolute();
                    break;
                case 'jog':
                    t.jog();
                    break;
                case 'c':
                    i.value = '';
                    break;
                case 'ac':
                    $.do('.ji input', e => e.value = '');
                    break; 
                default:
                    console.warn(
                        '[PND] Unknown jog key pressed:',
                        k.className,
                        k.innerText
                    );
                    break;
            }
        }

        private ping() {
            this.getGrblStatus();
            setTimeout(() => this.getPos(), 50);
        }

        private initJog() {
            const t = this;
            const s = t.status;
            t.jogAxisChanged();
            t.updateJogAbsolute();

            $.do('.ji input', (i) => {
                i.addEventListener('keydown', (e: KeyboardEvent) => {
                    const a = $.which(i, ['x', 'y', 'z']) as 'x' | 'y' | 'z';
                    s.jog.axis = a;
                    t.updateJogAxis();

                    if (s.jog.deleteValueOnKey) {
                        i.value = '';
                        s.jog.deleteValueOnKey = false;
                    }
                });
                i.addEventListener('blur', () => {
                    t.jogAxisChanged();
                });
            });
        }

        private init() {
            const t = this;

            t.updateAbsolute();
            t.updateCoolant();
            t.updateCoords();
            t.updateFeed();
            t.updateIsLaser();
            t.updateJogFeed();
            t.updateJogStep();
            t.updateMetric();
            t.updateSpindle();
            t.updateSpindleDir();
            t.updateSpindleSpeed();
            t.updateState();

            t.getBuild();
            setTimeout(() => t.getVars(), 100);
            /* if nopoll */
            setTimeout(() => t.ping(), 200);
            /* else */
            setTimeout(
                () => (t._int_ref = setInterval(() => t.ping(), 1000)),
                200
            );
            /* endif */
        }

        private destroy() {
            if (this._int_ref !== null) {
                clearInterval(this._int_ref);
                this._int_ref = null;
            }
        }

        

        public saveState() {
            localStorage.setItem(_LSK, JSON.stringify(this.status));
        }

        public loadState() {
            try {
                var a = localStorage.getItem(_LSK);
                var b = JSON.parse(a) as GrblStatus;
                this.status.jog = b.jog;
            } catch(e){
                console.warn('[PND] Loading status failed:', e);
            }
        }

        public constructor() {
            const t = this;
            Ser.onMessage = (m) => t.parseMessage(m);
            Ser.onOpen = () => {
                t.init();
                t.initJog();
                t.loadState();
            };
            Ser.onClose = () => t.destroy();
            Ser.onError = () => t.destroy();

            Ser.connect();

            $.do('.kb .k', (e) => (e.onclick = (ev) => t.handleKey(ev)));
            $.do('.ji .k', (e) => (e.onclick = (ev) => t.handleJogKey(ev)));
        }
    }

    G.pendant = new GrblPendant();
    document.addEventListener('click', () => G.pendant.saveState());
    document.addEventListener('keyup', () => G.pendant.saveState());
});
