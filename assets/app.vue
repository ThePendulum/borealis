<template>
    <div>
        <div class="panel" id="hue">
            <select id="hueMode" class="control input mode" :value="hueMode" @change="updateHueMode">
                <option value="0">Mono</option>
                <option value="1">Rainbow</option>
                <option value="2">Spectrum</option>
                <option value="3">RGB</option>
            </select>

            <div class="control hue">
                <input type="number" min="0" max="360" :value="hue" class="input" @input="updateHue">

                <div class="control-picker" :style="hueGradient">
                    <input type="range" min="0" max="360" :value="hue" class="control-slider" @input="updateHue">
                </div>
            </div>

            <div class="control hue-speed">
                <input type="number" min="=-1" max="1" step="0.01" :value="hueSpeed.toFixed(2)" class="input" @input="updateHueSpeed">

                <div class="control-picker control-speed">
                    <input type="range" min="-1" max="1" step="0.01" :value="hueSpeed" class="control-slider" @input="updateHueSpeed">
                </div>

                <input type="button" value="0" class="button" @click="updateHueSpeed(0)">
            </div>
        </div>

        <div class="panel" id="saturation">
            <select id="saturationMode" class="control input mode" :value="saturationMode" @change="updateSaturationMode">
                <option value="0" selected="selected">Mono</option>
            </select>

            <div class="control saturation">
                <input type="number" min="0" max="1" step="0.01" :value="saturation.toFixed(2)" class="input" @input="updateSaturation">

                <div class="control-picker" :style="saturationGradient">
                    <input type="range" min="0" max="1" step="0.01" :value="saturation" class="control-slider" @input="updateSaturation">
                </div>
            </div>

            <div class="control saturation-speed">
                <input type="number" min="-1" max="1" step="0.01" :value="saturationSpeed.toFixed(2)" class="input" @input="updateSaturationSpeed">

                <div class="control-picker control-speed">
                    <input type="range" min="-1" max="1" step="0.01" :value="saturationSpeed" class="control-slider" @input="updateSaturationSpeed">
                </div>

                <input type="button" value="0" class="button" @click="updateSaturationSpeed(0)">
            </div>
        </div>

        <div class="panel" id="value">
            <select id="valueMode" class="control input mode" :value="valueMode" @change="updateValueMode">
                <option value="0" selected="selected">Mono</option>
                <option value="1">Chase</option>
                <option value="2">Strobe</option>
            </select>

            <div class="control value">
                <input type="number" min="0" max="1" step="0.01" :value="value.toFixed(2)" class="input" @input="updateValue">

                <div class="control-picker" :style="valueGradient">
                    <input type="range" min="0" max="1" step="0.01" :value="value" class="control-slider" @input="updateValue">
                </div>
            </div>

            <div class="control value-speed">
                <input type="number" min="-1" max="1" step="0.01" :value="valueSpeed.toFixed(2)" class="input" @input="updateValueSpeed">

                <div class="control-picker control-speed">
                    <input type="range" min="-1" max="1" step="0.01" :value="valueSpeed" class="control-slider" @input="updateValueSpeed">
                </div>

                <input type="button" value="0" class="button" @click="updateValueSpeed(0)">
            </div>
        </div>
    </div>
</template>

<script>
    import {mapState} from 'vuex';

    import {hsvToHex} from './hsvToRgb.js';

    export default {
        computed: {
            ...mapState(['hue', 'hueMode', 'hueSpeed', 'saturation', 'saturationMode', 'saturationSpeed', 'value', 'valueMode', 'valueSpeed']),
            hueGradient() {
                return `background: linear-gradient(to right, ${hsvToHex(0, this.saturation, this.value)}, ${hsvToHex(60, this.saturation, this.value)}, ${hsvToHex(120, this.saturation, this.value)}, ${hsvToHex(180, this.saturation, this.value)}, ${hsvToHex(240, this.saturation, this.value)}, ${hsvToHex(300, this.saturation, this.value)}, ${hsvToHex(360, this.saturation, this.value)}`;
            },
            saturationGradient() {
                return `background: linear-gradient(to right, ${hsvToHex(this.hue, 0, this.value)}, ${hsvToHex(this.hue, 1, this.value)})`;
            },
            valueGradient() {
                return `background: linear-gradient(to right, ${hsvToHex(this.hue, this.saturation, 0)}, ${hsvToHex(this.hue, this.saturation, 1)})`;
            }
        },
        methods: {
            updateHue(event) {
                this.$store.dispatch('hue', Number(event.target.value));
            },
            updateHueMode(event) {
                this.$store.dispatch('hueMode', Number(event.target.value));
            },
            updateHueSpeed(value) {
                if(value.target) {
                    this.$store.dispatch('hueSpeed', Number(value.target.value));
                } else {
                    this.$store.dispatch('hueSpeed', Number(value));
                }
            },
            updateSaturation(event) {
                this.$store.dispatch('saturation', Number(event.target.value));
            },
            updateSaturationMode(event) {
                this.$store.dispatch('saturationMode', Number(event.target.value));
            },
            updateSaturationSpeed(value) {
                if(value.target) {
                    this.$store.dispatch('saturationSpeed', Number(value.target.value));
                } else {
                    this.$store.dispatch('saturationSpeed', Number(value));
                }
            },
            updateValue(event) {
                this.$store.dispatch('value', Number(event.target.value));
            },
            updateValueMode(event) {
                this.$store.dispatch('valueMode', Number(event.target.value));
            },
            updateValueSpeed(value) {
                if(value.target) {
                    this.$store.dispatch('valueSpeed', Number(value.target.value));
                } else {
                    this.$store.dispatch('valueSpeed', Number(value));
                }
            }
        }
    };
</script>

<style lang="sass">
    .mode {
        width: 100%;
    }

    .control {
        display: flex;
        margin: 0 0 .5rem;

        .input {
            width: 3.5rem;
        }
    }

    .control-picker {
        flex-grow: 1;
    }

    .control-slider {
        background: none;
        -webkit-appearance: none;
        width: calc(100% + .6rem);
        height: 100%;
        padding: .5rem 0;
        margin: 0 0 0 -.3rem;
        cursor: crosshair;

        &::-moz-range-track {
            background: none;
        }

        &::-webkit-slider-thumb {
            -webkit-appearance: none;
            width: 0;
            height: 0;
            border-bottom: solid .6rem #fff;
            border-left: solid .4rem transparent;
            border-right: solid .4rem transparent;
            margin: .5rem 0 -.5rem;
        }
    }

    .control-speed {
        background: repeating-linear-gradient(to right, #222 0, #000 .5%);
    }
</style>
