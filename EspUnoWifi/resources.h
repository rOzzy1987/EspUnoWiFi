
/////////////////////////////////////////////////////
//   GENERATED FILE!                               //
//   Do not edit!                                  //
//   Generate using CLI: npm run build             //
/////////////////////////////////////////////////////
#ifndef __HTML_H__
#define __HTML_H__

// file:            pendant.html
// size:                   25143 bytes
// compressed size:        10555 bytes

const char res_pendant_html[] PROGMEM = R"=====(
<!doctype html><html lang=en xmlns=http://www.w3.org/1999/xhtml><meta charset=utf-8><meta http-equiv=pragma content=no-cache><meta http-equiv=cache-control content=no-cache><meta http-equiv=expires content=0><meta name=viewport content="width=device-width,initial-scale=1"><title>GRBL Pendant</title><link rel=preconnect href=https://fonts.googleapis.com><link rel=preconnect href=https://fonts.gstatic.com crossorigin><link href="https://fonts.googleapis.com/css2?family=Workbench&display=swap" rel=stylesheet><script src=serial.js></script><style>*{padding:0;margin:0;box-sizing:border-box;transition:all ease-in-out .2s}body{padding:24px;font-family:sans-serif;font-size:18px;display:flex;justify-content:start;flex-wrap:wrap;flex-direction:column;gap:24px;background:#ccd;width:100svw;min-height:100svh}.led{font-size:14px;line-height:14px;text-transform:uppercase}.led:before{content:'\2B24';color:#300;margin-right:5px}.led.active:before{color:red}#dro{display:flex;flex-wrap:wrap;flex-grow:0;gap:48px;width:100%}#dro .cat{display:flex;flex-direction:column;gap:12px;min-width:280px;background-color:#313;border:3px solid #000;border-radius:16px;padding:16px;box-shadow:inset 3px 3px #fff}#dro .dsp{display:flex;align-items:baseline;justify-content:space-between;gap:24px}#dro .name,#dro .title,#dro .unit{color:#bcd;font-weight:700;width:75px}#dro .title{text-transform:uppercase;font-size:12px;margin-bottom:-24px}#dro .val{flex-grow:1;font-family:Workbench,monospace;font-size:48px;text-align:right;color:#0a0}#kb,#kb .group,#kb .vgroup{display:flex;gap:16px}#kb{flex-wrap:wrap}#kb .vgroup{flex-direction:column}#kb .empty,#kb .key{height:60px;width:60px;font-size:24px;line-height:24px}#kb .key{padding:18px 8px;text-align:center;position:relative;border:1px solid #000;border-radius:4px;background:#dcb;text-transform:uppercase;cursor:pointer}#kb .key:hover{box-shadow:8px 8px 8px rgba(0,0,0,.4)}#kb .key:active{top:4px;left:4px;box-shadow:0 0 8px rgba(0,0,0,.4)}#kb .group.ctrl .key{width:136px}#kb .key sub{font-size:12px}#kb .key.lgt{background:#fed}#kb .key.red{color:#fff;background:#a00}#kb .key.ylw{background:#dd3}#kb .key.lbl{background:#8af}#kb .key.grn{color:#fff;background:#060}#kb .key.onoff:before{content:'\2B24';color:#300;position:absolute;top:-3px;left:3px;font-size:12px}#kb .key.onoff.active:before{color:red}@media (max-width:720px){#dro{gap:12px}#dro .title{display:none;text-transform:uppercase;font-size:12px;margin-bottom:0}#dro .cat{flex-direction:row;flex-wrap:wrap;flex-grow:1}#dro .cat.mchn{display:none}#dro .val{font-size:24px;flex-grow:0}#dro .unit{display:none}#kb,#kb .group,#kb .vgroup{gap:10px}#kb .empty,#kb .key{padding:14px 5px;width:48px;height:48px;font-size:19px;line-height:19px}#kb .group.ctrl .key{width:106px}#kb .key sub{font-size:10px}}</style><div id=dro><div class="cat mchn"><div class=title>Machine</div><div class=dsp><div class=name>X:</div><div class="val st mx">123,34</div><div class="unit l">mm</div></div><div class=dsp><div class=name>Y:</div><div class="val st my">123,34</div><div class="unit l">mm</div></div><div class=dsp><div class=name>Z:</div><div class="val st mz">123,34</div><div class="unit l">mm</div></div></div><div class=cat><div class=title>Work</div><div class=dsp><div class=name>X:</div><div class="val st wx">123,34</div><div class="unit l">mm</div></div><div class=dsp><div class=name>Y:</div><div class="val st wy">123,34</div><div class="unit l">mm</div></div><div class=dsp><div class=name>Z:</div><div class="val st wz">123,34</div><div class="unit l">mm</div></div></div><div class=cat><div class=title>Status</div><div class=dsp><div class=name>Feed:</div><div class="val st feed">123,34</div><div class="unit v">mm/min</div></div><div class=dsp><div class=name>Spindle:</div><div class="val st sspd">65</div><div class="unit sspd">%</div></div><div class=dsp><div class=name>State:</div><div class="val st sttxt">Idle</div></div></div></div><div id=kb><div class=vgroup><div class="group ctrl"><div class="key red" data-gcode=&#x18;>Stop</div><div class="key ylw" data-gcode=!>Pause</div><div class="key ylw" data-gcode=~>Start</div></div><div class=group><div class=vgroup><div class=key data-gcode=G92X0>X<sub>0</sub></div><div class=key data-cmd=wo data-arg=xh>X<sub>1/2</sub></div></div><div class=vgroup><div class=key data-gcode=G92Y0>Y<sub>0</sub></div><div class=key data-cmd=wo data-arg=yh>Y<sub>1/2</sub></div></div><div class=vgroup><div class=key data-gcode=G92Z0>Z<sub>0</sub></div><div class=key data-cmd=wo data-arg=zh>Z<sub>1/2</sub></div></div><div class=vgroup><div class="key lbl" data-cmd=fd data-arg=+>F +</div><div class="key lbl" data-cmd=fd data-arg=->F -</div></div><div class=vgroup><div class="key grn" data-gcode=M5S0>S<sub>0</sub></div><div class="key red" data-cmd=spn data-arg=100>S<sub>100</sub></div></div><div class=vgroup><div class="key ylw" data-cmd=spn data-arg=5>S<sub>5</sub></div><div class="key red" data-cmd=spn data-arg=50>S<sub>50</sub></div></div></div></div><div class=group><div class=vgroup><div class=empty><div class="led st abs">Abs</div><div class="led st spn">Spn</div><div class="led st cln">Cln</div></div><div class="key lgt" data-cmd=jg data-arg=X->&#x25C0;</div><div class=key data-cmd=jh data-arg=X>X<sub>&#x21D2;0</sub></div></div><div class=vgroup><div class="key lgt" data-cmd=jg data-arg=Y>&#x25B2;</div><div class=key data-gcode=$H>&#x2302;</div><div class="key lgt" data-cmd=jg data-arg=Y->&#x25BC;</div></div><div class=vgroup><div class=empty></div><div class="key lgt" data-cmd=jg data-arg=X>&#x25B6;</div><div class=key data-cmd=jh data-arg=Y>Y<sub>&#x21D2;0</sub></div></div><div class=vgroup><div class="key lgt" data-cmd=jg data-arg=Z>Z+</div><div class=key data-cmd=jh data-arg=Z>Z<sub>&#x21D2;0</sub></div><div class="key lgt" data-cmd=jg data-arg=Z->Z-</div></div><div class=vgroup><div class="key onoff" data-cmd=js data-arg=100>100</div><div class="key onoff" data-cmd=js data-arg=10>10</div><div class="key onoff" data-cmd=js data-arg=1>1</div></div><div class=vgroup><div class="key onoff" data-cmd=jf data-arg=100 data-gcode=F100>F<sub>100</sub></div><div class="key onoff" data-cmd=jf data-arg=1000 data-gcode=F1000>F<sub>1k</sub></div><div class="key onoff" data-cmd=jf data-arg=10000 data-gcode=F10000>F<sub>10k</sub></div></div></div><div class=group><div class="key ylw" data-gcode=$X title="Clear alarm">A!</div><div class="key lbl" title=Reconnect data-cmd=conn>&#x267A;</div><div class="key ylw" title=Reset data-cmd=rst>R</div></div></div><script>window.onload=function(){var e=function(e,t){for(var s of document.querySelectorAll(e))t(s)},t=function(s,i){if("string"==typeof s)return e(s,(e=>t(e,i)));i&&!s.classList.contains("active")&&s.classList.add("active"),!i&&s.classList.contains("active")&&s.classList.remove("active")},s=function(t,s){e(t,(e=>{e.innerText=s}))},i={spindle:{isLaser:!0,isActive:!1,speed:0,max:1e3},isMetric:!0,isAbsolute:!0,isCoolantActive:!1,feed:{current:0,max:1e4},mpos:{x:0,y:0,z:0},wpos:{x:0,y:0,z:0},state:"",jog:{supported:!1,step:10,feed:1e3}},n=function(){"Idle"==i.state&&ArduinoSerial.send("$$\n")},o=function(){ArduinoSerial.send("?\n")},r=function(){s(".unit.l",i.isMetric?"mm":"in"),s(".unit.v",i.isMetric?"mm/min":"in/min")},a=function(e){i.isMetric!=e&&(i.isMetric=e,r())},c=function(){t(".st.abs",i.isActive)},d=function(e){i.isAbsolute!=e&&(i.isAbsolute=e,c())},u=function(){t(".st.spn",i.spindle.isActive)},l=function(e){i.spindle.isActive!=e&&(i.spindle.isActive=e,u())},p=function(){t(".st.cln",i.isCoolantActive)},f=function(e){i.isCoolantActive!=e&&(i.isCoolantActive=e,p())},m=function(){s(".st.feed",i.feed.current.toFixed())},v=function(){var e=i.spindle.speed;i.spindle.isLaser&&(e/=i.spindle.max/100),s(".st.sspd",e)},x=function(e){e=Number(e),i.spindle.speed!=e&&(i.spindle.speed=e,v())},b=function(){var e=i.spindle.isLaser?"%":"Rpm";s(".unit.sspd",e)},g=function(){var e=i.isMetric?2:3;s(".st.wx",i.wpos.x.toFixed(e)),s(".st.wy",i.wpos.y.toFixed(e)),s(".st.wz",i.wpos.z.toFixed(e)),s(".st.mx",i.mpos.x.toFixed(e)),s(".st.my",i.mpos.y.toFixed(e)),s(".st.mz",i.mpos.z.toFixed(e))},A=function(){s(".st.sttxt",i.state)},w=function(){e("[data-cmd=js]",(e=>{t(e,e.dataset.arg==i.jog.step)}))},M=function(){e("[data-cmd=jf]",(e=>{t(e,e.dataset.arg==i.jog.feed)}))},y=function(e){var t,s=e.split(" ");for(var n of s){"G21"==n&&a(!0),"G20"==n&&a(!1),"G90"==n&&d(!0),"G91"==n&&d(!1),"M3"!=n&&"M4"!=n||l(!0),"M5"==n&&l(!1),"M7"!=n&&"M8"!=n||f(!0),"M9"==n&&f(!1);var o=/^F([0-9]+(?:\.[0-9]*)?)$/g.exec(n);o&&(t=o[1],t=Number(t),i.feed!=t&&(i.feed.current=t,m()));var r=/^S([0-9]+(?:\.[0-9]*)?)$/g.exec(n);r&&x(r[1])}},h=function(e){for(var t=e.split(","),s=0;s<t.length;s++){var n=t[s];n.startsWith("MPos:")?(n=n.substring(5),i.mpos.x=Number(n),i.mpos.y=Number(t[++s]),i.mpos.z=Number(t[++s])):n.startsWith("WPos:")?(n=n.substring(5),i.wpos.x=Number(n),i.wpos.y=Number(t[++s]),i.wpos.z=Number(t[++s])):i.state=n}g(),A()},j=function(e,t){switch(t=Number(t),Number(e)){case 32:!function(e){i.spindle.isLaser!=e&&(i.spindle.isLaser=e,b())}(1==t),v();break;case 30:i.spindle.max=t,v()}},F=function(e){var t=e.currentTarget,s=e=>{ArduinoSerial.send(e+"\n")};t.dataset.hasOwnProperty("gcode")&&s(t.dataset.gcode+"\n");if(t.dataset.hasOwnProperty("cmd")){var n=t.dataset.cmd,o=t.dataset.arg;switch(n){case"wo":switch(o){case"xh":s("G92X"+i.wpos.x/2);break;case"yh":s("G92Y"+i.wpos.y/2);break;case"zh":s("G92Z"+i.wpos.z/2)}break;case"fd":var r=i.feed.current,a=Math.pow(10,.2);s("F"+(r=Math.round("+"==o?r*a:r/a)).toFixed(3));break;case"spn":s("M3S"+(i.spindle.max*Number(o)/100).toFixed());break;case"js":i.jog.step=Number(o),w();break;case"jf":i.jog.feed=Number(o),M();break;case"jg":var c=o+i.jog.step.toFixed(3)+"F"+i.jog.feed.toFixed(1);s(i.jog.supported?"$J="+c:"G91G1"+c);break;case"jh":s("G90G1"+o+"0");break;case"conn":ArduinoSerial.connect();break;case"rst":ArduinoSerial.reset()}}},S=function(){"Idle"==i.state&&ArduinoSerial.send("$G\n"),setTimeout(o,50)},k=null,N=function(){null!==k&&(clearInterval(k),k=null)};ArduinoSerial.onMessage=function(e){var t=e.replace("\r","\n").split("\n");for(var s of t){s=s.trim();var i=/^\[(.*)\]$/gi.exec(s);i&&y(i[1]);var n=/^\<(.*)\>$/gi.exec(s);n&&h(n[1]);var o=/^\$([0-9]*)=([0-9]+(?:\.[0-9]+))/gi.exec(s);o&&j(o[1],o[2])}},ArduinoSerial.onOpen=function(){c(),p(),g(),m(),b(),M(),w(),r(),u(),v(),A(),o(),setTimeout(n,100),setTimeout((()=>k=setInterval(S,1e3)),200)},ArduinoSerial.onClose=N,ArduinoSerial.onError=N,ArduinoSerial.connect(),e(".key",(e=>e.onclick=F))}</script>
)=====";


// file:             serial.html
// size:                   11021 bytes
// compressed size:         5520 bytes

const char res_serial_html[] PROGMEM = R"=====(
﻿<!doctype html><html lang=en xmlns=http://www.w3.org/1999/xhtml><meta charset=utf-8><meta http-equiv=pragma content=no-cache><meta http-equiv=cache-control content=no-cache><meta http-equiv=expires content=0><meta name=viewport content="width=device-width,initial-scale=1"><title>Serial Monitor</title><style>*{box-sizing:border-box;margin:0;padding:0}html{height:100svh;overflow-y:scroll}body{display:flex;flex-direction:column;justify-content:space-between;gap:0;height:100svh;font-family:sans-serif}#msgBar{display:flex;flex-direction:row;justify-content:space-between;gap:24px;padding:8px 24px;width:100%;background-color:#5f9ea0}#msgBar>div{display:flex;flex-direction:row;align-items:center;gap:8px}#msgDiv{flex-grow:1;white-space:pre;font-family:monospace;padding:2px 8px;overflow:scroll}.ed-dd{width:120px;height:24px;position:relative}.ed-dd *{position:absolute;top:0;left:0}.ed-dd select{width:120px}.ed-dd input{width:100px}.baud-label{font-size:12pt;margin-right:-18px;height:24px;background:#fff;color:gray;padding:2px;padding-right:10px;border-radius:4px}input,select{font-size:12pt;white-space:pre;border:none;padding:2px;height:24px;border-radius:4px}input:focus,select:focus{outline:0}.svg_icon{cursor:pointer;width:24px;height:24px;transition:fill-opacity .5s;-moz-transition:fill-opacity .5s;-webkit-transition:fill-opacity .5s;-o-transition:fill-opacity .5s;vertical-align:middle;fill:#006461;fill-opacity:0.7}.svg_icon:hover{fill-opacity:1}#msg{width:100%}.msgl{clear:both}.msgl .stamp{opacity:.3;padding-right:8px;float:left}.msgl .msg{display:inline-block}#ovrly{display:flex;flex-direction:column;align-items:center;justify-content:center;gap:16px;position:fixed;width:100svW;height:100svh;background:rgba(0,0,0,.5);color:#fff;z-index:100}#ovrly a:active,#ovrly a:link,#ovrly a:visited{color:#fff;background-color:#5f9ea0;text-decoration:none;padding:8px 24px;font-size:16pt;border-radius:20px}@media(max-width:700px){#msgBar{flex-direction:column-reverse;gap:8px;padding:8px 12px}.baud-label{display:none}#msg{width:100%;height:36px;font-size:16pt}}@media (max-width:400px){.msgl .stamp{display:none}}</style><body><svg xmlns=http://www.w3.org/2000/svg version=1.1 style=display:none><symbol id=svg_sendtext viewBox="0 0 512 512"><path d="M224 387.814v124.186l-192-192 192-192v126.912c223.375 5.24 213.794-151.896 156.931-254.912 140.355 151.707 110.55 394.785-156.931 387.814z"></path></symbol><symbol id=svg_resetuno viewBox="0 0 512 512"><path d="M444.84 83.16c-46.804-51.108-114.077-83.16-188.84-83.16-141.385 0-256 114.615-256 256h48c0-114.875 93.125-208 208-208 61.51 0 116.771 26.709 154.848 69.153l-74.848 74.847h176v-176l-67.16 67.16z"></path><path d="M464 256c0 114.875-93.125 208-208 208-61.51 0-116.771-26.709-154.847-69.153l74.847-74.847h-176v176l67.16-67.16c46.804 51.108 114.077 83.16 188.84 83.16 141.385 0 256-114.615 256-256h-48z"></path></symbol></svg><div id=msgBar><div style=flex-grow:1><input id=msg placeholder="Send message"></div><div><span class=baud-label>Baud rate:</span> <span class=ed-dd><select id=baud><option>300<option>1200<option>2400<option>4800<option>9600<option>14400<option>19200<option>28800<option>38400<option>57600<option>74880<option selected>115200<option>128000<option>230400<option>250000<option>1000000<option>2000000</select> <input id=baudtxt> </span><select id=lef><option value=->No line ending<option value=n selected>Newline<option value=r>Carriage return<option value=rn>Both NL&CR</select> <svg id=send class=svg_icon onclick=sendmsg()><use xlink:href=#svg_sendtext width=24px height=24px></use></svg> <svg id=reset class=svg_icon onclick=ArduinoSerial.reset()><use xlink:href=#svg_resetuno width=24px height=24px></use></svg></div></div><div id=msgDiv></div><div id=ovrly><p>Disconnected.</p><a href=# onclick="return ArduinoSerial.connect(),!1">Reconnect?</a></div><script src=serial.js></script><script>window.onload=function(){var e=document.getElementById("baudtxt"),n=document.getElementById("baud"),t=document.getElementById("msg"),o=document.getElementById("msgDiv"),i=document.getElementById("ovrly");e.onchange=function(){ArduinoSerial.setBaud(this.value)},n.onchange=function(){e.value!=this.value&&(e.value=this.value,e.onchange())},n.onclick=function(){n.selectedIndex=-1},t.onkeyup=function(e){13==e.keyCode&&d()};var d=function(){var e=t.value,n=document.getElementById("lef").value;if(e){switch(n){case"n":e+="\n";break;case"r":e+="\r";break;case"rn":e+="\n\r"}ArduinoSerial.send(e),t.focus(),t.select()}},a=function(){document.title="Serial Monitor Disconnected",i.style.cssText=""},l=function(e){var n=o,t=n.scrollHeight-n.clientHeight-n.scrollTop,i=new Date,d=document.createElement("span");d.classList.add("stamp"),d.innerHTML=i.getHours().toFixed().padStart(2,"0")+":"+i.getMinutes().toFixed().padStart(2,"0")+":"+i.getSeconds().toFixed().padStart(2,"0");var a=document.createElement("span");a.classList.add("msg"),a.innerText=e;var l=document.createElement("div");if(l.classList.add("msgl"),l.appendChild(d),l.appendChild(a),n.appendChild(l),t<1.5*l.offsetHeight){if(n.childNodes.length>300)for(;n.childNodes.length>200;)n.removeChild(n.childNodes[0]);n.scrollTop=n.scrollHeight}};ArduinoSerial.onError=a,ArduinoSerial.onClose=a,ArduinoSerial.onOpen=function(e){var n=document.location.hostname;l(`----------- Connected to ${n} -----------`),document.title=`Serial Monitor at ${n}`,i.style.display="none"},ArduinoSerial.onMessage=l,ArduinoSerial.getBaud((function(n){e.value=n})),ArduinoSerial.connect()}</script>
)=====";


// file:               serial.js
// size:                    3458 bytes
// compressed size:         1453 bytes

const char res_serial_js[] PROGMEM = R"=====(
(function(){var l=function(){},b={readyState:-1},c=document.location.hostname,a=function(){l('[AS] Opening WebSocket...');b=new WebSocket(`ws://${c}:81`, 'arduino');var A=null,_=window.ArduinoSerial,B=function(_A){A!==null&&(_A=A+_A);var _b=_A.substr(_A.length-1);if(_b!='\n'&&_b!='\r'){A=_A;return}A=null;_.onMessage(_A)};b.onerror=C=>{console.error('[AS] WebSocket error!',C);typeof _.onError=='function'&&_.onError(C)};b.onclose=_a=>{l('[AS] WebSocket closed');typeof _.onClose=='function'&&_.onClose(_a)};b.onopen=D=>{l('[AS] WebSocket opened');typeof _.onOpen=='function'&&_.onOpen(D)};b.onmessage=E=>{if(typeof _.onMessage!='function')return;try {var d=E.data;if(typeof (d)=='string')B(d);else{var _c=new FileReader();_c.onload=()=>B(_c.result);_c.readAsText(E.data)}} catch (aA) {console.error(aA);b.close()}}},e=function(aB){if(b.readyState!=1)return;b.send(aB)},f=function(aC,_B){var _C=new XMLHttpRequest();_C.onload=()=>{l('[AS] Baud rate',aC);_B!==void 0&&_B(_C.responseText)};aC&&(aC=parseInt(aC,10),aC>=9600&&(url+=`?baudrate=${aC}`));_C.open('GET','/getbaudrate');_C.send()},g=function(){},h=function(aD){f(aD);l('[AS] Setting baud rate',aD)},i=function(){l('[AS] Resetting Arduino');var aE=new XMLHttpRequest();aE.open('POST','/reset',!1);aE.send()};window.ArduinoSerial={connect:a};window.ArduinoSerial.connect=a;window.ArduinoSerial.send=e;window.ArduinoSerial.getBaud=g;window.ArduinoSerial.setBaud=h;window.ArduinoSerial.reset=i})();
)=====";


// file:            netstat.html
// size:                    3063 bytes
// compressed size:         1310 bytes

const char res_netstat_html[] PROGMEM = R"=====(
﻿<!doctype html><html lang=en xmlns=http://www.w3.org/1999/xhtml><meta charset=utf-8><title>OSEPP Esp8266 Uploder for Uno</title><style>.select-editable{position:relative;background-color:#fff;border:solid grey 1px;width:120px;height:18px}.select-editable select{position:absolute;top:0;left:0;font-size:14px;border:none;width:120px;margin:0}.select-editable input{position:absolute;top:0;left:0;width:100px;padding:1px;font-size:12px;border:none}.select-editable input:focus,.select-editable select:focus{outline:0}</style><form action=setap method=post><table><tr><td style=width:150px>AP SSID<td><input name=ssid value=%s><tr><td>AP PSWD<td><input name=pswd type=password placeholder="new password"><tr><td>&nbsp;<td><input type=submit value=update></table></form><br><form action=setwf method=post><table><tr><td style=width:150px>WIFI SSID<td><div class=select-editable><select onchange='document.getElementById("wifissid").value=this.value' onclick="this.selectedIndex=-1">%s</select> <input id=wifissid name=ssid value=%s></div><tr><td>WIFI Password<td><input name=pswd type=password placeholder="Your WIFI Password"><tr><td>&nbsp;<td><input type=submit value=update></table></form><br><table><tr><td style=width:150px>Local IP<td>%s<tr><td>subnetMask<td>%s<tr><td>gateway<td>%s<tr><td>dns<td>%s</table>
)=====";


// file:              redir.html
// size:                     463 bytes
// compressed size:          238 bytes

const char res_redir_html[] PROGMEM = R"=====(
<!doctype html><html lang=en-US><meta http-equiv=refresh content="10;url=netstat">ok[<span id=totalSecond>0</span>]<script>var second=0;setInterval((()=>{totalSecond.innerText=++second,11==second&&(location.href="netstat")}),1e3)</script>
)=====";


// file:              index.html
// size:                    3045 bytes
// compressed size:         1277 bytes

const char res_index_html[] PROGMEM = R"=====(
<!doctype html><html lang=en xmlns=http://www.w3.org/1999/xhtml><meta charset=utf-8><title>OSEPP Esp8266 Uploder for Uno</title><style>.select-editable{position:relative;background-color:#fff;border:solid grey 1px;width:120px;height:18px}.select-editable select{position:absolute;top:0;left:0;font-size:14px;border:none;width:120px;margin:0}.select-editable input{position:absolute;top:0;left:0;width:100px;padding:1px;font-size:12px;border:none}.select-editable input:focus,.select-editable select:focus{outline:0}</style><form action=setap method=post><table><tr><td style=width:150px>AP SSID<td><input name=ssid value=%s><tr><td>AP PSWD<td><input name=pswd type=password placeholder="new password"><tr><td>&nbsp;<td><input type=submit value=update></table></form><br><form action=setwf method=post><table><tr><td style=width:150px>WIFI SSID<td><div class=select-editable><select onchange='document.getElementById("wifissid").value=this.value'>%s</select> <input id=wifissid name=ssid value=%s></div><tr><td>WIFI Password<td><input name=pswd type=password placeholder="Your WIFI Password"><tr><td>&nbsp;<td><input type=submit value=update></table></form><br><table><tr><td style=width:150px>Local IP<td>%s<tr><td>subnetMask<td>%s<tr><td>gateway<td>%s<tr><td>dns<td>%s</table>
)=====";



#endif