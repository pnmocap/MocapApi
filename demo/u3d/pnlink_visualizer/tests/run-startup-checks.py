"""Windows smoke checks against the actual production player, with no injected code.

Temporarily changes Visualizer endpoint preferences and restores all preferences
after the player exits. Does not send capture/calibration commands to a device.
"""
import argparse
import ctypes
import json
from pathlib import Path
import socket
import subprocess
import time
import winreg


REGISTRY = r'Software\Noitom\visualizer'


def preferences():
    values = {}
    with winreg.OpenKey(winreg.HKEY_CURRENT_USER, REGISTRY) as key:
        i = 0
        while True:
            try:
                name, value, kind = winreg.EnumValue(key, i)
            except OSError:
                break
            values[name] = (value, kind)
            i += 1
    return values


def restore(values):
    names = preferences()
    with winreg.OpenKey(winreg.HKEY_CURRENT_USER, REGISTRY, 0, winreg.KEY_SET_VALUE) as key:
        for name in names:
            if name not in values:
                winreg.DeleteValue(key, name)
        for name, (value, kind) in values.items():
            winreg.SetValueEx(key, name, 0, kind, value)


def close(process):
    if process.poll() is not None:
        return
    def visit(hwnd, _):
        pid = ctypes.c_ulong()
        ctypes.windll.user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
        if pid.value == process.pid:
            ctypes.windll.user32.PostMessageW(hwnd, 0x10, 0, 0)
        return True
    callback = ctypes.WINFUNCTYPE(ctypes.c_bool, ctypes.c_void_p, ctypes.c_void_p)(visit)
    ctypes.windll.user32.EnumWindows(callback, 0)
    try:
        process.wait(timeout=8)
    except subprocess.TimeoutExpired:
        process.terminate()  # Only the player launched by this script.
        process.wait(timeout=5)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--build', required=True, type=Path)
    ap.add_argument('--output', required=True, type=Path)
    args = ap.parse_args()
    build = args.build.resolve()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    assert not json.loads((build / 'manifest.json').read_text(encoding='utf-8'))['test_harness_included']
    before = preferences()
    results = []
    try:
        # Use a documentation-only address that must be unavailable on this PC.
        for name, ip, port, busy in [('unavailable-ip', '203.0.113.83', 58084, False),
                                    ('valid-native-connection', '127.0.0.1', 58084, False),
                                    ('occupied-port', '127.0.0.1', 58084, True)]:
            with winreg.OpenKey(winreg.HKEY_CURRENT_USER, REGISTRY, 0, winreg.KEY_SET_VALUE) as key:
                winreg.SetValueEx(key, 'UDP.Local.IP_h1684550576', 0, winreg.REG_BINARY, ip.encode() + b'\0')
                winreg.SetValueEx(key, 'UDP.Local.Port_h525594288', 0, winreg.REG_DWORD, port)
                winreg.SetValueEx(key, 'UDP.Remote.IP_h1185147353', 0, winreg.REG_BINARY, b'127.0.0.1\0')
                winreg.SetValueEx(key, 'UDP.Remote.Port_h2135351129', 0, winreg.REG_DWORD, 58085)
            held = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) if busy else None
            process = None
            try:
                if held:
                    held.setsockopt(socket.SOL_SOCKET, socket.SO_EXCLUSIVEADDRUSE, 1)
                    held.bind((ip, port))
                player = build / 'player'
                log = output / (name + '.log')
                process = subprocess.Popen([str(player / 'visualizer.exe'), '-logFile', str(log)], cwd=player)
                time.sleep(7)
                alive = process.poll() is None
                text = log.read_text(encoding='utf-8', errors='replace')
                native = 'Visualizer UDP ready:' in text
                blocked = 'Visualizer connection unavailable:' in text
                titles=[]
                def title_visit(hwnd, _):
                    pid=ctypes.c_ulong()
                    ctypes.windll.user32.GetWindowThreadProcessId(hwnd,ctypes.byref(pid))
                    if pid.value==process.pid and ctypes.windll.user32.IsWindowVisible(hwnd):
                        buf=ctypes.create_unicode_buffer(1024)
                        ctypes.windll.user32.GetWindowTextW(hwnd,buf,len(buf))
                        titles.append(buf.value)
                    return True
                callback=ctypes.WINFUNCTYPE(ctypes.c_bool,ctypes.c_void_p,ctypes.c_void_p)(title_visit)
                ctypes.windll.user32.EnumWindows(callback,0)
                close(process)
                result = {'case': name, 'alive_after_7s': alive, 'native_connection_open': native,
                          'failure_reported': blocked, 'exit_code': process.returncode, 'window_titles': titles}
                result['passed'] = alive and process.returncode == 0 and (
                    native and not blocked if name == 'valid-native-connection' else blocked and not native)
                results.append(result)
            finally:
                if process:
                    close(process)
                if held:
                    held.close()
    finally:
        restore(before)
    result = {'passed': all(x['passed'] for x in results) and len(results) == 3,
              'production_player_no_test_code': True, 'preferences_restored': preferences() == before,
              'device_commands_sent': 0, 'cases': results}
    (output / 'result.json').write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(result, ensure_ascii=False))
    return 0 if result['passed'] and result['preferences_restored'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
