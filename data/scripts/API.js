
class API {
	constructor() {
		axios.defaults.timeout = 60000;
		axios.defaults.baseURL = "http://datalogger.local";
		axios.defaults.headers.common['Content-Type'] = 'application/json';
	}

	getRTC() {
		return axios.get('/api/rtc')
			.then(response => {
				return response.data;
			})
			.catch(error => {
				console.log(error);
			})
	}

	putRTC(epoch) {
		return axios.put('/api/rtc', { epoch: epoch })
			.then(response => {
				console.log(response);
			})
			.catch(error => {
				console.log(error);
			})
	}

	getTimer() {
		return axios.get('/api/timer')
			.then(response => {
				return response.data;
			})
			.catch(error => {
				console.log(error);
			})
	}

	putTimer(minutes) {
		return axios.put('/api/timer', { minutes: minutes })
			.then(response => {
				console.log(response);
			})
			.catch(error => {
				console.log(error);
			})
	}

	getSystem() {
		return axios.get('/api/system')
			.then(response => {
				return response.data;
			})
			.catch(error => {
				console.log(error);
			})
	}

	putSystem(wifiSSID, wifiPassword, httpUser, httpPassword) {
		return axios.put('/api/system', {
				wifiSSID: wifiSSID,
				wifiPassword: wifiPassword,
				httpUser: httpUser,
				httpPassword: httpPassword,
			})
			.then(response => {
				console.log(response);
			})
			.catch(error => {
				console.log(error);
			})
	}

	deleteSystem() {
		return axios.delete('/api/system')
			.then(response => {
				console.log(response);
			})
			.catch(error => {
				console.log(error);
			})
	}

	getFirmware() {
		return axios.get('/api/firmware')
			.then(response => {
				return response.data;
			})
			.catch(error => {
				console.log(error);
			});
	}

	postLogin() {
		return axios.post('/api/login', null)
			.then(response => {
				return true;
			})
			.catch(error => {
				return false;
			});
	}
}

export default API;
