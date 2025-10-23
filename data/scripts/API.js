
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

	getYears() {
		return axios.get('/api/logs')
			.then(response => {
				return response.data;
			})
			.catch(error => {
				console.log(error);
			});
	}

	getMonths(year) {
		const url = '/api/logs/' + year;
		return axios.get(url)
			.then(response => {
				return response.data;
			})
			.catch(error => {
				console.log(error);
			});
	}

	getDays(year, month) {
		const url = '/api/logs/' + year + '/' + month;
		return axios.get(url)
			.then(response => {
				return response.data;
			})
			.catch(error => {
				console.log(error);
			});
	}

	getLogfile(year, month, day) {
		const url = '/api/logs/' + year + '/' + month + '/' + day;
		return axios.get(url)
			.then(response => {
				return response.data;
			})
			.catch(error => {
				console.log(error);
			});
	}

	deleteLogfile(year, month, day) {
		const url = '/api/logs/' + year + '/' + month + '/' + day;
		return axios.delete(url)
			.then(response => {
				console.log(response);
			})
			.catch(error => {
				console.log(error);
			});
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

	getSerial1() {
		return axios.get('/api/serial1')
			.then(response => {
				return response.data;
			})
			.catch(error => {
				console.log(error);
			})
	}

	putSerial1(baud, config) {
		return axios.put('/api/serial1', {
			baud: baud,
			config: config,
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

	getModbus() {
		return axios.get('/api/modbus')
			.then(response => {
				return response.data;
			})
			.catch(error => {
				console.log(error);
			});
	}

	getConfig(slot) {
		return axios.get('/api/modbus/' + slot + '/config')
			.then(response => {
				return response.data;
			})
			.catch(error => {
				console.log(error);
			});
	}

	putConfig(slot, deviceAddress, functionCode, registerAddress, valueType) {
		return axios.put('/api/modbus/' + slot + '/config', {
			deviceAddress: deviceAddress,
			functionCode: functionCode,
			registerAddress: registerAddress,
			valueType: valueType,
		})
			.then(response => {
				console.log(response);
			})
			.catch(error => {
				console.log(error);
			})
	}

	getValue(slot) {
		return axios.get('/api/modbus/' + slot)
			.then(response => {
				return response.data;
			})
			.catch(error => {
				console.log(error);
			});
	}

	getLogin() {
		return axios.get('/api/login')
			.then(response => {
				return true;
			})
			.catch(error => {
				return false;
			});
	}
}

export default API;
