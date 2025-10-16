
axios.defaults.timeout = 60000;
axios.defaults.baseURL = "http://datalogger.local";
axios.defaults.headers.common['Content-Type'] = 'application/json';

new Vue({
	el: '#app',

	data() {
		return {
			authenticated: false,
			firmwareVersion: "???",
		}
	},

	mounted() {
	},

	beforeUnmount() {
	},

	created() {
		this.getFirmware();
	},

	computed: {
	},

	methods: {
		getFirmware() {
			axios.get('/api/firmware')
				.then(response => {
					this.firmwareVersion = response.data.version;
				})
				.catch(error => {
					console.log(error);
				});
		},

		doLogin() {
			axios.post('/api/login', null)
				.then(response => {
					this.authenticated = true;
				})
				.catch(error => {
					this.authenticated = false;
				});
		},

	},
});
