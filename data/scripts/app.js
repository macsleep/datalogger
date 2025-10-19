
import API from './API.js';

const api = new API();

new Vue({
	el: '#app',

	data() {
		intervalId: null;
		return {
			dateBrowser: null,
			dateRTC: null,
			timerMinutes: null,
			wifiSSID: null,
			wifiPassword: null,
			httpUser: null,
			httpPassword: null,
			authenticated: false,
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
		this.getFirmwareVersion();
		this.getTimer();
	},

	computed: {
		isDisabled() {
			return !this.authenticated
		}
	},

	watch: {
		authenticated(newValue, oldValue) {
			if (newValue) this.getSystemSettings();
		}
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
			api.putTimer(Number(this.timerMinutes));
		},

		getSystemSettings() {
			api.getSystem()
				.then(data => {
					this.wifiSSID = data.wifiSSID;
					this.wifiPassword = data.wifiPassword;
					this.httpUser = data.httpUser;
					this.httpPassword = data.httpPassword;
				})
		},

		setSystemSettings() {
			api.putSystem(this.wifiSSID, this.wifiPassword, this.httpUser, this.httpPassword);
		},

		getFirmwareVersion() {
			api.getFirmware()
				.then(data => this.firmware = data.version);
		},

		doLogin() {
			api.postLogin()
				.then(data => this.authenticated = data);
		},
	}
});
