globalThis['$'] = {
    id(sel: string): HTMLElement | null {
        return document.getElementById(sel);
    },
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
    },
    sng(sel: string, root?: HTMLElement): HTMLElement | null {
        return root == null
            ? document.querySelector(sel)
            : root.querySelector(sel);
    },
};
