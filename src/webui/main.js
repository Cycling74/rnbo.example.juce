import { getSliderState, getToggleState } from 'juce-framework-frontend';

function bindSliderParam(name, sliderId, valueId) {
    const slider  = document.getElementById(sliderId);
    const display = document.getElementById(valueId);
    const state   = getSliderState(name);

    state.valueChangedEvent.addListener(() => {
        const norm = state.getNormalisedValue();
        slider.value = norm;
        slider.style.setProperty('--fill', (norm * 100) + '%');
        display.textContent = state.getScaledValue().toFixed(3);
    });

    slider.addEventListener('mousedown',  () => state.sliderDragStarted());
    slider.addEventListener('touchstart', () => state.sliderDragStarted(), { passive: true });
    slider.addEventListener('mouseup',    () => state.sliderDragEnded());
    slider.addEventListener('touchend',   () => state.sliderDragEnded());

    slider.addEventListener('input', () => {
        const norm = parseFloat(slider.value);
        state.setNormalisedValue(norm);
        slider.style.setProperty('--fill', (norm * 100) + '%');
        display.textContent = state.getScaledValue().toFixed(3);
    });
}

function bindToggleParam(name, toggleId) {
    const toggle = document.getElementById(toggleId);
    const state  = getToggleState(name);

    state.valueChangedEvent.addListener(() => {
        toggle.checked = state.getValue();
    });

    toggle.addEventListener('change', () => {
        state.setValue(toggle.checked);
    });
}

bindSliderParam('kink1', 'slider-kink1', 'val-kink1');
bindSliderParam('kink2', 'slider-kink2', 'val-kink2');
bindSliderParam('kink3', 'slider-kink3', 'val-kink3');
bindToggleParam('automate', 'toggle-automate');
