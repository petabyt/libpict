function iserr(x)
	if type(x) == number then
		return x < 0
	end
	return false
end

function main()
	print("Hello, World")
	r, rc = ptp.connect()
	if rc ~= 0 then
		print("Failed to connect: ", r)
		return
	end

	rc = r:openSession()
	if rc ~= 0 then return end

	info, rc = r:getDeviceInfo()
	if rc ~= 0 then return end
	print(string.format("Connected to '%s'", info["model"]))

	asd, rc = r:sendOperation(0x1001, {})
	if rc ~= 0 then return end

	r:disconnect()
end

main()
