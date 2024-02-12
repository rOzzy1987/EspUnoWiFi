(function () {
    class CommonClass {
        id(sel: string): HTMLElement | null {
            return document.getElementById(sel);
        }

        /// Selects elements by selector
        sel(sel: string, root?: HTMLElement): HTMLElement[] {
            const result =
                root == null
                    ? document.querySelectorAll(sel)
                    : root.querySelectorAll(sel);
            const arr = [] as HTMLElement[];
            if (result) {
                result.forEach((e) => arr.push(e as HTMLElement));
            }
            return arr;
        }

        /// Get single element for selector
        sng(sel: string, root?: HTMLElement): HTMLElement | null {
            return root == null
                ? document.querySelector(sel)
                : root.querySelector(sel);
        }
        /// Run action on all elements found by selector
        do(sel: string, op: (e: HTMLElement) => any): any {
            for (var l of this.sel(sel)) {
                op(l);
            }
        }
        /// Toggle class on selected element(s)
        tc(
            sel: string | HTMLElement,
            cssClass: string,
            on: boolean | null = null
        ): any {
            if (typeof sel == 'string')
                return this.do(sel, (e) => this.tc(e, cssClass, on));

            if (on !== false && !sel.classList.contains(cssClass)) {
                sel.classList.add(cssClass);
            } else if (on !== true && sel.classList.contains(cssClass)) {
                sel.classList.remove(cssClass);
            }
        }
        set(sel: string | HTMLElement, txt: string | null) {
            if (typeof sel == 'string')
                return this.do(sel, (e) => this.set(e, txt));

            if (sel.tagName == 'input' || sel.tagName == 'select') {
                (sel as any).value = txt;
            } else sel.innerText = txt;
        }
        seth(sel: string | HTMLElement, html: string) {
            if (typeof sel == 'string')
                return this.do(sel, (e) => this.seth(e, html));
            sel.innerHTML = html;
        }
        which(el: HTMLElement, cs: string[]): string | null {
            for (var c of cs) {
                if (this.is(el, c)) return c;
            }
            return null;
        }
        is(el: HTMLElement, c: string): boolean {
            return el.classList.contains(c);
        }
        num: CommonNumClass = new CommonNumClass();
    }

    class CommonNumClass {
        get dp(): string {
            return (1.2).toFixed(1).substring(1, 2);
        }

        /// Number to universal
        n2u(n: number | string, dec: number = 3): string {
            return (typeof n == 'number' ? n.toFixed(dec) : n).replace(
                this.dp,
                '.'
            );
        }
        u2n(n: string): number {
            return Number(n.replace('.', this.dp));
        }
    }

    globalThis['$'] = new CommonClass();
    var m = document.createElement('div');
    m.classList.add('mnu');
    m.innerHTML = `<a href="serial.html">Serial monitor</a><br><a href="netstat.html">Network settings</a><br><a href="pendant.html">Grbl pendant</a>`;

    window.addEventListener('load', () => document.body.appendChild(m));
})();
