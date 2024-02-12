window.addEventListener('load', () => {
    const $ = globalThis.$;
    const Ser = globalThis.ArduinoSerial;

    class SpindleStatus {
        isLaser: boolean = true;
        isActive: boolean = false;
        speed: number = 0;
        max: number = 1000;
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
        woff?: Coordinate = null;

        state: string = 'Idle';
        jog: JogStatus = new JogStatus();
        version = new GrblVersion();
    }

    class GrblPendant {
        private status: GrblStatus = new GrblStatus();
        private _int_ref: number | null = null;
        private jogAbsolute: boolean = false;

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
            var d = this.status.isMetric ? 2 : 3;
            $.set('.st.wx', this.status.wpos.x.toFixed(d));
            $.set('.st.wy', this.status.wpos.y.toFixed(d));
            $.set('.st.wz', this.status.wpos.z.toFixed(d));
            $.set('.st.mx', this.status.mpos.x.toFixed(d));
            $.set('.st.my', this.status.mpos.y.toFixed(d));
            $.set('.st.mz', this.status.mpos.z.toFixed(d));
        }

        private _alarm_int_ref: number | null;

        private updateState() {
            const s = this.status.state;
            $.set('.st.sttxt', s);
            if (s == 'Alarm' && !this._alarm_int_ref)
                this._alarm_int_ref = setInterval(
                    () => $.tc('.key.arm', 'active'),
                    333
                );
            if (s != 'Alarm' && this._alarm_int_ref) {
                clearInterval(this._alarm_int_ref);
                $.tc('.key.arm', 'active', false);
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

        private parseGrblStatus(data) {
            var codes = data.split(' ');
            for (var code of codes) {
                code = code.toUpperCase();
                if (code == 'G21') this.setMetric(true);
                if (code == 'G20') this.setMetric(false);
                if (code == 'G90') this.setAbsolute(true);
                if (code == 'G91') this.setAbsolute(false);
                if (code == 'M3' || code == 'M4') this.setSpindle(true);
                if (code == 'M5') this.setSpindle(false);
                if (code == 'M7' || code == 'M8') this.setCoolant(true);
                if (code == 'M9') this.setCoolant(false);

                var fMatch = /^F([0-9]+(?:\.[0-9]*)?)$/g.exec(code);
                if (fMatch) this.setFeed(fMatch[1]);
                var sMatch = /^S([0-9]+(?:\.[0-9]*)?)$/g.exec(code);
                if (sMatch) this.setSpindleSpeed(sMatch[1]);
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
                        part = part.substring(4);
                        s.mpos.x = Number(part);
                        s.mpos.y = Number(parts[++i]);
                        s.mpos.z = Number(parts[++i]);
                    } else if (kw.startsWith('WC')) {
                        part = part.substring(kw.length + 1);
                        s.woff ??= new Coordinate();
                        s.woff.x = Number(part);
                        s.woff.y = Number(parts[++i]);
                        s.woff.z = Number(parts[++i]);
                        s.wpos.x = s.mpos.x - s.woff.x;
                        s.wpos.y = s.mpos.y - s.woff.y;
                        s.wpos.z = s.mpos.z - s.woff.z;
                    } else if (kw == 'WPos') {
                        (part = part.substring(4)), (s.wpos.x = Number(part));
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

        private parseMessage(msg) {
            var lines = msg.replace('\r', '\n').split('\n');
            for (var l of lines) {
                l = l.trim();
                var gcodeStatusMatch = /^\[(?:GC:)?(.*)\]$/gi.exec(l),
                    versionMatch =
                        /^\[(?:ver:)?(\d+)\.?(\d+)([a-z]+)(.*):\]$/gi.exec(l),
                    posMatch = /^\<(.*)\>$/gi.exec(l),
                    varMatch = /^\$(\d*)=(\d+\.\d+)/gi.exec(l);
                if (gcodeStatusMatch) {
                    this.parseGrblStatus(gcodeStatusMatch[1]);
                } else if (versionMatch) {
                    this.parseVersion(
                        versionMatch[1],
                        versionMatch[2],
                        versionMatch[3]
                    );
                } else if (posMatch) {
                    var parts = posMatch[1].split('|');
                    this.status.state =
                        parts.length == 1
                            ? parts[0].substring(0, parts[0].indexOf(','))
                            : parts[0];
                    for (var part of parts) this.parseStatus(part);
                } else if (varMatch) {
                    this.parseVar(varMatch[1], varMatch[2]);
                }
            }
        }

        private handleKey(e: Event) {
            var key = e.currentTarget as HTMLElement;
            var _s = (_d) => {
                /* if debug */
                console.log('[PND]', _d);
                /* end */
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
                                _s('G92X' + this.status.wpos.x / 2);
                                break;
                            case 'yh':
                                _s('G92Y' + this.status.wpos.y / 2);
                                break;
                            case 'zh':
                                _s('G92Z' + this.status.wpos.z / 2);
                                break;
                        }
                        break;
                    case 'fd':
                        var f = this.status.feed.current;
                        var fa = Math.pow(10, 0.2);
                        f = Math.round(arg == '+' ? f * fa : f / fa);
                        _s('F' + f.toFixed(3));
                        break;
                    case 'spn':
                        var s = (this.status.spindle.max * Number(arg)) / 100;
                        _s('M3S' + s.toFixed());
                        break;
                    case 'js':
                        this.status.jog.step = Number(arg);
                        this.updateJogStep();
                        break;
                    case 'jf':
                        this.status.jog.feed = Number(arg);
                        this.updateJogFeed();
                        break;
                    case 'jg':
                        var gc =
                            arg +
                            this.status.jog.step.toFixed(3) +
                            'F' +
                            this.status.jog.feed.toFixed(1);
                        _s(
                            this.status.jog.supported
                                ? '$J=G91' + gc
                                : 'G91G1' + gc
                        );
                        break;
                    case 'jh':
                        _s('G90G1' + arg + '0' + this.status.jog.feed);
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
            const x = this.status.jog.axis;
            this.updateJogAxis();
            this.updateJogInputs();
            this.status.jog.deleteValueOnKey =
                $.sng('.ji input.' + x).value ==
                this.status.wpos[x].toFixed(this.status.isMetric ? 2 : 3);
        }

        private handleJogKey(e: Event) {
            var k = e.target as HTMLElement;
            const a = $.which(k, ['x', 'y', 'z']) as 'x' | 'y' | 'z';
            const i = $.sng('.ji input.' + this.status.jog.axis);
            const ia = $.sng('.ji input.' + a);
            const s = this.status;

            switch (
                $.which(k, ['sa', 'lit', 'rld', 'bks', 'abs', 'dec', 'jog'])
            ) {
                case 'sa':
                    if (a == s.jog.axis) return;
                    s.jog.axis = a;
                    this.jogAxisChanged();
                    break;
                case 'rld':
                    ia.value = s.wpos[a];
                    this.updateJogInputs();
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
                    this.jogAbsolute = !this.jogAbsolute;
                    break;

                case 'jog':
                    this.updateJogInputs();
                    const x = $.num.n2u($.sng('.ji  input.x').value);
                    const y = $.num.n2u($.sng('.ji  input.y').value);
                    const z = $.num.n2u($.sng('.ji  input.z').value);

                    const abs = this.jogAbsolute ? 'G90' : 'G91';
                    Ser.sendln(
                        `${s.jog.supported ? '$J=' + abs : abs + 'G1'}${x == '' ? '' : 'X' + x}${y == '' ? '' : 'Y' + y}${z == '' ? '' : 'Z' + z}F${s.jog.feed}`
                    );
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
            this.jogAxisChanged();

            $.do('.ji input', (i) => {
                i.addEventListener('keydown', (e: KeyboardEvent) => {
                    const a = $.which(i, ['x', 'y', 'z']) as 'x' | 'y' | 'z';
                    this.status.jog.axis = a;
                    this.updateJogAxis();

                    if (this.status.jog.deleteValueOnKey) {
                        i.value = '';
                        this.status.jog.deleteValueOnKey = false;
                    }
                });
                i.addEventListener('blur', () => {
                    this.jogAxisChanged();
                });
            });
        }

        private init() {
            this.updateAbsolute();
            this.updateCoolant();
            this.updateCoords();
            this.updateFeed();
            this.updateIsLaser();
            this.updateJogFeed();
            this.updateJogStep();
            this.updateMetric();
            this.updateSpindle();
            this.updateSpindleSpeed();
            this.updateState();

            this.getPos();
            setTimeout(() => this.getVars(), 100);
            setTimeout(
                () => (this._int_ref = setInterval(() => this.ping(), 1000)),
                200
            );
        }

        private destroy() {
            if (this._int_ref !== null) {
                clearInterval(this._int_ref);
                this._int_ref = null;
            }
        }

        public constructor() {
            Ser.onMessage = (m) => this.parseMessage(m);
            Ser.onOpen = () => {
                this.init();
                this.initJog();
            };
            Ser.onClose = () => this.destroy();
            Ser.onError = () => this.destroy();

            Ser.connect();

            $.do('.kb .key', (e) => (e.onclick = (ev) => this.handleKey(ev)));
            $.do(
                '.ji .key',
                (e) => (e.onclick = (ev) => this.handleJogKey(ev))
            );
        }
    }

    globalThis.pendant = new GrblPendant();
});
