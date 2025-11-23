function iserr(x)
	if type(x) == number then
		return x < 0
	end
	return false
end

function main()
	print("Hello, World")
	r = ptp.connect()
	if iserr(r) then
		print("Failed to connect: ", r)
		return
	end

	if iserr(r:openSession()) then return end

	info = r:getDeviceInfo()
	if iserr(info) then return end
	print(string.format("Connected to '%s'", info["model"]))

	r:disconnect()
end

main()
