window.addEventListener('load', async () => {
    interface IWifiNetworkData {
        ssid: string;
        password: string | null;
    }

    interface IWifiStatus {
        ip?: string;
        subnet?: string;
        gateway?: string;
        dns?: string;
        available: string[];
    }

    interface INetstatData {
        ap: IWifiNetworkData;
        client: IWifiNetworkData;
        status: IWifiStatus;
    }

    function loadData(): INetstatData {
        /* debug */
        var data = {
            ap: { ssid: 'EspDebugAp', password: 'Passw0rd' },
            client: { ssid: 'PrettyFly', password: 'ForaWiFi' },
            status: {
                available: ['FulesKancso', 'WifiCola'],
            },
        };
        return data;
        /* else */
        const xhr = new XMLHttpRequest();
        xhr.open('GET', 'netstat', false);
        xhr.send();
        return JSON.parse(xhr.response) as INetstatData;
        /* end */
    }

    const data = loadData();

    const apForm = globalThis.$.sng('form[action=setap]');
    const wfForm = globalThis.$.sng('form[action=setwf]');

    globalThis.$.sng('[name=ssid]', apForm).value = data.ap.ssid;
    globalThis.$.sng('[name=pswd]', apForm).value = data.ap.password;
    globalThis.$.sng('[name=ssid]', wfForm).value = data.client.ssid;
    globalThis.$.sng('[name=pswd]', wfForm).value = data.client.password;

    const avaEl = globalThis.$.sng('select', wfForm);
    for (const w of data.status.available) {
        var e = document.createElement('option');
        e.innerText = w;
        e.value = w;
        avaEl.appendChild(e);
    }

    globalThis.$.sng('[name=ip]').value = data.status.ip ?? '';
    globalThis.$.sng('[name=sm]').value = data.status.subnet ?? '';
    globalThis.$.sng('[name=gw]').value = data.status.gateway ?? '';
    globalThis.$.sng('[name=dns]').value = data.status.dns ?? '';

    const ss = globalThis.$.sel('.see');
    for (var s of ss) {
        const el = globalThis.$.sng('input', s.parentElement);
        s.onclick = () => {
            el.type = el.type == 'password' ? 'text' : 'password';
        };
    }
});
