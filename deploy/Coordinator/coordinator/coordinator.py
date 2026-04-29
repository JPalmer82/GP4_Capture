from flask import Flask, request, jsonify
import subprocess

SESSION_NAME_KEY = "SESSION_NAME"
SESSION_SEARCH_ID_KEY = "SESSION_SEARCH_ID"
nextAvailablePort = 7777

app = Flask(__file__)

def CreateServerLocalTest(SessionName, SessionSearchID):
    global nextAvailablePort
    subprocess.Popen(
        [
            "D:/UnrealSRC/Unreal561/Engine/Binaries/Win64/UnrealEditor.exe",
            "D:/JP/Capture_JP/Capture_JP.uproject",
            "-server",
            "-log",
            '-epicapp="Server"',
            f'-PORT="{nextAvailablePort}"',
            f'-{SESSION_NAME_KEY}="{SessionName}"'
            f'-{SESSION_SEARCH_ID_KEY}={SessionSearchID}'
        ]
    )

    usedPort = nextAvailablePort
    nextAvailablePort += 1
    return usedPort


@app.route('/Session', methods = ['POST'])
def CreateServer():
    print(dict(request.headers))
    sessionName = request.get_json().get(SESSION_NAME_KEY)
    sessionSearchID = request.get_json().get(SESSION_SEARCH_ID_KEY)

    port = CreateServerLocalTest(sessionName, sessionSearchID)
    return jsonify({"status":"success", "PORT":port}), 200

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=80)