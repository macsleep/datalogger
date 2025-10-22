
import API from './API.js';

const api = new API();

new Vue({
	el: '#app',

	data() {
		return {
			intervalId: null,
			authenticated: false,

			dateBrowser: null,
			dateRTC: null,
			timerMinutes: null,
			wifiSSID: null,
			wifiPassword: null,
			httpUser: null,
			httpPassword: null,
			modbusSlots: null,
			config: {
				slot: null,
				deviceAddress: null,
				functionCode: null,
				registerAddress: null,
				valueType: null,
			},
			modbusValue: null,
			serial1Baud: null,
			serial1Config: null,
			firmware: null,
		}
	},

	mounted() {
		this.intervalId = setInterval(this.getDate, 1000);
	},

	beforeUnmount() {
		if (this.intervalId) {
			clearInterval(this.intervalId);
		}
	},

	created() {
		this.getTimer();
		this.getModbus();
		this.getSerial1Settings();
		this.getFirmwareVersion();
	},

	computed: {
		isDisabled() {
			return !this.authenticated
		}
	},

	watch: {
		authenticated(newValue, oldValue) {
			if (newValue) this.getSystemSettings();
		},
	},

	methods: {
		getDate() {
			api.getRTC()
				.then(data => {
					this.dateBrowser = new Date().toUTCString();
					this.dateRTC = new Date(data.epoch * 1000).toUTCString();
				});
		},

		syncronizeDate() {
			let seconds = Math.round(new Date().getTime() / 1000);
			api.putRTC(seconds);
		},

		getTimer() {
			api.getTimer()
				.then(data => this.timerMinutes = data.minutes);
		},

		setTimer() {
			api.putTimer(Number(this.timerMinutes))
				.then(this.getTimer());
		},

		getSystemSettings() {
			api.getSystem()
				.then(data => {
					this.wifiSSID = data.wifiSSID;
					this.wifiPassword = data.wifiPassword;
					this.httpUser = data.httpUser;
					this.httpPassword = data.httpPassword;
				});
		},

		setSystemSettings() {
			api.putSystem(this.wifiSSID, this.wifiPassword, this.httpUser, this.httpPassword)
				.then(this.getSystemSettings());
		},

		getModbus() {
			api.getModbus()
				.then(data => {
					this.modbusSlots = data.slots;
				});
		},

		getConfig() {
			api.getConfig(this.config.slot)
				.then(data => {
					this.config.deviceAddress = data.deviceAddress;
					this.config.functionCode = data.functionCode;
					this.config.registerAddress = data.registerAddress;
					this.config.valueType = data.valueType;
					this.modbusValue = null;
				});
		},

		setConfig() {
			api.putConfig(
				this.config.slot,
				Number(this.config.deviceAddress),
				Number(this.config.functionCode),
				Number(this.config.registerAddress),
				this.config.valueType)
				.then(this.getConfig());
		},

		getValue() {
			api.getValue(this.config.slot)
				.then(data => {
					this.modbusValue = data.value;
				})
		},

		getSerial1Settings() {
			api.getSerial1()
				.then(data => {
					this.serial1Baud = data.baud;
					this.serial1Config = data.config;
				});
		},

		setSerial1Settings() {
			api.putSerial1(Number(this.serial1Baud), String(this.serial1Config).toUpperCase())
				.then(this.getSerial1Settings());
		},

		getFirmwareVersion() {
			api.getFirmware()
				.then(data => this.firmware = data.version);
		},

		doLogin() {
			api.getLogin()
				.then(data => this.authenticated = data);
		},
	}
});
