

window.addEventListener('load', () => {
    function initEditableDropdowns(){
        const dds = globalThis.$.sel('.ed-dd');
    
        dds.forEach(dd => {
            const sEl = globalThis.$.sel('select', dd)[0] as HTMLSelectElement;
            const tEl = globalThis.$.sel('input', dd)[0] as HTMLInputElement;
    
            sEl.addEventListener('change', (e) => {
                if (tEl.value != sEl.value) {
                    tEl.value = sEl.value;
                    tEl.dispatchEvent(new Event("change"));
                }
            });
            sEl.addEventListener('click', () => {
                sEl.selectedIndex = -1;
            });
        });
    } 

    initEditableDropdowns();
})