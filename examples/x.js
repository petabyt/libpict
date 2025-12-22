import { LibPict } from "libpict";

let r = new LibPict();
try {
	r.connect();
	let di = r.getDeviceInfo();
	console.log(JSON.stringify(di));
	r.disconnect();
} catch (e) {
	console.log("Error: ", e);
}
