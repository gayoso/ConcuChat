import os
path = os.path.dirname(os.path.abspath(__file__))

lines = []

pathLogServer = path + '/server/bin/Debug/log.txt'
logServer = open(pathLogServer)
for line in logServer:
	if line[0] == '/':
		continue
	else:
		lines.append(line)
logServer.close()
os.remove(pathLogServer)

pathLogClient = path + '/cliente/bin/Debug/log.txt'
logClient = open(pathLogClient)
for line in logClient:
	if line[0] == '/':
		continue
	else:
		lines.append(line)
logClient.close()
os.remove(pathLogClient)

lines.sort()

logMerged = open('log_merged.txt', 'w')
for line in lines:
	logMerged.write(line)
logMerged.close()
