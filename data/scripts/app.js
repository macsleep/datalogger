
axios.defaults.timeout = 60000;
axios.defaults.baseURL = "http://datalogger.local";
axios.defaults.headers.common['Content-Type'] = 'application/json';

new Vue({
	el: '#app',

	data() {
	},

	methods: {
	},
});

